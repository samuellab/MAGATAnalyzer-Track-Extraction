/* 
 * File:   MaggotTrackPoint.cpp
 * Author: Marc
 * 
 * Created on October 30, 2009, 2:25 PM
 * (C) Marc Gershow; licensed under the Creative Commons Attribution Share Alike 3.0 United States License.
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/us/ or send a letter to
 * Creative Commons, 171 Second Street, Suite 300, San Francisco, California, 94105, USA.
 */
#include <limits.h>
#include <vector>
#include <algorithm>
#include <iostream>
#include <sstream>
#include "MaggotTrackPoint.h"
#include "ImTrackPoint.h"
//#include "PointExtractor.h"
#include "cv.h"
#include "ContourPoint.h"
#include "CvUtilsPlusPlus.h"
#include "AutoThresholder.h"
#include "tictoc.h"
#include "MidlineMaggotTrackPoint.h"
using namespace std;

CvMemStorage *MaggotTrackPoint::ms = NULL;


 MaggotTrackPoint::MaggotTrackPoint(double x, double y, double area, double cov[],  int t, CvPoint offset, IplImage *src, int winSize)
        : ImTrackPoint (x,y,area,cov,t,offset,src,winSize) {
     init();
 }
 MaggotTrackPoint::MaggotTrackPoint(double x, double y, double area, double cov[],  int t, int ID, CvPoint offset, IplImage *src, int winSize)
        : ImTrackPoint (x,y,area,cov,t,ID, offset,src,winSize) {
     init();
 }
 MaggotTrackPoint::MaggotTrackPoint(const ImTrackPoint *pt) : ImTrackPoint (pt) {
     init();
 }
 MaggotTrackPoint::MaggotTrackPoint(const MaggotTrackPoint* pt) : ImTrackPoint (pt) {
    targetArea = pt->targetArea;
    maxAngle = pt->maxAngle;
    head = pt->head;
    tail = pt->tail;
    mid = pt->mid;
    threshold = pt->threshold;
    htValid = pt->htValid;
    contour = pt->contour;
    midline = pt->midline;
    prev = NULL;
    next = NULL;
 }

 void MaggotTrackPoint::analyze() {
     _TICTOC_TIC_FUNC;
     stringstream s; s << ("Analyzing MTP ID ");
     s << getID() << " frame " << frameNum << " x " << x << " y " << y << " area " << area;
     message(s.str().c_str(), verb_verbose);
     
     if (prev == NULL) {
         message ("prev point is null", verb_debug);
     } else {
         if (prev->next == NULL) {
             message ("bad linkage: prev->next is NULL!", verb_error);
         } else {
            s.str("");
            s << "prev point is " << prev->getID() << " prev->next is " << prev->next->getID();
            message (s.str().c_str(), verb_debug);
         }
     }

     //reset contour if already exists
     contour.clear();
     thresholdAndContour();
     findHT();
     message("matching HT with Prev", verb_debug);
     matchHTWithPrev();
     message("finding segment beginning", verb_debug);
     MaggotTrackPoint *sb = segmentBeginning();
     if (sb == NULL) {
         message ("segment beginning returned NULL", verb_error);
         sb = this;
     } else {
         message ("found segment beginning", verb_debug);
         s.str("");
         s << "segment beginning ID is " << sb->getID();
         message (s.str().c_str(), verb_debug);
     }
     if (endOfSegment()) {
         message ("this is the end of the segment", verb_debug);
     } else {
         if (next->endOfSegment()) {
             message ("next is end of segment", verb_debug);
         } else {
             message ("neither this nor next is the end of the segment", verb_debug);
         }
     }

     TICTOC::timer().tic("aligning head and tail at each point");
     message("calculating ht dot product", verb_debug);
     double dps = sb->htDPSegment(0);
     s.str("");
     s << "ht dot product = " << dps;
     message (s.str().c_str(), verb_debug);
     if (dps < 0) {
         message ("ht dot product was less than 0", verb_debug);
         sb->flipHTSegment();
         message ("done flipping segment", verb_debug);
     }
     TICTOC::timer().toc("aligning head and tail at each point");
     message ("done with alignment", verb_debug);
     cvClearMemStorage(ms);
     s.str(""); s << ("Done analyzing MTP ID ") << getID();
     message(s.str().c_str(), verb_debug);

     _TICTOC_TOC_FUNC;
 }

 void MaggotTrackPoint::init() {
     head = tail = mid = cvPoint(-1,-1);
     if (ms == NULL) {
         cout << "ms allocated\n";
         ms = cvCreateMemStorage(0);
     } 
     targetArea = -1;
     threshold = -1;
     prev = next = NULL;
     htValid = false;
     maxAngle = 3.141592653/2;
 }
 double MaggotTrackPoint::optimumArea(double minArea, double maxArea, int *thr) {
     _TICTOC_TIC_FUNC;
     cvClearMemStorage(ms);
     IplImage *thresh = NULL;
     stringstream s; s << ("Finding optimum area between ");
     s << minArea << " and " << maxArea;
     message(s.str().c_str(), verb_debug);
     AutoThresholder at(im, &thresh, getLocation(imOrigin), ms);
     at.setMessageHandler(mh);
     int tt;
     if (thr == NULL) {
         thr = &tt;
     }
     targetArea = at.optimumArea((int) (minArea + 0.5), (int) (maxArea + 0.5), thr);
     if (targetArea > 0) {
         threshold = *thr;
     }
     cvClearMemStorage(ms);
     cvReleaseImage(&thresh);
     _TICTOC_TOC_FUNC;
     return targetArea;
 }

 void MaggotTrackPoint::thresholdAndContour() {
     _TICTOC_TIC_FUNC;
     cvClearMemStorage(ms);
     IplImage *thresh = NULL;
//     allocateImage(&thresh, this->im);
     stringstream s; s << ("Threshold and contour with TargetArea ");
     s << targetArea << " and threshold " << threshold;
     message(s.str().c_str(), verb_debug);
     AutoThresholder at(im, &thresh, getLocation(imOrigin), ms);
     at.setMessageHandler(mh);
     if (targetArea > 0) {
        int tstart = (threshold > 0 && threshold < 255) ? (int) threshold : 128;
 
         at.toTargetArea((int) targetArea, tstart, 0, 255);
         
         s.str(""); s << "after threshold to target area, threshold = " << at.getThresh();
         message(s.str().c_str(), verb_debug);
     } else {
         if (threshold < 0 || threshold > 255) {
             message("Maggot Track Point Screw Up: Threshold not set!  Target area not set!", verb_error);
             return;
         }
         cvThreshold(im, thresh, threshold, 1, CV_THRESH_TOZERO);
     }
     CvSeq *c, *start;
     //extract the contour in absolute coordinates, not relative to image origin
     cvFindContours(thresh, ms,  &start, sizeof(CvContour), CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE,imOrigin);
     cvReleaseImage(&thresh);

     c = selectContainingContour(start, getLocation());
     if (c == NULL) {
         c = nearestContour(start, getLocation());
     }

     if (c == NULL) {
         message ("cvFindContours returned NULL!", verb_error);
         area = 0;
         contour.push_back(cvPoint((int) x, (int) y));
     } else {
         area = fabs(cvContourArea(c, CV_WHOLE_SEQ));
         for (int j = 0; j < c->total; ++j) {
             contour.push_back(*(CvPoint *)cvGetSeqElem(c,j));
        }
     }
     _TICTOC_TOC_FUNC;
 }
 void MaggotTrackPoint::findHT() {
     _TICTOC_TIC_FUNC;
     vector <ContourPoint> v;
     //ContourPoint cp;
     int spacing = (int) (contour.size()/6);

     message ("findHT called", verb_debug);
     if (contour.size() <= 6) {
         message ("contour has 6 or fewer points", verb_warning);
         head = cvPoint(INT_MIN, INT_MIN);
         tail = cvPoint(INT_MIN, INT_MIN);
         mid.x = (int) (x + 0.5);
         mid.y = (int) (y + 0.5);
         htValid = false;
         _TICTOC_TOC_FUNC;
         return;
     }

     //calculate the convex hull
     //we won't look for any candiate head/tail locations
     //that aren't on the hull
     //(this means they are in a crook/bend
     //
    int *hull = (int *) cvAlloc (contour.size()*sizeof(int));
    CvMat pt_mat = cvMat(1, contour.size(), CV_32SC2, & (*contour.begin()));
    CvMat hull_mat = cvMat(1, contour.size(), CV_32SC1, hull );
    cvConvexHull2( &pt_mat, &hull_mat, CV_CLOCKWISE, 0 );

     //create contour points, setting angle
     int id, fid, rid;
     for (int j = 0; j < hull_mat.cols; ++j) {
         id = hull[j];
         ContourPoint cp(contour.at(id), id);
         fid = (id + spacing)%contour.size();
         rid = (id - spacing + contour.size())%contour.size();
         cp.setAngle(contour.at(fid), contour.at(rid));
         v.push_back(cp);
     }

     cvFree(&hull);

     sort(v.begin(), v.end(), ContourPoint::sharpestIsFirst);
     vector<ContourPoint>::iterator it1, it2;

     //remove all less sharp points within contour length / 4 from sharpest point
     //then remove all less sharp points within contour length/4 of next sharpest point & so on
     for (it1 = v.begin(); it1 != v.end(); ) {
         int index = it1->getID();
         for (it2 = (++it1); it2 != v.end(); ) {
             if ((it2->getID() + contour.size() - index)%(contour.size()) < contour.size()/4 ||
                     (index - it2->getID() + contour.size())%(contour.size()) < contour.size()/4) {
                 it2 = v.erase(it2);
             } else {
                 ++it2;
             }
         }
     }
     int ncan = 0;
     for (it1 = v.begin(); it1 != v.end(); ++it1) {
         if (it1->getAngle() <= maxAngle) {
            ++ncan;
         }
     }
     //head tail location is valid iff we find exactly two candidate points
     htValid = (ncan == 2);
     int tind, hind;
     //one candidate point: make the other point halfway around the contour
     if (ncan == 1) {
         tind = v.front().getID();
         hind = (tind + contour.size()/2)%contour.size();
     }
     //two candidate points - you're done
     if (ncan == 2) {
         tind = v.front().getID();
         hind = v.at(1).getID();
     }

     //more than two candidates, find the two that are farthest apart
     if (ncan > 2) {
         int *dist = new int[ncan*ncan];
         int j,k;
         for (j = 0; j < ncan; ++j) {
             for (k = 0; k < ncan; ++k) {
                 dist[j + k*ncan] = (v.at(j).getID() + contour.size() - v.at(k).getID())%contour.size();
             }
         }
         tind = hind = 0;
         for (j = 0; j < ncan; ++j) {
             for (k = 0; k < ncan; ++k) {
                 if (dist[j + k*ncan] > (v.at(tind).getID() + contour.size() - v.at(hind).getID())%contour.size()) {
                     tind = j;
                     hind = k;
                 }
             }
         }
         delete [] dist;
     }
     //no candidates, we have a ball probably, but for now just set head/tail to INT_MIN, INT_MIN
     //set mid to COM
     if (ncan == 0) {
         head = cvPoint(INT_MIN, INT_MIN);
         tail = cvPoint(INT_MIN, INT_MIN);
         mid.x = (int) (x + 0.5);
         mid.y = (int) (y + 0.5);
     } else {
         head = contour.at(hind);
         tail = contour.at(tind);
         CvPoint mid1,mid2;
         mid1 = contour.at(((hind + tind)/2)%contour.size());
         mid2 = contour.at(((hind + tind + contour.size())/2)%contour.size());
         mid.x = (mid1.x + mid2.x)/2;
         mid.y = (mid1.y + mid2.y)/2;
         extractMidline(hind, tind);
     }
     stringstream s;
     s << "findHT completed:  mid = " << mid.x << "," << mid.y << ";  head = " << head.x << "," << head.y << "; tail = " << tail.x << "," << tail.y << ";  htValid = " << htValid;
     message (s.str().c_str(), verb_debug);
     _TICTOC_TOC_FUNC;
 }

 void MaggotTrackPoint::extractMidline(int hind, int tind) {
     message ("enter extractMidline", verb_debug);
     std::vector <CvPoint> left;
     std::vector <CvPoint> right;
     std::vector <CvPoint2D32f> mid, midi;
     int numEvenPts = num_mid_pts * 5;
     int ind;
     if (hind < 0 || hind >= contour.size() || tind < 0 || tind >= contour.size()) {
         message("extract midline called with out of range head and/or tail index", verb_warning);
         return;
     }

     stringstream s;
     s << "tind = " << tind << "  hind = " << hind;
     s << "  contour.size = " << contour.size();
     message (s.str().c_str(), verb_debug);

     for (ind = tind; ind != hind; ind = (ind +1)%contour.size()) {
         left.push_back(contour.at(ind));
     }
  
     for (ind = tind; ind != hind; ind = ((ind - 1)%((int) contour.size()) + contour.size()) % contour.size()) {
         right.push_back(contour.at(ind));
     }
     left.push_back(contour.at(hind));
     right.push_back(contour.at(hind));
     message ("created left and right", verb_debug);
     mid = meanOfTwoSequences<CvPoint>(left, right);
     numEvenPts = mid.size() > numEvenPts ? mid.size() : numEvenPts;
     midi = evenSpacePtSequence(mid, 0, numEvenPts);
     double blursigma = numEvenPts/num_mid_pts;
     blursigma = blursigma < 1 ? 1 : blursigma;
     blursigma = blursigma > 7 ? 7 : blursigma;
     midline = resamplePtSequence(convolveVector(midi, gaussKernel(blursigma)), num_mid_pts);

     message ("resampled blurred midline", verb_debug);

     bool debugmidline = false;
     if (debugmidline) {
         message ("entered midline debug", verb_debug);
        IplImage *dst = NULL;
        allocateImage(&dst, cvGetSize(im), IPL_DEPTH_8U, 3);
        if (dst == NULL) return;
        cvConvertImage(im, dst, 0);
        for (vector<CvPoint>::iterator it = left.begin(); it != (left.end() - 1); ++it) {
            cvLine(dst, pointDiff(*it, imOrigin), pointDiff(*(it+1), imOrigin), CV_RGB(0,0,255),1,8,0);
        }
        for (vector<CvPoint>::iterator it = right.begin(); it != (right.end() - 1); ++it) {
            cvLine(dst, pointDiff(*it, imOrigin), pointDiff(*(it+1), imOrigin), CV_RGB(255,0,0),1,8,0);
        }
        if ( !(mid.empty()) ) {
            for (vector<CvPoint2D32f>::iterator it = mid.begin();it != (mid.end() - 1); ++it) {
                cvLine(dst, pointDiff(cvPoint((int) it->x, (int) it->y), imOrigin), pointDiff(cvPoint((int) (it+1)->x, (int) (it+1)->y), imOrigin), CV_RGB(255,255,0) ,1,8,0);
            }
        }
        if ( !(midline.empty()) ) {
            for (vector<CvPoint2D32f>::iterator it = midline.begin();it != (midline.end()); ++it) {
                cvCircle(dst, pointDiff(cvPoint((int) it->x, (int) it->y), imOrigin),1, CV_RGB(255,255,0),-1,8,0);
            }
        }
        cvNamedWindow("midline debug", 0);
        cvShowImage("midline debug", dst);
        cvWaitKey(5);
        cvReleaseImage(&dst);
     }
    message ("exit extractMidline", verb_debug);
 }

MaggotTrackPoint::~MaggotTrackPoint() {
}

void MaggotTrackPoint::matchHTWithPrev() {
    message ("enter matchHTWithPrev", verb_debug);
    if (prev == NULL || prev->midline.size() != midline.size()) return;

    vector<CvPoint2D32f> tmid = midline;
    offsetVectorByPoint(&tmid, cvPoint2D32f (prev->mid.x - mid.x, prev->mid.y - mid.y));    
    double d1 = sumDistance(prev->midline, tmid);
    std::reverse(tmid.begin(), tmid.end());
    double d2 = sumDistance(prev->midline, tmid);

    if (d2 < d1) {
        flipHT();
    }
    stringstream s;
    s << "match HTWithPrev completed:  mid = " << mid.x << "," << mid.y << ";  head = " << head.x << "," << head.y << "; tail = " << tail.x << "," << tail.y << ";  htValid = " << htValid;

    message (s.str().c_str(), verb_debug);
}
void MaggotTrackPoint::flipHT() {
    std::reverse(midline.begin(), midline.end());
    CvPoint temp = head;
    head = tail;
    tail = temp;
}
void MaggotTrackPoint::flipHTSegment() {

    flipHT();
    for (MaggotTrackPoint *mtp = this; !(mtp->endOfSegment()); mtp = mtp->next) {
        if (mtp == NULL) {
            message ("end Of Segment failed to detect that next is null", verb_error);
            return;
        }
        mtp->flipHT();
    }


    //if (endOfSegment()) {
    //    return;
    //} else {
    //    next->flipHTSegment();
   // }
}

double MaggotTrackPoint::htDPSegment(double prevSum) {
    if (prev == NULL) {
        if (next != NULL)   {
            return next->htDPSegment(0);
        } else {
            return 0;
        }
    }
    for (MaggotTrackPoint *mtp = this; !(mtp->endOfSegment()); mtp = mtp->next) {
        if (mtp == NULL) {
            message ("end Of Segment failed to detect that next is null", verb_error);
            return prevSum;
        }
        CvPoint dx = pointDiff(mtp->mid, mtp->prev->mid);
        CvPoint th = pointDiff(mtp->head, mtp->tail);
        prevSum += dotProduct(th, dx);
    }
    return prevSum;
   // if (endOfSegment()) {
 //       return prevSum;
 //   }
 //   if (next == NULL) {
 //       message ("end Of Segment failed to detect that next is null", verb_error);
//        return prevSum;
//    }
//    CvPoint dx = pointDiff(mid, prev->mid);
  //      CvPoint th = pointDiff(head, tail);
    //    prevSum += dotProduct(th, dx);
    // removed normalization 9/14/2010
//    return next->htDPSegment(prevSum);
}

MaggotTrackPoint *MaggotTrackPoint::segmentBeginning() {
    if (prev == NULL || prev->endOfSegment()) {
        return this;
    }
    MaggotTrackPoint *mtp;
    for (mtp = this; mtp->prev != NULL && !(mtp->prev->endOfSegment()); mtp = mtp->prev) {
        ;//intentionally blank
    }
    return mtp;

}

MaggotTrackPoint *MaggotTrackPoint::nextSegment() {
    //if we are at the end of a segment, skip forward to the beginning of the next
    //one
    if (endOfSegment()) {
        MaggotTrackPoint *p = next;
        while (p != NULL) {
            if (p->htValid) return p;
            p = p->next;
        }
        return p;
    } else {
        //find the end of this segment
        MaggotTrackPoint *mtp;
        for (mtp = this; mtp->next != NULL; mtp = mtp->next) {
            if (mtp->next->endOfSegment()) {
                break;
            }
        }
        //then return the next segment
        if (mtp->next != NULL) {
            return mtp->next->nextSegment();
        } else {
            return mtp;
        }
    }
}

bool MaggotTrackPoint::endOfSegment() {
    return (!htValid || next == NULL);
}

static int writeCvPoint (CvPoint pt, FILE *f) {
    int x = pt.x;
    int y = pt.y;
    if (fwrite(&x, sizeof(int), 1, f) != 1) return -1;
    if (fwrite(&y, sizeof(int), 1, f) != 1) return -1;
    return 0;
}

/*
static int writeCvPoint (CvPoint2D32f pt) {
    float x = pt.x;
    float y = pt.y;
    if (fwrite(&x, sizeof(float), 1, f) != 1) return -1;
    if (fwrite(&y, sizeof(float), 1, f) != 1) return -1;
    return 0;
}
*/


static inline int readInt (bool *error, FILE *f) {
    if (*error)
        return 0;
    int x;
    if (fread(&x, sizeof(int), 1, f) != 1){
        *error = true;
        return 0;
    }
    return x;
}
static inline int writeInt (int x, FILE *f) {
    if (fwrite(&x, sizeof(int), 1, f) != 1) return -1;
    return 0;
}

static inline int writeFloat (float x, FILE *f) {
    if (fwrite(&x, sizeof(int), 1, f) != 1) return -1;
    return 0;
}
static inline float readFloat (bool *error, FILE *f) {
    if (*error)
        return 0;
    float x;
    if (fread(&x, sizeof(float), 1, f) != 1){
        *error = true;
        return 0;
    }
    return x;
}
static CvPoint readCvPoint (bool *error, FILE *f) {
    if (*error)
        return cvPoint(0,0);
    /*
    if (fread(&x, sizeof(int), 1, f) != 1){
        *error = true;
        return cvPoint(0,0);
    }
    if (fread(&y, sizeof(int), 1, f) != 1) {
        *error = true;
        return cvPoint(0,0);
    }
     */
    CvPoint pt;
    pt.x = readInt(error, f);
    pt.y = readInt(error, f);
    return pt;
}
string MaggotTrackPoint::saveDescription() {
    string s = ImTrackPoint::saveDescription();
    s.append("(int targetArea) (float threshold) (int head-tail valid) (int int head x, head y) (int int mid) (int int tail) (int n contour points) (n x int int contour points)\n");
    return s;
}
int MaggotTrackPoint::toDisk(FILE *f) {
    ImTrackPoint::toDisk(f);
    //int targetArea, float threshold, int htValid, cvp head, cvp mid, cvp tail, int ncontourpts, cvp[n] contourpts
    if (writeInt((int) targetArea, f) != 0) return -1;
    if (writeFloat(threshold, f) != 0) return -1;
    if (writeInt(htValid ? 1 : 0, f) != 0) return -1;
    if (writeCvPoint(head, f) != 0) return -1;
    if (writeCvPoint(mid, f) != 0) return -1;
    if (writeCvPoint(tail, f) != 0) return -1;
    if (writeInt(contour.size(), f) != 0) return -1;
    int nptswritten = 0;
    for (vector<CvPoint>::iterator it = contour.begin(); it != contour.end(); ++it) {
        if (writeCvPoint(*it,f) != 0) return -1;
        ++nptswritten;
    }
    if (nptswritten != contour.size()) {
        stringstream s; s << "I was suppoed to write " << contour.size() << " contour points, but instead I wrote " << nptswritten;
        message (s.str().c_str(), verb_error);
    }
    return 0;
}
int MaggotTrackPoint::sizeOnDisk() {
    //3 ints, 1 float, (3+n) contour points = 2*int
    return ImTrackPoint::sizeOnDisk() + 3*sizeof(int) + sizeof(float) + (3 + contour.size())*2*sizeof(int);
}



TrackPoint *MaggotTrackPoint::fromDisk(FILE *f) {
    ImTrackPoint *itp = dynamic_cast<ImTrackPoint *> (ImTrackPoint::fromDisk(f));
    if (itp == NULL) {
        return NULL;
    }
    MaggotTrackPoint *mtp = new MaggotTrackPoint (itp);
    delete itp;
    bool error = false;
    mtp->targetArea = readInt(&error, f);
    mtp->threshold = readFloat(&error, f);
    mtp->htValid = (readInt(&error, f) != 0);
    mtp->head = readCvPoint(&error, f);
    mtp->mid = readCvPoint(&error, f);
    mtp->tail = readCvPoint(&error, f);
    int npts = readInt(&error, f);
    for (int j = 0; j < npts; ++j) {
        mtp->contour.push_back(readCvPoint(&error,f));
    }
    return mtp;
}
//returns true on error
bool MaggotTrackPoint::convertITPToMTP (vector<TrackPoint *> *v) {
    vector<TrackPoint *>::iterator it;
    MaggotTrackPoint *mtprev = NULL;
    for (it = v->begin(); it != v->end(); ++it) {
        ImTrackPoint *itp = dynamic_cast<ImTrackPoint *> (*it);
        if (itp == NULL) {
            return true;
        } else {
            //check to see if it's already a maggot track point
            MaggotTrackPoint *mtp = dynamic_cast<MaggotTrackPoint *> (itp);
            //if not, convert it
            if (mtp == NULL) {
                mtp = new MaggotTrackPoint(itp);
                //get rid of old point
                delete itp;
            }
            mtp->linkBehind(mtprev);
            (*it) = mtp;
            mtprev = mtp;
        }
    }
    return false;
}

bool MaggotTrackPoint::analyzeTrack(vector<TrackPoint*>* v, double minArea, double maxArea, double maxAngle, std::string winname) {
    _TICTOC_TIC_FUNC;
    int showEvery = 10;
    if (MidlineMaggotTrackPoint::convertITPToMTP(v)) {
        _TICTOC_TOC_FUNC;
        return true; //error
    }
    if (findAndSetTargetArea(v, minArea, maxArea, maxAngle)) {
        _TICTOC_TOC_FUNC;
        return true;
    }
     vector<TrackPoint *>::iterator it;
     static IplImage *im = NULL;
     bool showim = true;
     for (it = v->begin(); it != v->end(); ++it) {
        MaggotTrackPoint *mtp = dynamic_cast<MaggotTrackPoint *> (*it);
        if (mtp == NULL) return true;
        mtp->analyze();
        if (showim && !winname.empty() && (mtp->idNum%showEvery) == 0) {
            TICTOC::timer().tic("displaying maggot image post analysis");
            mtp->makeMovieFrame(&im);
            cvShowImage(winname.c_str(), im);
            if (cvWaitKey(1) > 0) {
                showim = false; //abort showing image for this track if any key is pressed
            }
            TICTOC::timer().toc("displaying maggot image post analysis");
        }
     }
     _TICTOC_TOC_FUNC;
     return false;
}

bool MaggotTrackPoint::findAndSetTargetArea(vector<TrackPoint*>* v, double minArea, double maxArea, double maxAngle) {
    _TICTOC_TIC_FUNC;
    vector<double> optimumAreas;
    vector<int> thresholds;
    double oa, targetArea;
    int startingThresh;
     vector<TrackPoint *>::iterator it;
     for (it = v->begin(); it != v->end(); ++it) {
       MaggotTrackPoint *mtp = dynamic_cast<MaggotTrackPoint *> (*it);
       if (mtp == NULL) return true;
       int thr;
       oa = mtp->optimumArea(minArea,maxArea,&thr);
       if (oa > 0) {
           optimumAreas.push_back(oa);
           thresholds.push_back(thr);
       }
     }
     if (optimumAreas.empty()) {
         MaggotTrackPoint *mtp = dynamic_cast<MaggotTrackPoint *> (v->front());
         stringstream s;
         s << "optimum area failed for an entire track of length " << v->size();
         mtp->message(s.str().c_str(), v->size() > 20 ? verb_warning : verb_message);
         targetArea = -1;
         startingThresh = (int) mtp->threshold;
     } else {
        MaggotTrackPoint *mtp = dynamic_cast<MaggotTrackPoint *> (v->front());
        stringstream s;
        s << "determined optimum area for " <<optimumAreas.size() << "/" << v->size() << " points\n";
        mtp->message(s.str().c_str(), verb_message);
        sort(optimumAreas.begin(), optimumAreas.end());
        sort(thresholds.begin(), thresholds.end());
        targetArea = optimumAreas.at(optimumAreas.size()/2);
        startingThresh = thresholds.at(thresholds.size()/2);
     }
     for (it = v->begin(); it != v->end(); ++it) {
       MaggotTrackPoint *mtp = dynamic_cast<MaggotTrackPoint *> (*it);
       if (mtp == NULL) return true;
       mtp->setAnalysisParams(targetArea, maxAngle, startingThresh);
     }
     _TICTOC_TOC_FUNC;
     return false;
}

int MaggotTrackPoint::makeMovieFrame (IplImage **dst) {
    if (im == NULL) return -1;
    allocateImage(dst, cvGetSize(im), IPL_DEPTH_8U, 3);
    if (*dst == NULL) return -1;
    cvConvertImage(im, *dst, 0);
    CvScalar cc = htValid ? CV_RGB(255,0,0) : CV_RGB(255,0,255);
    for (vector<CvPoint>::iterator it = contour.begin(); it != (contour.end() - 1); ++it) {
        cvLine(*dst, pointDiff(*it, imOrigin), pointDiff(*(it+1), imOrigin), cc,1,8,0);
    }
    if ( !(midline.empty()) ) {
        for (vector<CvPoint2D32f>::iterator it = midline.begin();it != (midline.end() - 1); ++it) {
            cvLine(*dst, pointDiff(cvPoint((int) it->x, (int) it->y), imOrigin), pointDiff(cvPoint((int) (it+1)->x, (int) (it+1)->y), imOrigin), CV_RGB(255,255,0) ,1,8,0);
        }
    }
    cvLine(*dst, pointDiff(contour.front(), imOrigin), pointDiff(contour.back(), imOrigin), cc,1,8,0);
    cvCircle(*dst, pointDiff(head, imOrigin), 2, CV_RGB(0,255,0), -1, 8, 0);
    cvCircle(*dst, pointDiff(tail, imOrigin), 2, CV_RGB(0,0,255), -1, 8, 0);
    return 0;
}
