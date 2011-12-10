/* 
 * File:   mycvutilites.h
 * Author: Marc
 *
 * Created on December 22, 2008, 1:30 PM
 *
 * (C) Marc Gershow; licensed under the Creative Commons Attribution Share Alike 3.0 United States License.
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/us/ or send a letter to
 * Creative Commons, 171 Second Street, Suite 300, San Francisco, California, 94105, USA.
 */

#ifndef _CV_UTILS_PLUS_PLUS_H
#define	_CV_UTILS_PLUS_PLUS_H


#include <cv.h>
#include <cxcore.h>
#include <cxtypes.h>
#include <highgui.h>
#include <assert.h>
#include <vector>

#define BAD_FRAME_VALUE -100
#define FRAME_LOAD_ERROR -101
    
typedef struct _MovieT {
    CvCapture *video;
    CvSize frame_size;
    long int num_frames;
} MovieT;

/*sizeEqual
 *
 * returns 1 iff sz1.width and sz1.height are the same as sz2*/
int sizeEqual (CvSize sz1, CvSize sz2);

/*allocateImage
 *
 *calls cvCreateImage to create an image and its data
 *if *img is already allocated, checks to see if the size, depth, and nchannels are the same
 *if they are, returns without making any changes
 *if they are not, calls cvReleaseImage to free *img's data and allocates new storage*/
void allocateImage (IplImage **img, CvSize size, int depth, int channels);

void allocateImage (IplImage **img, const IplImage *src);
/*void imCopy (const IplImage *src, IplImage **dst)
 *
 *(re)allocates memory for dst, then copies src into dst
 */

void imCopy (const IplImage *src, IplImage **dst);
/*getNumFrames
 *
 *returns the number of frames in a loaded video*/
long int getNumFrames (CvCapture *video);

/*loadFrame
 *
 * loads frame frame_num from movie mov as a single channel 8 bit image
 * if frame_num < 0, loads image from current location and increments current location
 * by 1
 * if *im is correctly allocated, copies frame into image; otherwise releases im's storage
 * and reallocates
 */
int loadFrame (MovieT *mov, long int frame_num, IplImage **im);


int loadMovie (MovieT **mov, const char *fname);

void releaseMovie (MovieT **mov);

/*does a binary threshold on src and stores result in dst
 *new memory is allocated for dst if dst == NULL or is the wrong size or type (old memory is released)
 **/
void threshold (const IplImage *src, IplImage **dst, double thresh);
/*
 *      mode:
 *  * CV_RETR_EXTERNAL - retrive only the extreme outer contours
    * CV_RETR_LIST - retrieve all the contours and puts them in the list
    * CV_RETR_CCOMP - retrieve all the contours and organizes them into two-level hierarchy: top level are external boundaries of the components, second level are bounda boundaries of the holes
    * CV_RETR_TREE - retrieve all the contours and reconstructs the full hierarchy of nested contours
 **/
CvSeq *findContours (const IplImage *src, int mode, CvMemStorage *storage);
CvPoint *contourToPoints (CvSeq *contour);
void blurIm (const IplImage *src, IplImage *dst, double sigma);
double magnitude (CvPoint v);

double angleBetween (CvPoint v1, CvPoint v2);
//returns x1 - x2;
CvPoint pointDiff (CvPoint x1, CvPoint x2);

CvPoint pointSum (CvPoint x1, CvPoint x2);
CvPoint2D32f pointDiff(CvPoint2D32f p1, CvPoint2D32f p2);
double pointDistance (CvPoint2D32f p1, CvPoint2D32f p2);
int dotProduct (CvPoint v1, CvPoint v2);
double sumDistance (const std::vector <CvPoint2D32f> &v1, const std::vector <CvPoint2D32f> &v2);

int convInd (int ind, int j, int anchor, int npts);
void convolveCircular (double *arr, int arr_npts, double *kernel, int kernel_npts, int anchor, double *out);
void gaussK (double *kernel, double sigma);
void dgaussK (double *kernel, double sigma);
void deriv (double *arr, int arr_npts, double sigma, double *out);
void imInfo (IplImage *src, char *msg);
std::string imStrInfo(IplImage *im);
double crossProduct (CvPoint v1, CvPoint v2);

/*loads the pixels specified by r from the file specified by fname
 *loads as a grayscale image
 *allocates memory in dst if not already there
 */
void loadSubImage (const char *fname, IplImage **dst, CvRect r);

/*copies the subimage specified by r into dst, allocating memory if needed*/
void subImage (IplImage *src, IplImage **dst, CvRect r);

/*copies src into dst, allocating memory if needed*/
void cloneImage (const IplImage *src, IplImage **dst);

/*thresholds the image to a target number of nonzero pixels
  src and dst must be different*/
int thresholdToTargetArea (IplImage *src, IplImage *dst, int targetArea, int tlow, int thigh, int tstart);

int thresholdToNumRegions (IplImage *src, IplImage *dst, int nregions, int minArea, int maxArea, int bestArea);

/*double pointDistance (CvPoint p1, CvPoint p2)
 *
 *distance between p1 and p2
 */
double pointDistance (CvPoint p1, CvPoint p2);
/* CvRect rectIntersect (CvRect r1, CvRect r2)
 *
 * returns the intersection of two rectangles
 * if the rectangles do not intersect, one of length or width will be < 0
 *
 */
CvRect rectIntersect (CvRect r1, CvRect r2);

/*IplImage *LVtoIpl (char* lvsrc, int lvlinewidth, int LVWidth, int LVHeight)
 *
 *wraps a labview image into an IPL image
 */
IplImage *LVtoIpl (char* lvsrc, int lvlinewidth, int LVWidth, int LVHeight);
/*void copyAllocatedImage (IplImage *src, IplImage *dst)
 *
 *copies src to dst making sure the rois match
 *if source roi width > dst roi width (etc.) copies a subset of the src
 *if dst roi wider, copies the entire source into a subset of the dst
 *
 *in all cases, upper left corner of roi1 copies to upper left corner of roi2
 */
void copyAllocatedImage (IplImage *src, IplImage *dst);

double contourArea (CvSeq *contour);

CvPoint2D32f centerOfMass (CvSeq *contour);

/* finds the center of mass, weighted by image intensity within the contour
 * bounding box
 *
 */
CvPoint2D32f centerOfMass (CvSeq *contour, const IplImage *src, double *covMat = NULL);

/*void onlyInContour (IplImage *src, IplImage **dst,CvSeq *contour);
 *returns a copy of src
 *copies only the area within the bounding box of contour, subject to
 *all pixels within the contour are unchanged
 *all pixels outside the contour are 0
 */
void onlyInContour (IplImage *src, IplImage **dst,CvSeq *contour, CvPoint offset);

CvSeq *selectContainingContour (CvSeq *firstContour, CvPoint pt);
CvSeq *selectContainingContour (CvSeq *firstContour, CvPoint2D32f pt);
CvSeq *nearestContour (CvSeq *firstContour, CvPoint pt);
CvSeq *nearestContour (CvSeq *firstContour, CvPoint2D32f pt);

/* finds the contours in src
 * then returns a pointer to the contour containing x0, or failing that
 * a pointer to the contour with center of mass nearest x0, or failing that
 * NULL
 *
 * src is altered by the call
 */

CvSeq *findCorrespondingContour (IplImage *src, CvMemStorage *ms, CvPoint2D32f x0);
CvSeq *findCorrespondingContour (IplImage *src, CvMemStorage *ms, CvPoint x0);
bool inRectangle (CvRect r, CvPoint pt);
bool inRectangle (CvRect r, CvPoint2D32f pt);


/* template <class T> std::vector<T> cvSeqToVector (const CvSeq *seq)
 * copies a CvSequence to a stl vector
 * */

template <class T> std::vector<T> cvSeqToVector (const CvSeq *seq);


/* std::vector<CvPoint2D32f> meanOfTwoSequences (CvSeq* seq1, CvSeq* seq2)
 *
 * computes the mean of two sequences (e.g. the line between them) of CvPoints
 * the result is returned as a vector of CvPoint2D32fs
 * 
 * the length of the midline is the same as the length of the longest sequence
 * if the two sequences have different lengths, then we interpolate the shorter sequence
 * to have the same length as the longer sequence
 */

template <typename pttype> std::vector<CvPoint2D32f, std::allocator<CvPoint2D32f> > meanOfTwoSequences (const std::vector<pttype, std::allocator<pttype> > &seq1, const std::vector<pttype, std::allocator<pttype> > &seq2);

/* std::vector<CvPoint2D32f> meanOfTwoSequences (CvSeq* seq1, CvSeq* seq2)
 *
 * computes the mean of two sequences (e.g. the line between them) of CvPoints
 * the result is returned as a vector of CvPoint2D32fs
 *
 * the length of the midline is the same as the length of the longest sequence
 * if the two sequences have different lengths, then we interpolate the shorter sequence
 * to have the same length as the longer sequence
 */

template <class pttype> std::vector<CvPoint2D32f> meanOfTwoSequences (const std::vector<pttype> &seq1, const std::vector<pttype> &seq2);
void combinePointVector (const std::vector<double> &x, const std::vector<double> &y, std::vector<CvPoint2D32f> *ptlist);
template <class pttype> void splitPointVector (const std::vector<pttype> &ptlist, std::vector<double> *x, std::vector<double> *y);
//includes both endpoints
template <class numtype> std::vector<numtype> resampleSequence (const std::vector<numtype> &seq, int newSeqLen);
template <class pttype> std::vector<CvPoint2D32f> resamplePtSequence (const std::vector<pttype> &seq, int newSeqLen);

std::vector<double> convolveVector (const std::vector<double> &x, const std::vector<double> &kernel, int offset);
std::vector<double> convolveVector (const std::vector<double> &x, const std::vector<double> &kernel);
std::vector<CvPoint2D32f> convolveVector (const std::vector<CvPoint2D32f> &x, const std::vector<double> &kernel, int offset);
std::vector<CvPoint2D32f> convolveVector (const std::vector<CvPoint2D32f> &x, const std::vector<double> &kernel);
void offsetVectorByPoint (std::vector<CvPoint2D32f> *v, CvPoint2D32f pt);
std::vector<double> gaussKernel (double sigma);
std::vector<double> dgaussKernel (double sigma);
std::vector<double> derivative (const std::vector<double> &x, double sigma);
void simplePlot (const std::vector<double> &x, IplImage **dst);

//templated functions must be defined in a .h file, so here they are -- aargh

/* template <class T> std::vector<T> cvSeqToVector (const CvSeq *seq)
 * copies a CvSequence to a stl vector
 * */

template <class T> std::vector<T> cvSeqToVector (const CvSeq *seq) {
    std::vector<T> vec;
    for (int j = 0; j < seq->total; ++j) {
        vec.push_back(*(T *) cvGetSeqElem(seq, j));
    }
    return vec;
};


/* std::vector<CvPoint2D32f> meanOfTwoSequences (CvSeq* seq1, CvSeq* seq2)
 *
 * computes the mean of two sequences (e.g. the line between them) of CvPoints
 * the result is returned as a vector of CvPoint2D32fs
 *
 * the length of the midline is the same as the length of the longest sequence
 * if the two sequences have different lengths, then we interpolate the shorter sequence
 * to have the same length as the longer sequence
 */

template <class pttype> std::vector<CvPoint2D32f> meanOfTwoSequences (const CvSeq* seq1, const CvSeq* seq2) {

    return meanOfTwoSequences(cvSeqToVector<pttype>(seq1), cvSeqToVector<pttype>(seq2));

}

/* std::vector<CvPoint2D32f> meanOfTwoSequences (CvSeq* seq1, CvSeq* seq2)
 *
 * computes the mean of two sequences (e.g. the line between them) of CvPoints
 * the result is returned as a vector of CvPoint2D32fs
 *
 * the length of the midline is the same as the length of the longest sequence
 * if the two sequences have different lengths, then we interpolate the shorter sequence
 * to have the same length as the longer sequence
 */

template <class pttype> std::vector<CvPoint2D32f, std::allocator<CvPoint2D32f> > meanOfTwoSequences (const std::vector<pttype, std::allocator<pttype> > &seq1, const std::vector<pttype, std::allocator<pttype> > &seq2) {
    pttype pt1, pt2, pt2a;
    std::vector<CvPoint2D32f> meanseq;
    const std::vector<pttype> *longer, *shorter;

    int j,nlong, nshort, s1, s2;
    double shortindex, r1, r2;
    //arrange things so length(seq1) >= length(seq2)
    if (seq1.size() < seq2.size()) {
        longer = &seq2;
        shorter = &seq1;
    } else {
        longer = &seq1;
        shorter = &seq2;
    }

    nlong = longer->size();
    nshort = shorter->size();
    meanseq.resize(nlong, cvPoint2D32f(0,0));
    for (j = 0; j < nlong; ++j) {
      //  pt1 = (*longer)[j];
        pt1 = longer->at(j);
        shortindex = j * double(nshort)/double(nlong);
        s1 = (int) shortindex;
        s2 = s1 + 1;
//        pt2 = (*shorter)[s1];
        pt2 = shorter->at(s1);
        if (s2 < nshort) {
            // pt2a = (*shorter)[s2];
            pt2a = shorter->at(s2);
             r1 = 0.5*(s2 - shortindex);
             r2 = 0.5 - r1;
             meanseq[j].x = 0.5 * pt1.x + r1 * pt2.x + r2 * pt2a.x;
             meanseq[j].y = 0.5 * pt1.y + r1 * pt2.y + r2 * pt2a.y;
        } else {
            meanseq[j].x = 0.5 * pt1.x + 0.5 * pt2.x;
            meanseq[j].y = 0.5 * pt1.y + 0.5 * pt2.y;
        }
    }
    return meanseq;
}


template <class pttype> void splitPointVector (const std::vector<pttype> &ptlist, std::vector<double> *x, std::vector<double> *y) {
    typedef typename std::vector<pttype>::const_iterator cit;
    cit p;
    x->clear();
    y->clear();
    for (p = ptlist.begin(); p != ptlist.end(); ++p) {
        x->push_back(p->x);
        y->push_back(p->y);
    }
}

//includes both endpoints
template <class numtype> std::vector<numtype> resampleSequence (const std::vector<numtype> &seq, int newSeqLen) {
    int j,ind1,ind2;
    double interpind;
    numtype val;
    std::vector<numtype> resamp;
    for (j = 0; j < newSeqLen; ++j) {
        interpind = j*seq.size()/(newSeqLen-1);
        ind1 = (int) interpind;
        if (ind1 >= seq.size()) {
            val = seq.back();
        } else {
            ind2 = ind1 + 1;
            val = (ind2 - interpind) * seq.at(ind1) + (interpind - ind1) * seq.at(ind2);
        }
        resamp.push_back(val);
    }
    return resamp;
}

/* yi = linearInterpolation (x, y, xi)
//x must be sorted in ascending order, and can not have any duplicates
//this is not checked for
*/
template <class ytype, class xtype> std::vector<ytype> linearInterpolation (const std::vector<xtype> x, const std::vector<ytype> y, const std::vector<xtype> xi) {
    std::vector<ytype> yi;
    typedef typename std::vector<xtype>::const_iterator xcit;
    int i;
    xtype x0, x1, x2, dx;
    ytype y1, y2, dy;
    for (xcit it = xi.begin(); it != xi.end(); ++it) {
        x0 = *it;
        for (i = 1; i < x.size() && x.at(i) < x0; ++i); //go to first point >= xi
        if (i >= x.size() - 1) {
            x2 = x.back();
            x1 = *(x.rbegin() + 1);
            y2 = y.back();
            y1 = *(y.rbegin() + 1);
        } else {
            x1 = x.at(i-1);
            x2 = x.at(i);
            y1 = y.at(i-1);
            y2 = y.at(i);
        }
        dx = x2 - x1;
        dy = y2 - y1;
        
        yi.push_back(y1 + dy * (x0-x1)/dx);
    }
    return yi;
    
}

template <class pttype> std::vector<CvPoint2D32f> resamplePtSequence (const std::vector<pttype> &seq, int newSeqLen) {
    std::vector<double> x, y, xr, yr;
    std::vector<CvPoint2D32f> rseq;
    splitPointVector(seq, &x, &y);
    xr = resampleSequence(x, newSeqLen);
    yr = resampleSequence(y, newSeqLen);
    combinePointVector(xr, yr, &rseq);
    return rseq;
}

/* newSeq = evenSpacePtSequence (seq, newSpacing, numPts
 * resamples seq to have an approximately even distance between the points
 * if newSpacing >= 0, the distance between points is newSpacing
 * if newSpacing <= 0 & numPts > 1, there are numPts evenly spaced points
 * otherwise, the spacing is equal to the minimum distance between pts.
 */

template <class pttype> std::vector<CvPoint2D32f> evenSpacePtSequence (const std::vector<pttype> &seq, double newSpacing = 0, int numPts = 0) {
    typedef typename std::vector<pttype>::const_iterator cit;
    std::vector<double> cumdistance, di;
    std::vector<double> x, y, xi, yi;
    double d, mind = 0;
    cumdistance.push_back(0);
    splitPointVector(seq, &x, &y);

    //calculate cumulative distance along sequence
    for (cit it = seq.begin() + 1; it != seq.end() - 1; ++it) {
        d = pointDistance(*it, *(it+1));
        mind = (d > 0 && d < mind) ? d : mind;
        cumdistance.push_back(cumdistance.back() + d);
    }

    //determine new spacing between points and create interpolated vector
    if (newSpacing <= 0) {
        newSpacing = numPts <= 1 ? mind : cumdistance.back() / (numPts - 1);
    }
    numPts = (int) (cumdistance.back() / newSpacing + 1.0000001);
    d = 0;
    for (int i = 0; i < numPts; ++i) {
        di.push_back(d);
        d += newSpacing;
    }

    //check for and remove duplicate points
    for (int i = cumdistance.size() - 1; i >= 1; --i) {
        if (cumdistance.at(i) <= cumdistance.at(i-1)) {
            cumdistance.erase(cumdistance.begin() + i);
            x.erase(x.begin() + i);
            y.erase(y.begin() + i);
        }
    }


    //interpolate
    xi = linearInterpolation(cumdistance, x, di);
    yi = linearInterpolation(cumdistance, y, di);

    std::vector<CvPoint2D32f> seqi;
    combinePointVector(xi, yi, &seqi);
    return seqi;
}

#endif	
