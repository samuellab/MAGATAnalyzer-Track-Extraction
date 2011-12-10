/*trackpoint.cpp
 *
 * implements the TrackPoint class
 *
 * (C) Marc Gershow; licensed under the Creative Commons Attribution Share Alike 3.0 United States License.
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/us/ or send a letter to
 * Creative Commons, 171 Second Street, Suite 300, San Francisco, California, 94105, USA.
 */
 #include <cmath>
 #include "trackpoint.h"
#include "cv.h"
#include "cxcore.h"
int TrackPoint::lastID = 0;

TrackPoint::~TrackPoint() {
    
}

 TrackPoint::TrackPoint (double x, double y, double area, const double cov[], int t) {
     Init(x,y,area,cov,t,++lastID);
 }


 TrackPoint::TrackPoint (double x, double y, double area, const double cov[], int t, int ID) {
     Init(x,y,area,cov,t,ID);
 }

TrackPoint::TrackPoint (const TrackPoint *pt) {
    if (pt == NULL) {
        Init(0,0,0,NULL,0,-1);
    } else {
        Init(pt->x, pt->y, pt->area, pt->cov, pt->frameNum, pt->idNum);
        setMessageHandler(pt->mh);
    }
}

void TrackPoint::Init (double x, double y, double area, const double cov[], int t, int ID) {
     this->x = x;
     this->y = y;
     this->frameNum = t;
     this->idNum = ID;
     this->area = area;
     if (cov == NULL) {
         this->cov[0] = this->cov[1] = this->cov[2] = this->cov[3] = 0;
     } else {
         setCovariance(cov);
     }
     mh = NULL;
 }

 /*
const ptLoc TrackPoint::getLocation() {
	 ptLoc pt;
	 pt.x = x;
	 pt.y = y;
	 return pt;
 }
 */
 void TrackPoint::setLocation(double x, double y) {
	 this->x = x;
	 this->y = y;
 }
 
 
 void TrackPoint::setFrame(int t) {
	 this->frameNum = t;
 }
 
void TrackPoint::draw(IplImage *dst, bool active, int x0, int y0, int ptrad) {
    if (dst == NULL) {
        return;
    }
    CvScalar color = active ? CV_RGB(0,255,255) : CV_RGB(255,0,0);
    cvCircle(dst, cvPoint((int) (x0 + x +0.5), (int) (y0 + y + 0.5)), ptrad, color, 1, 8, 0);
}
void TrackPoint::drawConnected(IplImage *dst, bool active, int x0, int y0, TrackPoint *pt) {
     if (dst == NULL) {
        return;
    }
    CvScalar color = active ? CV_RGB(0,255,255) : CV_RGB(255,0,0);
    cvLine(dst, cvPoint((int) (x0 + x +0.5), (int) (y0 + y + 0.5)), cvPoint((int) (x0 + pt->x +0.5), (int) (y0 + pt->y + 0.5)), color, 2, 8, 0);
}

/* int toDisk (FILE *f)
 * f is a pointer to a BINARY output file
 * writes the trackpoint to disk in the format
 * int frameNum, float x, float y
 *
 * nonzero return value indicates an error
 *
 * subclasses should overwrite this function to output additional
 * information
 */
int TrackPoint::toDisk(FILE* f) {
    if (f == NULL) {
        return -1;
    }
    float tempf; int tempi; //make sure we have appropriate types if x,y,frameNum change

    tempi = (int) frameNum;
    if (fwrite(&tempi, sizeof(int), 1, f) != 1) {
        return -1;
    }
    tempf = (float) x;
    if (fwrite(&tempf, sizeof(float), 1, f) != 1) {
        return -1;
    }
    tempf = (float) y;
    if (fwrite(&tempf, sizeof(float), 1, f) != 1) {
        return -1;
    }
    tempf = (float) area;
    if (fwrite(&tempf, sizeof(float), 1, f) != 1) {
        return -1;
    }
    float c[4];
    for (int j = 0; j < 4; ++j) {
        c[j] = cov[j];
    }
    if (fwrite(c, sizeof(float), 4, f) != 4) {
        return -1;
    }
    return 0;
}
string TrackPoint::saveDescription() {
    return string ("(int frameNumber) (float x) (float y) (float area) (float covariance[4])\n");
}

int TrackPoint::sizeOnDisk() {
    return (sizeof(int) + 7 * sizeof(float));
}

TrackPoint *TrackPoint::fromDisk(FILE *f) {
    int t;
    float x, y, a, c[4];
    double cov[4];
    if (fread(&t, sizeof(int),1,f) != 1) return NULL;
    if (fread(&x, sizeof(float),1,f) != 1) return NULL;
    if (fread(&y, sizeof(float),1,f) != 1) return NULL;
    if (fread(&a, sizeof(float),1,f) != 1) return NULL;
    if (fread(&c, sizeof(float), 4, f) != 4) return NULL;
    for (int j = 0; j < 4; ++j) {
        cov[j] = c[j];
    }
    return new TrackPoint(x,y,fabs(a),cov,t); //Kludge to fix a previous bug, but shouldn't be harmful
}

YAML::Emitter& TrackPoint::toYAML (YAML::Emitter& out) {
    out << YAML::BeginSeq;
    out << YAML::Key << "frame Number" << frameNum;
}