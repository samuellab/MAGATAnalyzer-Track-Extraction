/* 
 * File:   PointExtractor.cpp
 * Author: Marc
 * 
 * Created on October 21, 2009, 10:47 AM
 * (C) Marc Gershow; licensed under the Creative Commons Attribution Share Alike 3.0 United States License.
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/us/ or send a letter to
 * Creative Commons, 171 Second Street, Suite 300, San Francisco, California, 94105, USA.
 */
#include <assert.h>
#include <vector>
#include <sstream>
#include <string>
#include <ctime>
#include "cv.h"
#include "ctype.h"
#include "trackpoint.h"

#include "PointExtractor.h"
#include "CvUtilsPlusPlus.h"
#include "tictoc.h"
using namespace std;



//inline void extendImVector (vector<IplImage *> *v, int len);

PointExtractor::PointExtractor() {
    init();
}


void PointExtractor::setThresholdCompareIm (const IplImage *src) {
    cloneImage(src,&thresholdCompareIm);
    globalThresh = false;
}

void PointExtractor::setGlobalThreshold (int thresh) {
    stringstream s;
    message ("attempting to set global threshold", verb_message);
    if (analysisRect.width > 0 && analysisRect.height > 0) {
        s << "allocating image current ptr = " << (int) (thresholdCompareIm) << " w = " << analysisRect.x << " h = " << analysisRect.y;
        message(s.str().c_str(), verb_debug);
        allocateImage(&thresholdCompareIm, cvSize(analysisRect.width, analysisRect.height), IPL_DEPTH_8U, 1);
        s.str("");
        s << "setting im to thresh " << thresh;
        message(s.str().c_str(),verb_message);
        cvSet (thresholdCompareIm, cvScalarAll(thresh), NULL);
    } else {
        message ("set Global threshold called, but analysis rect not set yet; not allocating a comparison image", verb_warning);
        stringstream s;
        s << "Analysis Rect: x = " << analysisRect.x << " y = " << analysisRect.y << " width = " << analysisRect.width << " height = " << analysisRect.height << "\n";
        message (s.str().c_str(), verb_warning);
    }
    globalThresh = true;
    this->thresh = thresh;

}

void PointExtractor::calculateBackground() {
  
     if (!firstRun && frameNum <= bakValidUntil) {
     
        return;
     }
     message ("calculating background", verb_message);

    if (fl == NULL) {
        message ("frame loader null, can't load frame", verb_error);
       
        return;
    }
    _TICTOC_TIC_FUNC; //only fire when we are actually calculating background
    int first = frameNum;
    first = (first + bp.resampleInterval <= lastFrame) ? first : lastFrame - bp.resampleInterval;
    first = (first >= firstFrame) ? first : firstFrame;
    
    int last = first + bp.resampleInterval;
    last = (last <= lastFrame) ? last : lastFrame;
    double delta = (last - first) / (bp.nBackgroundFrames - 1);

    if (fnm != _frame_none) {
        double normSum = fl->getFrameNormFactor(first, fnm);
        if (normSum <= 0) {
            message("getFrameNormFactor reports error", verb_error);
        }
        for (int j = 1; j < bp.nBackgroundFrames; j++) {
            stringstream ss;
            ss << "norm sum = " << normSum;
            message (ss.str().c_str(), verb_debug);
            int nf = fl->getFrameNormFactor((int) (first + j*delta + 0.5),fnm);
            if (nf <= 0) {
                message("getFrameNormFactor reports error", verb_error);
            }
            normSum += nf;
        }
        if (normSum <= 0) {
            message ("sum of norm factors gives non positive number", verb_error);
        }
        if (bp.nBackgroundFrames <= 0) {
            message ("number of background frames is nonpositive", verb_error);
        }
        normFactor = (int) (normSum / bp.nBackgroundFrames);
        stringstream ss;
        ss << "norm sum is " << normSum << " nBackgroundFrames is " << bp.nBackgroundFrames << " norm factor is " << normFactor;
        message(ss.str().c_str(), verb_message);
    } else {
        message ("frame normalization is turned off", verb_message);
        normFactor = -1;
    }
    message ("calling get frame", verb_debug);
    if (fl->getFrame(first, &backgroundIm, fnm, normFactor) != 0) {
        message ("frame loader reports error", verb_error);
    }
    
    analysisRect = fl->getAnalysisRect();
    for (int j = 1; j < bp.nBackgroundFrames; j++) {
        if (fl->getFrame((int) (first + j*delta + 0.5), &currentFrame, fnm, normFactor) == 0) {
            if (currentFrame == NULL || backgroundIm == NULL) {
                message ("current frame or background im is NULL", verb_error);
            } else {
               cvMin (currentFrame, backgroundIm, backgroundIm);
            }
        } else {
            message ("frame loader reports error", verb_error);
        }
    }
   
    bakValidUntil = first + bp.resampleInterval;
    
    if (bp.blurSigma > 0) {
        std::stringstream s;
        message ("blurring background ", verb_verbose);
        IplImage *im = checkoutTempIm();
        cloneImage(backgroundIm, &im);
        blurIm(im, backgroundIm, bp.blurSigma);
        checkinTempIm(&im);
    }
    _TICTOC_TOC_FUNC;
}

int PointExtractor::nextFrame() {
    if (!firstRun) {
        frameNum+= increment;
    }
    if (messagesOn) {
        stringstream s; s << "advancing to frame ";
        s<< frameNum;
        message (s.str().c_str(), verb_message);
    }
    cvClearMemStorage(tempStorage);
    firstRun = false;
    return loadFrame(frameNum);
}

IplImage *PointExtractor::generateForeground(int increment) {
    _TICTOC_TIC_FUNC;
    IplImage *dst = NULL;
    nextFrame();
    cloneImage(baksub, &dst);
    clock_t cl = clock();
    this->increment = increment;
    while (nextFrame() == 0) {
        cvMax(baksub, dst, dst);
        if (!(frameNum%(100 * increment))){
            cout << frameNum <<" completed " << increment * (clock() - cl) / frameNum << " ms per frame\n";
        }
    }
    _TICTOC_TOC_FUNC;
    return dst;
}
int PointExtractor::loadFrame(int frameNum) {
    _TICTOC_TIC_FUNC;
    this->frameNum = frameNum;
    if (frameNum > lastFrame)
        return -3;
    if (fl == NULL)
        return -2;
    calculateBackground();
    if (fl->getFrame(frameNum, &currentFrame, fnm, normFactor) != 0) {
        message ("frame Loader returned error", verb_warning);
        _TICTOC_TOC_FUNC;
        return -1;
    }
    assert (currentFrame != NULL);
    analysisRect = fl->getAnalysisRect(); //sync the analysis rectangle with what was actually loaded from the file
    stringstream s;
    s << "loaded image with dims w = " << currentFrame->width << " h = " << currentFrame->height<< " and depth " << currentFrame->depth << " and nchannels = " << currentFrame->nChannels;
    message (s.str().c_str(), verb_debug);

    message("load frame subtracting and thresholding", verb_verbose);
    allocateImage(&baksub, currentFrame);
    allocateImage (&threshIm, currentFrame);
    cvSub(currentFrame, backgroundIm, baksub);
//    blurIm(baksub, baksub, 1);
    

    if (foregroundIm == NULL) {
        cloneImage(baksub, &foregroundIm);
    } else {
        s.str("");
        s << "foreground image has dims w = " << foregroundIm->width << " h = " << foregroundIm->height << " and depth " << foregroundIm->depth << " and nchannels = " << foregroundIm->nChannels;
        message (s.str().c_str(), verb_debug);
        cvMax(baksub, foregroundIm, foregroundIm);
    }
    threshToZero();
    _TICTOC_TOC_FUNC;
    return 0;
}

void PointExtractor::threshToZero() {
    _TICTOC_TIC_FUNC;
    message ("pethreshto0", verb_debug);
    if (globalThresh) {
        message ("global threshold", verb_debug);
        if (bp.blurSigma > 0) {
            IplImage *tmp = checkoutTempIm();
            message ("im checked out", verb_debug);
            allocateImage(&tmp, baksub);
            message ("im allocated", verb_debug);
            blurIm(baksub, tmp, bp.blurSigma);
            message ("im blurred", verb_debug);
            cvThreshold(tmp, threshIm, thresh, 1, CV_THRESH_TOZERO);
            message ("im thresholded", verb_debug);
            checkinTempIm(&tmp);
            message ("im checked in", verb_debug);
        } else {
            cvThreshold(baksub, threshIm, thresh, 1, CV_THRESH_TOZERO);
        }
    } else {
        IplImage *tmp = checkoutTempIm();
        allocateImage(&tmp, baksub);
        if (bp.blurSigma > 0) {
            blurIm(baksub, threshIm, bp.blurSigma);
            cvCmp(threshIm, thresholdCompareIm, tmp, CV_CMP_GE);
        } else {
            cvCmp(baksub, thresholdCompareIm, tmp, CV_CMP_GE);
        }
        cvSetZero(threshIm);
        cvCopy(baksub, threshIm, tmp);
        checkinTempIm(&tmp);
    }
    message ("pethreshto0 done", verb_debug);
    _TICTOC_TOC_FUNC;
}

vector<TrackPoint *> *PointExtractor::getPoints() {
    _TICTOC_TIC_FUNC;
    message ("get points called", verb_debug);
    vector <TrackPoint *> *v = new vector<TrackPoint *>;
    CvSeq *c = findContours();
    ImTrackPoint *itp;
    while (c != NULL) {
        itp = contourToTrackPoint(c);
        if (itp != NULL) {
            v->push_back(itp);
        }
        c = c->h_next;
    }
    if (messagesOn) {
        stringstream s; s << "frame " << frameNum << ": extracted " << v->size() << " new points";
        message (s.str().c_str(), verb_message);
    }
    _TICTOC_TOC_FUNC;
    return v;
}

CvSeq *PointExtractor::findContours() {
    _TICTOC_TIC_FUNC;
    CvSeq *c;
    IplImage *tmp = checkoutTempIm();
    cloneImage(threshIm, &tmp);
//    tempImages.push_back(tmp);
    cvFindContours(tmp, tempStorage, &c, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, cvPoint(0,0));
    checkinTempIm(&tmp);
    _TICTOC_TOC_FUNC;
    return c;
}

ImTrackPoint *PointExtractor::contourToTrackPoint (CvSeq *c) {

    return contourToTrackPointCareful(c);
}

ImTrackPoint *PointExtractor::contourToTrackPointCareful (CvSeq *c) {
    _TICTOC_TIC_FUNC;
   // message("contour to track points called", verb_verbose);
    double area = fabs(cvContourArea(c, CV_WHOLE_SEQ));
    area  = area < c->total ? c->total : area;
    double cov[4];
    if (messagesOn) {
        stringstream s; s<< "CTTP: contour has area " << area;
        message (s.str().c_str(), verb_debug);
    }

    if (area < minArea || area >maxArea) {
        message ("contour rejected as out of size range", verb_debug);
        return NULL;
    }

    IplImage *tmp = checkoutTempIm();
    onlyInContour(threshIm, &tmp, c, cvPoint(-cvBoundingRect(c,0).x, -cvBoundingRect(c,0).y));
    //I think this is correct, but it is very weird and should be rewritten to move the bounding rect part back to onlyInContour
//    tempImages.push_back(tmp);

    CvPoint2D32f pt = centerOfMass(c, tmp, cov);

    ImTrackPoint *itp = new ImTrackPoint ( (pt.x + analysisRect.x ),(pt.y  + analysisRect.y), area, cov, frameNum, cvPoint(analysisRect.x, analysisRect.y), baksub, winSize);
    assert (itp != NULL);
    if (messagesOn) {
        stringstream s; s << "created new track point im coords: " << pt.x << " , " << pt.y << " track coords: " << itp->getLocation().x << " , " << itp->getLocation().y;
        message (s.str().c_str(), verb_debug);
    }
    checkinTempIm(&tmp);
    _TICTOC_TOC_FUNC;
    return itp;
}

vector<TrackPoint *> *PointExtractor::splitPoint(TrackPoint* pt, int npts, int targetArea) {
    _TICTOC_TIC_FUNC;
    CvSeq *c,*start;
    c = start = findContours();
    //look for a contour containing the pt

    c = selectContainingContour(start, pt->getLocation(analysisRect.x, analysisRect.y));

  
    if (c == NULL) {
        message("split point didn't find containing contour point. finding closest", verb_message);
        c = nearestContour(start, pt->getLocation(analysisRect.x, analysisRect.y));
    }
  IplImage *src = NULL, *thr = NULL;// *tmp = NULL;
    CvRect bb;
    subImage(currentFrame, &src, bb = cvBoundingRect(c,0));
    allocateImage(&thr, src);
    //allocateImage(&tmp, src);

    int t = thresholdToNumRegions(src, thr, npts, (int) (targetArea/2), (int) (targetArea*1.5), (int) targetArea);
    stringstream s; s << "threshold to num regions found a threshold of " << t;
    

    cvFindContours(thr, tempStorage, &c, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, cvPoint(bb.x,bb.y));
    vector<TrackPoint *> *v = new vector<TrackPoint *>;
    int nc = 0;
    while (c != NULL) {
        ++nc;
        ImTrackPoint *itp = contourToTrackPointCareful(c);
        if (itp != NULL) {
            v->push_back(itp);
        }
        c = c->h_next;
    }
    s << " " << nc << " regions found";
    message(s.str().c_str(), verb_verbose);
    cvReleaseImage(&thr); cvReleaseImage(&src);
    _TICTOC_TOC_FUNC;
    return v;

}
/*
inline void extendImVector (vector<IplImage *> *v, int len) {
    if (len > v->size())
        v->resize(len, NULL);
}
*/
void PointExtractor::init() {
     IplImage **imsToInit[] = {&backgroundIm, &baksub, &currentFrame, &thresholdCompareIm, &threshIm, &foregroundIm};
     const int nptrs = 6;
     for (int j = 0; j < nptrs; j++) {
         *(imsToInit[j]) = NULL;
     }
     frameNum = 0;
     bakValidUntil = -1;
     fl = NULL;
     tempStorage = cvCreateMemStorage(0);
//     extendImVector(&tempImages, 1);
     tempImages.clear();
     tempImages.push_back(NULL);

     analysisRect.width = analysisRect.height = -1;
     setAreaRange(0,1E4);
     winSize = -1;
     firstFrame = -1;
     lastFrame = -1;
     frameNum = -1;
     mh = NULL;
     firstRun = true;
     increment = 1;
     fnm = _frame_none;
     normFactor = -1;
     ncheckedout = 0;
}
PointExtractor::~PointExtractor() {
    IplImage **imsToRelease[] = {&backgroundIm, &baksub, &currentFrame, &thresholdCompareIm, &threshIm, &foregroundIm};
    IplImage *imptr;
    const int nptrs = 6;
   // cout << "releasing ims\n";
    for (int j = 0; j < nptrs; j++) {
        if (imsToRelease[j] != NULL) cvReleaseImage(imsToRelease[j]);
    }
   // cout << "releasing temp ims\n";
    vector<IplImage *>::iterator it;
    for (it = tempImages.begin(); it != tempImages.end(); ++it) {
        if ((imptr = *it) != NULL) {
            cvReleaseImage(&imptr); //avoid any weirdness with trying to set it = 0 in releaseImage
        }
    }
  //  cout << "deleting frame loader\n";
    if (fl != NULL) delete fl;
//    cout << "releasing mem storage\n";
    cvReleaseMemStorage(&tempStorage);
 //   cout << "done\n";
}

void PointExtractor::setAnalysisRect (CvRect r) {
    stringstream s;
    s << "analysis rectangle set to " << r.x << " " << r.y << " " << r.width << " " << r.height;
    message (s.str().c_str(), verb_message);
    analysisRect = r;
    if (fl != NULL) {
        fl->setAnalysisRect(r);
    }
}
void PointExtractor::setFrameLoader (FrameLoader *fl) {
        clearFrameLoader ();
        this->fl = fl;
        if (analysisRect.height > 0 && analysisRect.width > 0) {
            fl->setAnalysisRect(analysisRect);
        } else {
            analysisRect = fl->getAnalysisRect();
        }
}

int PointExtractor::copyOrigImageOut (IplImage **dst) {
    if (currentFrame == NULL)
        return -1;
    cloneImage(currentFrame, dst);
    return 0;
}
int PointExtractor::copyBSImagOut (IplImage **dst) {
    if (baksub == NULL)
        return -1;
    cloneImage(baksub, dst);
    return 0;
}
 IplImage *PointExtractor::checkoutTempIm() {
     stringstream s;
     IplImage *rv;
     s << "tempImages size is " << tempImages.size();
     message (s.str().c_str(), verb_debug);
     if (tempImages.empty()) {
         rv = NULL;
     } else {
        rv = tempImages.back();
        tempImages.pop_back();
     }
     s.str("");
     message (s.str().c_str(), verb_debug);
     s << "tempImages size is " << tempImages.size();

     ++ncheckedout;
     s.str("");
     s << "checked out a ";
     if (rv == NULL) {
         s << "NULL pointer ";
     } else {
         s << rv->width << " by " << rv->height << "image ";
     }
     s << ncheckedout << " images checked out";
     message (s.str().c_str(), verb_debug);
     return rv;
 }
 void PointExtractor::checkinTempIm(IplImage **im) {
     tempImages.push_back(*im);
     stringstream s;
     s << "checked in a ";
     if (*im == NULL) {
         s << "NULL pointer ";
     } else {
         s << (*im)->width << " by " << (*im)->height << "image ";
     }
     *im = NULL;
     --ncheckedout;
     s << ncheckedout << " images checked out";
    message (s.str().c_str(), verb_debug);
 }

 void PointExtractor::copyOutAutoGeneratedForeground(IplImage** dst) {
     cloneImage(foregroundIm, dst);
 }
 