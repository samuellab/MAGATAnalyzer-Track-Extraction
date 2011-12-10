/* 
 * File:   MidlineMaggotTrackPoint.cpp
 * Author: Marc
 * 
 * Created on September 4, 2010, 12:57 PM
 * (C) Marc Gershow; licensed under the Creative Commons Attribution Share Alike 3.0 United States License.
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/us/ or send a letter to
 * Creative Commons, 171 Second Street, Suite 300, San Francisco, California, 94105, USA.
 */

#include "MidlineMaggotTrackPoint.h"
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

MidlineMaggotTrackPoint::~MidlineMaggotTrackPoint() {

}

 MidlineMaggotTrackPoint::MidlineMaggotTrackPoint(double x, double y, double area, double cov[],  int t, CvPoint offset, IplImage *src, int winSize)
        : MaggotTrackPoint (x,y,area,cov,t,offset,src,winSize) {
     
 }
 MidlineMaggotTrackPoint::MidlineMaggotTrackPoint(double x, double y, double area, double cov[],  int t, int ID, CvPoint offset, IplImage *src, int winSize)
        : MaggotTrackPoint (x,y,area,cov,t,ID, offset,src,winSize) {
    
 }
 MidlineMaggotTrackPoint::MidlineMaggotTrackPoint(const ImTrackPoint *pt) : MaggotTrackPoint (pt) {
     
 }
 MidlineMaggotTrackPoint::MidlineMaggotTrackPoint(const MaggotTrackPoint *pt) : MaggotTrackPoint (pt) {

 }
 string MidlineMaggotTrackPoint::saveDescription() {
    string s = MaggotTrackPoint::saveDescription();
    s.append("(int n midline points) (n x float float midline points)\n");
    return s;
}
 
 static int writeCvPoint (CvPoint pt, FILE *f) {
    int x = pt.x;
    int y = pt.y;
    if (fwrite(&x, sizeof(int), 1, f) != 1) return -1;
    if (fwrite(&y, sizeof(int), 1, f) != 1) return -1;
    return 0;
}


static int writeCvPoint2D32f (CvPoint2D32f pt, FILE *f) {
    float x = pt.x;
    float y = pt.y;
    if (fwrite(&x, sizeof(float), 1, f) != 1) return -1;
    if (fwrite(&y, sizeof(float), 1, f) != 1) return -1;
    return 0;
}



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
    CvPoint pt;
    pt.x = readInt(error, f);
    pt.y = readInt(error, f);
    return pt;
}
static CvPoint2D32f readCvPoint2D32f (bool *error, FILE *f) {
    if (*error)
        return cvPoint2D32f(0,0);
    CvPoint2D32f pt;
    pt.x = readFloat(error, f);
    pt.y = readFloat(error, f);
    return pt;
}
int MidlineMaggotTrackPoint::toDisk(FILE *f) {
    int rv = 0;
    if ((rv = MaggotTrackPoint::toDisk(f)) < 0) return rv;

    if (writeInt(midline.size(), f) != 0) return -1;
    int nptswritten = 0;
    for (vector<CvPoint2D32f>::iterator it = midline.begin(); it != midline.end(); ++it) {
        if (writeCvPoint2D32f(*it,f) != 0) return -1;
        ++nptswritten;
    }
    if (nptswritten != midline.size()) {
        stringstream s; s << "I was suppoed to write " << contour.size() << " contour points, but instead I wrote " << nptswritten;
        message (s.str().c_str(), verb_error);
    }
    return 0;
}
int MidlineMaggotTrackPoint::sizeOnDisk() {
    //1 int (size of midline), (n) midline points = 2*n*float
    return MaggotTrackPoint::sizeOnDisk() + 1*sizeof(int) + 2*sizeof(float)*(midline.size());
}



TrackPoint *MidlineMaggotTrackPoint::fromDisk(FILE *f) {
    MaggotTrackPoint *itp = dynamic_cast<MaggotTrackPoint *> (MaggotTrackPoint::fromDisk(f));
    if (itp == NULL) {
        return NULL;
    }
    MidlineMaggotTrackPoint *mtp = new MidlineMaggotTrackPoint (itp);
    delete itp;
    bool error = false;
    int npts = readInt(&error, f);
    for (int j = 0; j < npts; ++j) {
        mtp->midline.push_back(readCvPoint2D32f(&error,f));
    }
    return mtp;
}
//returns true on error
bool MidlineMaggotTrackPoint::convertITPToMTP (vector<TrackPoint *> *v) {
    vector<TrackPoint *>::iterator it;
    MaggotTrackPoint *mtprev = NULL;
    for (it = v->begin(); it != v->end(); ++it) {
        ImTrackPoint *itp = dynamic_cast<ImTrackPoint *> (*it);
        if (itp == NULL) {
            return true;
        } else {
            //check to see if it's already a maggot track point
            MidlineMaggotTrackPoint *mtp = dynamic_cast<MidlineMaggotTrackPoint *> (itp);
            //if not, convert it
            if (mtp == NULL) {
                mtp = new MidlineMaggotTrackPoint(itp);
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

