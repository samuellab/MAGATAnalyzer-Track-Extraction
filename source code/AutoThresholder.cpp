/* 
 * File:   AutoThresholder.cpp
 * Author: Marc
 * 
 * Created on November 11, 2009, 11:56 AM
 * (C) Marc Gershow; licensed under the Creative Commons Attribution Share Alike 3.0 United States License.
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/us/ or send a letter to
 * Creative Commons, 171 Second Street, Suite 300, San Francisco, California, 94105, USA.
 */
#include <vector>
#include <algorithm>
#include <string>
#include <sstream>
#include <cmath>

#include "AutoThresholder.h"
#include "CvUtilsPlusPlus.h"
#include "tictoc.h"
using namespace std;

AutoThresholder::AutoThresholder(IplImage *src, IplImage **dst, CvPoint2D32f refLocation, CvMemStorage *ms) {
    init(src, dst, refLocation, ms);
    
}
AutoThresholder::AutoThresholder(IplImage* src, IplImage** dst, CvPoint refLocation, CvMemStorage* ms) {
    init (src, dst, cvPoint2D32f(refLocation.x, refLocation.y), ms);
}

void AutoThresholder::init(IplImage* src, IplImage** dst, CvPoint2D32f refLocation, CvMemStorage* ms) {
    this->src = src;
    allocateImage(dst, src);
    this->dst = *dst;
    this->x0 = refLocation;
    if (ms == NULL) {
        this->ms = cvCreateMemStorage(0);
        freeMemStorageOnDestruction = true;
    } else {
        this->ms = ms;
        freeMemStorageOnDestruction = false;
    }
    mh = NULL;
}

int AutoThresholder::toTargetArea(int targetArea, int tstart, int tlow, int thigh)  {
    _TICTOC_TIC_FUNC;
    int t, area;
    //house keeping to make sure tlow < thigh and tstart is between them
    if (thigh < tlow) {
        t = tlow;
        tlow = thigh;
        thigh = t;
    }
    if (tstart < tlow) tstart = tlow;
    if (tstart > thigh) tstart = thigh;
    t = tstart;
    while (1) {
        cvThreshold(src, dst, t, 1, CV_THRESH_TOZERO);
        area = cvCountNonZero(dst);
       // printf ("tlow = %d, thigh =  %d, thresh = %d, area = %d, target area = %d\n",tlow, thigh, t, area);
        if (area < targetArea) {
            //no need to find contour area because it can't be greater than the # of non-zero points
            if (t <= tlow + 1) {
                //thresh = tlow;
                _TICTOC_TOC_FUNC;
                return t;
            }
            thigh = t;
        } else {
            //find contour containing point and determine that contours area 
            CvSeq *c = findCorrespondingContour(dst, ms, x0);//findCorrespondingContour(dst, ms, x0);
            if (c != NULL) {
                area = (int) fabs(cvContourArea(c, CV_WHOLE_SEQ));
            } else {
                area = 0;
            }

            if (area == targetArea) {
                break;
//                return t;
            }
            if (area < targetArea) {
                if (t <= tlow + 1)
                {
                    //t = tlow;
                    break;
                }
                thigh = t;
            } else {
                if (t >= (thigh - 1)) {
                    t = thigh;
                    break;
                }
                tlow = t;
            }
        }
        t = (thigh + tlow) / 2;
    }
    cvThreshold(src, dst, t, 1, CV_THRESH_TOZERO); //rethreshold because finding contours has destroyed dst
    thresh = t;
    _TICTOC_TOC_FUNC;
    return t;

}



/* returns the area of the contour containing our point
 * at all thresholds that produce a nonzero area contour
 *
 */
vector<pair<int,double> > AutoThresholder::allAreas() {
    _TICTOC_TIC_FUNC;
    vector<pair<int,double> > allArea;
    double area;
    //calculate the area for anything with nonzero area
    for (int t = 0; t < 255; t++) {
        cvThreshold(src, dst, t, 1, CV_THRESH_BINARY);        
        area = cvCountNonZero(dst);
        if (area <= 0) {
            break;
        }
        CvSeq *c = findCorrespondingContour(dst, ms, x0);
        if (c != NULL) {
            area = fabs(cvContourArea(c, CV_WHOLE_SEQ));
            if (area <= 0)
                break;
        } else {
            break;
        }
        allArea.push_back(pair<int,double> (t, area));
    }
    _TICTOC_TOC_FUNC;
    return allArea;
}

/* finds the optimum area of the animal, defined as the point that
 * maximizes the smoothed derivative of the logarithm of the area of the contour
 * containing the animal (smallest relative change), subject to these restrictions
 *
 * contour must have area above minArea
 * contour must have area below maxArea
 * threshold may not be at the boundary of allowed values
 * the derivative must be greater than the previous point and the next point
 * (disallows end point problems)
 *
 * optional extra return parameter threshold holds the threshold that produces
 * the optimum area
 */

double AutoThresholder::optimumArea(int minArea, int maxArea, int *threshold) {
    _TICTOC_TIC_FUNC
    double sigma = 3;
    stringstream s;
    vector<pair<int,double> > allArea = allAreas();

    if (allArea.empty()) {
        message ("allAreas returned empty vector", verb_warning);
        return -1;
    }

    vector<double> logArea, dl, ddl;
    int t, firstj, lastj; double area, maxdl;
    for (vector<pair<int,double> >::iterator it = allArea.begin(); it != allArea.end(); ++it) {
        logArea.push_back(log(it->second));
    }
    dl = derivative(logArea, sigma);
    ddl = derivative(dl, 1);
    for (firstj = 0; firstj < allArea.size() && (allArea[firstj].second > maxArea || ddl[firstj] < 0); ++firstj);
    for (lastj = allArea.size() - 1; (lastj >= 0) && (allArea[lastj].second < minArea || ddl[lastj] > 0); --lastj);
    if (firstj >= lastj) {
        s << "couldn't determine valid auto threshold area: ";
        s << "minArea = " << minArea << " maxArea = " << maxArea << " firstArea = " << allArea.front().second << " last Area = " << allArea.back().second << "\n";
        message(s.str().c_str(), verb_verbose);

        *threshold = -1;
        _TICTOC_TOC_FUNC;
        return -1;
    }


    s.str("");
    s << "determing best area between " << allArea[firstj].second << " and " << allArea[lastj].second << " pixels; thresholds are ";
    s << allArea[firstj].first << " and " << allArea[lastj].first;
    message(s.str().c_str(), verb_debug);

    maxdl = dl[firstj];
    *threshold = allArea[firstj].first;
    area = allArea[firstj].second;
    //changed j < lastj to j <= lastj 4/05; 
    for (int j = firstj; j <= lastj; ++j) {
        if (dl[j] > maxdl) {
            maxdl = dl[j];
            *threshold = allArea[j].first;
            area = allArea[j].second;
        }
    }
    _TICTOC_TOC_FUNC;
    return area;
}
double AutoThresholder::optimumArea(int minArea, int maxArea) {
    int threshold;
    return optimumArea(minArea, maxArea, &threshold);
}


struct dlassistT {
    int thresh;
    int area;
    int darea;
    double dlarea;
    static bool comp (const dlassistT d1, const dlassistT d2) {
        return d1.dlarea < d2.dlarea;
    }
};
int AutoThresholder::toMaxDLogArea(int tlow, int thigh) {
    _TICTOC_TIC_FUNC
    vector<dlassistT> v;
    dlassistT d, dlast;
    dlast.area = src->width*src->height;
    tlow = tlow < 0 ? 0 : tlow;
    thigh = thigh > 255 ? 255 : thigh;

    for (int t = tlow; t < thigh; t++) {
        cvThreshold(src, dst, t, 1, CV_THRESH_BINARY);
        d.thresh = t;
        d.area = cvCountNonZero(dst);
        if (d.area <= 0) {
            break;
        }
        CvSeq *c = findCorrespondingContour(dst, ms, x0);
        if (c != NULL) {
            d.area = (int) fabs(cvContourArea(c, CV_WHOLE_SEQ));
        }
        if (d.area <= 0) {
            break;
        }
        d.darea = d.area - dlast.area;
        d.dlarea = 1.0 * d.darea / d.area;
        v.push_back(d);
        dlast = d;
    }
    /*
    stringstream s;
    s << "sorting a vector of dlassistT ";
    for (vector<dlassistT>::iterator it = v.begin(); it != v.end(); ++it) {
        s << "[" << it->area << " " << it->darea << " " << it->dlarea << " " << it->thresh << "]\t";
    }
    message(s.str().c_str(), verb_debug);
     */
    if (v.empty()) {
        thresh = tlow;
        message ("no points in contour at lowest threshold", verb_warning);
        cvThreshold(src, dst, thresh, 1, CV_THRESH_TOZERO);
        _TICTOC_TOC_FUNC;
        return thresh;
    }
    sort (v.begin(), v.end(), dlassistT::comp);
    /*
    s.str(""); s << "result of sort is ";
    for (vector<dlassistT>::iterator it = v.begin(); it != v.end(); ++it) {
        s << "[" << it->area << " " << it->darea << " " << it->dlarea << " " << it->thresh << "]\t";
    }
     message(s.str().c_str(), verb_debug);
     */
    thresh = v.back().thresh;
    cvThreshold(src, dst, thresh, 1, CV_THRESH_TOZERO);
    _TICTOC_TOC_FUNC;
    return thresh;
}

int AutoThresholder::toMaxDLogArea(int range_of_current) {
    return toMaxDLogArea(thresh - range_of_current, thresh + range_of_current);
}
int AutoThresholder::toMaxDLogArea() {
    return toMaxDLogArea(0, 255);
}

AutoThresholder::~AutoThresholder() {
    if (freeMemStorageOnDestruction) {
        cvReleaseMemStorage(&ms);
    }

}

