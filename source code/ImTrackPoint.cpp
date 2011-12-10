/* 
 * File:   ImTrackPoint.cpp
 * Author: Marc
 * 
 * Created on October 26, 2009, 10:00 AM
 * (C) Marc Gershow; licensed under the Creative Commons Attribution Share Alike 3.0 United States License.
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/us/ or send a letter to
 * Creative Commons, 171 Second Street, Suite 300, San Francisco, California, 94105, USA.
 */

#include <string>

#include "ImTrackPoint.h"
#include "cv.h"
#include "cvtypes.h"
#include "cxcore.h"
#include "trackpoint.h"
#include "CvUtilsPlusPlus.h"



ImTrackPoint::~ImTrackPoint() {
   // cout << "itp destructor called\n";
    if (im != NULL) {
       // cout << ". ";
        cvReleaseImage(&im);
    }
}

void ImTrackPoint::setImage(CvPoint offset,IplImage* src, int winSize) {
    CvRect r;
    if (src == NULL)
        return;
    if (winSize <= 0) {
        return;
    }
    winSize = (winSize > src->width) ? src->width : winSize;
    winSize = (winSize > src->height) ? src->height : winSize;
    r.x = (int) (x - offset.x - winSize / 2 + 0.5);
    r.y = (int) (y - offset.y - winSize / 2 + 0.5);
    r.x = (r.x < 0) ? 0 : r.x;
    r.y = (r.y < 0) ? 0 : r.y;

    r.x = (r.x + winSize >= src->width) ? src->width - winSize - 1 : r.x;
    r.y = (r.y + winSize >= src->height) ? src->height - winSize - 1 : r.y;
    r.width = winSize;
    r.height = winSize;

    subImage(src, &im, r);
    imOrigin.x = r.x + offset.x;
    imOrigin.y = r.y + offset.y;
}


ImTrackPoint::ImTrackPoint(double x, double y, double area, double cov[], int t, CvPoint offset, IplImage* src, int winSize) 
        : TrackPoint(x, y,area, cov, t) {
    im = NULL;
    setImage(offset,src, winSize);
}

ImTrackPoint::ImTrackPoint(double x, double y, double area, double cov[], int t, int ID, CvPoint offset, IplImage* src, int winSize)
        : TrackPoint(x, y, area, cov, t, ID) {
    im = NULL;
    setImage(offset,src, winSize);
}

ImTrackPoint::ImTrackPoint(const TrackPoint *pt, const IplImage* src, CvPoint imOrigin)
    : TrackPoint(pt) {
    im = NULL;
    cloneImage(src, &im);
    this->imOrigin = imOrigin;
}

ImTrackPoint::ImTrackPoint(const ImTrackPoint *pt)
    : TrackPoint(pt) {
    im = NULL;
    cloneImage(pt->im, &im);
    this->imOrigin = pt->imOrigin;
}

int ImTrackPoint::copyImageOut (IplImage **dst) {
    if (im == NULL) {
        return -1;
    }
    cloneImage(im, dst);
    return 0;
}
int ImTrackPoint::makeMovieFrame (IplImage **dst) {
    return copyImageOut(dst);
}
string ImTrackPoint::saveDescription() {
    string s = TrackPoint::saveDescription();
    s.append("(int width) (int height) [if no image w = h = 0 and the following are not written]\n");
    s.append("[if image:] (int imOrigin x) (int imOrigin y) (w*h chars image data, row 1, then row 2, etc.)\n");
    return s;
}
int ImTrackPoint::toDisk(FILE *f) {
    if (TrackPoint::toDisk(f) != 0) return -1;

    if (im == NULL) {        
        //no image, but we still need to write that the width and height are zero
        //to avoid problems when loading
        int x[] = {0,0};
        if (fwrite(&x, sizeof(int), 2, f) != 2){
            return -1;
        } else {
            return 0; //count as a successful write if no image is stored
        }
    }
    int w = im->width; int h = im->height;

    //write the width and height of the image as ints
    if (fwrite(&w, sizeof(int), 1, f) != 1) return -1;
    if (fwrite(&h, sizeof(int), 1, f) != 1) return -1;

    //write the image offset as ints
    int x = imOrigin.x, y = imOrigin.y;
    if (fwrite(&x, sizeof(int), 1, f) != 1) return -1;
    if (fwrite(&y, sizeof(int), 1, f) != 1) return -1;

    //write the image data one row at a time
    for (int j = 0; j < h; ++j)  {
        char *ptr = im->imageData + j * im->widthStep;
        if (fwrite(ptr, sizeof(char), w, f) != w) return -1;
    }
    return 0;
}
int ImTrackPoint::sizeOnDisk() {
    if (im == NULL) {
        return TrackPoint::sizeOnDisk() + 2*sizeof(int);
    } else {
        return TrackPoint::sizeOnDisk() + 4*sizeof(int) + im->width*im->height*sizeof(char);
    }
}


TrackPoint *ImTrackPoint::fromDisk(FILE *f) {
    TrackPoint *pt, *impt;
    pt = TrackPoint::fromDisk(f);
    if (pt == NULL) {
        return NULL;
    }
    int w,h;
    if (fread(&w, sizeof(int), 1, f) != 1) {
        delete pt;
        return NULL;
    }
    if (fread(&h, sizeof(int), 1, f) != 1) {
        delete pt;
        return NULL;
    }
    IplImage *im = NULL;
    int x,y;
    bool valid = false;
    if (w !=0 && h != 0) {

        if (fread(&x, sizeof(int), 1, f) != 1) {
            delete pt;
            return NULL;
        }
        if (fread(&y, sizeof(int), 1, f) != 1) {
            delete pt;
            return NULL;
        }
        
        allocateImage(&im, cvSize(w,h), IPL_DEPTH_8U, 1);
        valid = true;
        for (int j = 0; j < h; ++j)  {
            char *ptr = im->imageData + j * im->widthStep;
            if (fread(ptr, sizeof(char), w, f) != w) {
                valid = false;
                break;
            };
        }
    }
    if (!valid) {
        delete pt;
        cvReleaseImage(&im);
        return NULL;
    } else {
        impt = new ImTrackPoint(pt,im, cvPoint(x,y));
        delete pt;
        cvReleaseImage(&im);
        return impt;
    }
}

