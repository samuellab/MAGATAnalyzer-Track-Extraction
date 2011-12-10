/*
 * (C) Marc Gershow; licensed under the Creative Commons Attribution Share Alike 3.0 United States License.
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/us/ or send a letter to
 * Creative Commons, 171 Second Street, Suite 300, San Francisco, California, 94105, USA.
 */

#include <cv.h>
#include <cxcore.h>
#include <cxtypes.h>
#include <cvtypes.h>
#include <stdio.h>
#include <highgui.h>
#include <math.h>
#include <iostream>
#include <assert.h>
#include <vector>
#include <sstream>
#include "CvUtilsPlusPlus.h"

#define _MYCV_DEBUG 0


int sizeEqual (CvSize sz1, CvSize sz2) {
    return (sz1.height == sz2.height && sz1.width == sz2.width);
}

/*z component of v1 cross v2 (x&y are 0)*/
double crossProduct (CvPoint v1, CvPoint v2) {
    return (v1.x*v2.y - v1.y*v2.x);
}
void allocateImage (IplImage **img, CvSize size, int depth, int channels) {
    if (*img == NULL) {
        if (_MYCV_DEBUG) printf  ("allocating a %d by %d img, depth = %d, channels = %d\n", size.width, size.height, depth, channels);
        *img = cvCreateImage (size, depth, channels);
    } else {
        imInfo (*img, "allocateImage: ");
        if (!(sizeEqual(size,cvGetSize(*img)) && depth == (*img)->depth && channels == (*img)->nChannels)){
            if (_MYCV_DEBUG) printf  ("size equal: %d, depth == %d, channels == %d, all %d\n", sizeEqual(size,cvGetSize(*img)), depth == (*img)->depth, channels == (*img)->nChannels, sizeEqual(size,cvGetSize(*img)) && depth == (*img)->depth && channels == (*img)->nChannels);
            if (_MYCV_DEBUG) printf  ("releasing current image: a %d by %d img, depth = %d, channels = %d\n", cvGetSize(*img).width, cvGetSize(*img).height, (*img)->depth, (*img)->nChannels);
            cvReleaseImage(img);
            if (*img != NULL) {
                printf ("cvReleaseImage failed!\n");
            }
            *img == NULL; //should be covered by releaseImage, but safe this way
            allocateImage (img,size,depth,channels);
        }
    }        
}

void allocateImage (IplImage **img, const IplImage *src) {
    assert (src != NULL);
    allocateImage (img, cvGetSize(src), src->depth, src->nChannels);
}
void cloneImage (const IplImage *src, IplImage **dst) {
    allocateImage (dst, src);
    cvCopy(src, *dst, NULL);
}

/*void imCopy (const IplImage *src, IplImage **dst)
 *
 *(re)allocates memory for dst, then copies src into dst
 */

void imCopy (const IplImage *src, IplImage **dst) {
    if (src == NULL) {
        return;
    }
    allocateImage(dst, cvGetSize(src), src->depth, src->nChannels);
    cvCopy(src, *dst, NULL);
}

void imInfo (IplImage *src, char *msg) {
    if (src != NULL) {
        if (_MYCV_DEBUG) printf  ("%s: src pointer = %d, size = %d by %d, depth = %d, nchannels = %d\n", msg, (int) src, src->width, src->height, src->depth, src->nChannels);
    } else
        if (_MYCV_DEBUG) printf ("%s: src pointer is NULL\n", msg);
}

std::string imStrInfo(IplImage *im) {
    if (im == NULL) {
        return std::string("pointer is NULLL");
    } else {
        std::stringstream s;
        s << "imptr = " << ((unsigned int) im) << " size = " << im->width << " by " << im->height << ", depth = " << im->depth << ", nchannels = " << im->nChannels;
        return s.str();
    }
}

long int getNumFrames (CvCapture *video) {
    long int nframes, current_frame;

    current_frame = (long int) cvGetCaptureProperty (video, CV_CAP_PROP_POS_FRAMES);
    cvSetCaptureProperty  (video, CV_CAP_PROP_POS_AVI_RATIO, 1);
    nframes = (long int) cvGetCaptureProperty (video, CV_CAP_PROP_POS_FRAMES);
    cvSetCaptureProperty (video, CV_CAP_PROP_POS_FRAMES, current_frame);
    return nframes;
}

int loadFrame (MovieT *mov, long int frame_num, IplImage **im) {
    int advance = 0;
    IplImage *frame;
    
    if (frame_num < 0) {
        frame_num = (long int) cvGetCaptureProperty (mov->video, CV_CAP_PROP_POS_FRAMES);
        advance = 1;
    } else {
        if (frame_num < mov->num_frames) {
         cvSetCaptureProperty (mov->video, CV_CAP_PROP_POS_FRAMES, frame_num);
        } else {
            fprintf (stderr, "loadFrame called with bad frame value\n");
            return BAD_FRAME_VALUE;
        }
        advance = 0;
    }

    frame = cvQueryFrame (mov->video);
    if (frame == NULL) {
        fprintf (stderr, "Error: loading frame %d of %d\n", frame_num, mov->num_frames);
        return FRAME_LOAD_ERROR;
    }
    if (_MYCV_DEBUG) printf  ("frame loaded\n");
    allocateImage (im, mov->frame_size, IPL_DEPTH_8U, 1);
    if (_MYCV_DEBUG) printf  ("mem allocated\n");
    cvConvertImage (frame, *im, CV_CVTIMG_FLIP);
    if (advance && frame_num < mov->num_frames - 1) {
        cvSetCaptureProperty (mov->video, CV_CAP_PROP_POS_FRAMES, frame_num + 1);
    }
    return 0;
}

int loadMovie (MovieT **mov, const char *fname) {
    *mov = (MovieT *) cvAlloc (sizeof(mov));
    if (*mov == NULL) {
        fprintf (stderr, "Out of memory\n");
        return -1;
    }

    (*mov)->video = cvCaptureFromFile (fname);
    if ((*mov)->video == NULL) {
        fprintf (stderr, "Error: Can't open video %s\n", fname);
        return -1;
    }

    cvQueryFrame ((*mov)->video);

    /*find out the size of the frame and number of frames in the movie*/
    (*mov)->frame_size.height = (int) cvGetCaptureProperty ((*mov)->video, CV_CAP_PROP_FRAME_HEIGHT);
    (*mov)->frame_size.width = (int) cvGetCaptureProperty ((*mov)->video, CV_CAP_PROP_FRAME_WIDTH);
    (*mov)->num_frames = getNumFrames ((*mov)->video);
}

void releaseMovie (MovieT **mov) {
    cvReleaseCapture (&((*mov)->video));
    cvFree (mov);
}

/*does a binary threshold on src and stores result in dst
 *new memory is allocated for dst if dst == NULL or is the wrong size or type (old memory is released)
 **/
void threshold (const IplImage *src, IplImage **dst, double thresh) {
    CvSize sz;
    sz = cvGetSize(src);
    allocateImage (dst, sz, IPL_DEPTH_8U,1);
    cvThreshold(src, *dst, thresh, 1, CV_THRESH_BINARY);
}
/*
 *      mode:
 *  * CV_RETR_EXTERNAL - retrive only the extreme outer contours
    * CV_RETR_LIST - retrieve all the contours and puts them in the list
    * CV_RETR_CCOMP - retrieve all the contours and organizes them into two-level hierarchy: top level are external boundaries of the components, second level are bounda boundaries of the holes
    * CV_RETR_TREE - retrieve all the contours and reconstructs the full hierarchy of nested contours
 **/
CvSeq *findContours (const IplImage *src, int mode, CvMemStorage *storage) {
    CvSeq *contours = 0;
    IplImage *tmp = cvCloneImage (src);
   
    cvFindContours (tmp, storage, &contours, sizeof(CvContour), mode, CV_CHAIN_APPROX_NONE, cvPoint(0,0));
    cvReleaseImage (&tmp);
    return contours;
}
CvPoint *contourToPoints (CvSeq *contour) {
    CvPoint *pt;
    assert (contour != NULL);
    if (contour->total <= 0) {
        return NULL;
    }
    pt = (CvPoint *) cvAlloc (contour->total*sizeof(CvPoint));
    //printf ("pt = %d\n", (int) pt);
    pt = (CvPoint *) cvCvtSeqToArray (contour, pt, CV_WHOLE_SEQ);
    //printf ("pt = %d\n", (int) pt);
    return pt;
    
}
void blurIm (const IplImage *src, IplImage *dst, double sigma) {
    assert (src != NULL);
    assert (dst != NULL);
    if (_MYCV_DEBUG) printf  ("blur: sigma = %g\n", sigma);
    cvSmooth(src,dst,CV_GAUSSIAN,0,0,sigma,0);
}

int dotProduct (CvPoint v1, CvPoint v2) {
    return (v1.x*v2.x + v1.y*v2.y );
}
double magnitude (CvPoint v) {
    return sqrt (dotProduct (v,v));
}

double angleBetween (CvPoint v1, CvPoint v2) {
    return acos(dotProduct(v1,v2)/(magnitude (v1)*magnitude(v2)));
}
//returns x1 - x2;
CvPoint pointDiff (CvPoint x1, CvPoint x2) {
    CvPoint x3;
    x3.x = x1.x - x2.x;
    x3.y = x1.y - x2.y;
    return x3;
}
CvPoint pointSum (CvPoint x1, CvPoint x2) {
    CvPoint x3;
    x3.x = x1.x + x2.x;
    x3.y = x1.y + x2.y;
    return x3;
}
int convInd (int ind, int j, int anchor, int npts) {
    return ((ind + j - anchor + npts) % npts);
}

void convolveCircular (double *arr, int arr_npts, double *kernel, int kernel_npts, int anchor, double *out) {
    int j, k;
    if (anchor < 0) {
        anchor = kernel_npts / 2;
    }
    for (k = 0; k < arr_npts; k++) {
        out[k] = 0;
        for (j = 0; j < kernel_npts; j++){
            out[k] += kernel[k]*arr[convInd(k,j,anchor,arr_npts)];
        }
    }
}

void gaussK (double *kernel, double sigma) {
    int npts, anchor, j,x;
    double sum = 0;
    npts = 2*((int) (3*sigma) + 1) + 1;
    anchor = npts/2;
    sigma = 2*sigma;
    for (j = 0; j < npts; j++){
        x = (j - anchor);
        kernel[j] = exp(-x*x/(sigma));
        sum += kernel[j];
    }
    for (j = 0; j < npts; j++){
        kernel[j] /= sum;
    }
}

void dgaussK (double *kernel, double sigma) {
    int npts;
    double df[2] = {-1,1};
    double gauss[500];
    gaussK (gauss,sigma);
    npts = 2*((int) (3*sigma) + 1) + 1;
    convolveCircular(gauss, npts, df, 2, 0,kernel);
    kernel[npts-1] = -kernel[0];
}

void deriv (double *arr, int arr_npts, double sigma, double *out) {
    double dg[500];
    int npts = 2*((int) (3*sigma) + 1) + 1;
    dgaussK (dg, sigma);
    convolveCircular (arr, arr_npts, dg, npts, -1, out);
}

/*loads the pixels specified by r from the file specified by fname
 *loads as a grayscale image
 *allocates memory in dst if not already there
 */
void loadSubImage (const char *fname, IplImage **dst, CvRect r) {
    IplImage *im;
    im = cvLoadImage(fname, CV_LOAD_IMAGE_GRAYSCALE);

    /*
    allocateImage(dst, cvSize(r.width, r.height), IPL_DEPTH_8U, 1);
    cvSetImageROI(im, r);
    cvCopy(im, *dst, NULL);
     */

    subImage (im, dst, r);
    cvReleaseImage(&im);
}

/*creates a sub image of src defined by the rectangle r, and places it in depth
 *if the rectangle is outside the image boundaries, it is brought inside, without
 *changing the width or height
 *
 */
void subImage (IplImage *src, IplImage **dst, CvRect r) {
    CvRect roi;
    roi = cvGetImageROI(src);
    allocateImage(dst, cvSize(r.width, r.height), src->depth, src->nChannels);
    r.x = r.x < 0 ? 0 : r.x;
    r.y = r.y < 0 ? 0 : r.y;
    r.x = (r.x + r.width) > src->width ? src->width - r.width  : r.x;
    r.y = (r.y + r.height) > src->height ? src->height - r.height : r.y;

    cvSetImageROI(src, r);

  //  cvSetImageROI(src, r);
    cvCopy(src, *dst, NULL);
    cvSetImageROI(src, roi);

}

int thresholdToTargetArea (IplImage *src, IplImage *dst, int targetArea, int tlow, int thigh, int tstart) {
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
        if (area == targetArea)
            return t;
        if (area < targetArea) {
            if (t <= tlow + 1)
                return t;
            thigh = t;
        } else {
            if (t >= (thigh - 1))
                return t;
            tlow = t;
        }
        t = (thigh + tlow) / 2;
    }
}
/*int thresholdToNumRegions (IplImage *src, IplImage *dst, int nregions, int minArea, int maxArea, int bestArea)
 *
 * attempts to threshold the image in src into nregions regions with size between minArea and maxArea
 * and no regions with size > maxArea
 *
 * if it is able to do that, it further tries to make all regions with area > minArea have an average area bestArea
 *
 * returns the threshold value
 */
/*
int thresholdToNumRegions (IplImage *src, IplImage *dst, int nregions, int minArea, int maxArea, int bestArea) {

    CvSeq *contours, *c;
    CvRect oldroi;
    CvMemStorage *storage;
    char name[512];
    int n, t, area, ntoobig, ntoosmall;
    int thigh, tlow;
    int totalarea;
    int showDebug = 1;
    IplImage *show = NULL;
    oldroi = cvGetImageROI(src);

    
    thigh = 255;
    tlow = 1;
    t = 128;

    storage = cvCreateMemStorage(0);

    if (showDebug) {
        sprintf(name, "thresholding to %d regions, bestArea = %d", nregions, bestArea);
        cvNamedWindow(name, 0);
        allocateImage(&show, cvGetSize(src), IPL_DEPTH_8U, 3);
    }

    while (1) {
        cvThreshold(src, dst, t, 1, CV_THRESH_TOZERO);
        cvFindContours(dst, storage, &contours, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, cvPoint(0,0));
        c = contours;
        n = 0;
        ntoobig = 0;
        ntoosmall = 0;
        totalarea = 0;
        if (showDebug) {
            cvConvertImage(src, show, 0);
        }
        while (c != NULL) {
            area = (int) (contourArea (c) + 0.5);
            if (area > maxArea) {
                if (showDebug) {
                    cvDrawContours(show, c, CV_RGB(255,0,0), CV_RGB(255,0,0), 0, 1, 8, cvPoint(0,0));
                }
                ntoobig++;
            } else if (area < minArea) {
                if (showDebug) {
                    cvDrawContours(show, c, CV_RGB(0,0,255), CV_RGB(255,0,0), 0, 1, 8, cvPoint(0,0));
                }
                ntoosmall++;
            } else {
                if (showDebug) {
                    cvDrawContours(show, c, CV_RGB(0,255,0), CV_RGB(255,0,0), 0, 1, 8, cvPoint(0,0));
                }
                totalarea += area;
                n++;
            }
            c = c->h_next;
        }
        if (showDebug) {
            cvShowImage(name, show);
            cvWaitKey(-1);
        }
        if (ntoobig == 0 && n == nregions) {
            if (totalarea > n * bestArea) {
                //regions are on average above optimum, so we increase the threshold
                if (t >= thigh - 1) {
                     break;
                }
                tlow = t;
            } else {
                //decrease threshold to make regions larger
                if (t <= tlow + 1) {
                    break;
                }
                thigh = t;
            }
           
        }else if (ntoobig > 0) {
            if (t >= thigh - 1) {
                break;
            }
            //increase threshold to decrease size of largest region / cause it to split
            tlow = t;
        } else {
            //decrease threshold to increase number of potential regions
            if (t <= tlow + 1) {
                break;
            }
            thigh = t;
        }
        t = (tlow + thigh)/2;
        cvClearMemStorage(storage);
    }
    if (showDebug) {
        cvReleaseImage(&show);
        cvDestroyWindow(name);
    }
    cvReleaseMemStorage(&storage);
    cvThreshold(src, dst, t, 1, CV_THRESH_TOZERO);
    return t;
}
*/

static double findGoodness(CvSeq *contours, int nregions, int minArea, int maxArea, int bestArea) {
    double goodness = 0;
    //-100 for every contour# you are away from nregions
    //-10 for every contour below minArea or above maxArea
    //-1 * (area - bestArea)^2 / (maxArea - minArea)^2 for each contour
    int nc = 0;
    int area;
    CvSeq *c = contours;
    while (c != NULL) {
        ++nc;
        area = (int) fabs(cvContourArea(c, CV_WHOLE_SEQ));
        if (area < minArea || area > maxArea) {
            goodness -= 10;
        }
        goodness -= 1.0*(area - bestArea)*(area - bestArea) / ((maxArea-minArea)*(maxArea - minArea));
        c = c->h_next;
    }
    goodness -= fabs(nc - nregions) * 100;
    return goodness;

}

int thresholdToNumRegions(IplImage* src, IplImage* dst, int nregions, int minArea, int maxArea, int bestArea) {
    //first find valid range, which is thresholded to between nregions*minArea and nregions*maxArea
    int mintotal = nregions*minArea;
    int maxtotal = nregions*maxArea;
    maxtotal = maxtotal >= (src->width) * (src->height) ? (src->width)*(src->height) - 1 : maxtotal;

    int bestThreshold = -1;
    double goodness, bestgoodness = -nregions*200;
    CvMemStorage *storage = cvCreateMemStorage(0);
    CvSeq *contours;

    for (int j = 1; j < 255; ++j) {
        cvThreshold(src, dst, j, 1, CV_THRESH_TOZERO);
        if (cvCountNonZero(dst) < mintotal) {
       //     std::cout << "< mintotal " << j;
            break; //we have too few nonzero points b/c threshold is too high, so stop incrementing
        }
        if (cvCountNonZero(dst) > maxtotal) {
       //     std::cout << "> maxtotal " << j;
            continue; //too many nonzero pixels, so no need to analyze further
        }
        cvFindContours(dst, storage, &contours, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, cvPoint(0,0) );
        goodness = findGoodness(contours, nregions, minArea, maxArea, bestArea);
        if (goodness > bestgoodness) {
            bestgoodness = goodness;
            bestThreshold = j;
         //   std::cout << goodness << " ";
         //   std::cout << j << "\t";
        }
    }
    cvReleaseMemStorage(&storage);
    cvThreshold(src, dst, bestThreshold, 1, CV_THRESH_TOZERO);
    return bestThreshold;

}


double contourArea (CvSeq *contour) {
    assert (contour != NULL);
    CvMoments moments;
    cvMoments(contour, &moments, 0);
    return cvGetSpatialMoment(&moments, 0, 0);
}

/* CvRect rectIntersect (CvRect r1, CvRect r2)
 *
 * returns the intersection of two rectangles
 * if the rectangles do not intersect, one of length or width will be < 0
 *
 */
CvRect rectIntersect (CvRect r1, CvRect r2) {
    int x1, y1, x2, y2;

    x1 = (r1.x > r2.x) ? r1.x : r2.x;
    x2 = (r1.x + r1.width < r2.x + r2.width) ? r1.x + r1.width : r2.x + r2.width;
    y1 = (r1.y > r2.y) ? r1.y : r2.y;
    y2 = (r1.y + r1.height < r2.y + r2.height) ? r1.y + r1.height : r2.y + r2.height;

    return cvRect(x1, y1, x2-x1, y2-y1);
}
/*double pointDistance (CvPoint p1, CvPoint p2)
 *
 *distance between p1 and p2
 */
double pointDistance (CvPoint p1, CvPoint p2) {
    return magnitude(pointDiff(p1, p2));
}
CvPoint2D32f pointDiff(CvPoint2D32f p1, CvPoint2D32f p2) {
    return cvPoint2D32f(p1.x - p2.x, p1.y - p2.y);
}
double pointDistance (CvPoint2D32f p1, CvPoint2D32f p2) {
    CvPoint2D32f p = pointDiff(p1, p2);
    return sqrt (p.x*p.x + p.y*p.y);
}
void offsetVectorByPoint (std::vector<CvPoint2D32f> *v, CvPoint2D32f pt) {
    std::vector<CvPoint2D32f>::iterator it;
    for (it = v->begin(); it != v->end(); ++it){
        it->x += pt.x;
        it->y += pt.y;
    }
}

double sumDistance (const std::vector <CvPoint2D32f> &v1, const std::vector <CvPoint2D32f> &v2) {
    if (v1.size() != v2.size()) {
        return -1;
    }
    double sd = 0;
    for (int j = 0; j < v1.size(); ++j) {
        sd += pointDistance(v1.at(j), v2.at(j));
    }
    return sd;
}

/*IplImage *LVtoIpl (char* lvsrc, int lvlinewidth, int LVWidth, int LVHeight)
 *
 *wraps a labview image into an IPL image
 */
IplImage *LVtoIpl (char* lvsrc, int lvlinewidth, int LVWidth, int LVHeight) {

	IplImage *cvim;
        cvim = cvCreateImageHeader(cvSize(LVWidth,LVHeight), IPL_DEPTH_8U, 1);
        cvim->imageData = lvsrc;
        cvim->widthStep = lvlinewidth;

        return cvim;
}

/*void copyAllocatedImage (IplImage *src, IplImage *dst)
 *
 *copies src to dst making sure the rois match
 *if source roi width > dst roi width (etc.) copies a subset of the src
 *if dst roi wider, copies the entire source into a subset of the dst
 *
 *in all cases, upper left corner of roi1 copies to upper left corner of roi2
 */

void copyAllocatedImage (IplImage *src, IplImage *dst) {
    CvRect sroi, droi, sroi2, droi2;

    sroi = cvGetImageROI(src);
    droi = cvGetImageROI(dst);

    sroi2 = sroi;
    droi2 = droi;

    sroi2.width = sroi.width < droi.width ? sroi.width : droi.width;
    droi2.width = sroi2.width;
    sroi2.height = sroi.height < droi.height ? sroi.height : droi.height;
    droi2.height = sroi2.height;

    cvSetImageROI(src, sroi2);
    cvSetImageROI(dst, droi2);

    cvCopy(src, dst, NULL);

    cvSetImageROI(src, sroi);
    cvSetImageROI(dst, droi);
}




CvPoint2D32f centerOfMass (CvSeq *contour) {
    CvPoint2D32f com;
    CvPoint pt;
    int j;
    double m00;
    CvMoments moments;
    assert (contour != NULL);
    if (contour->total == 1) {
        pt = *(CV_GET_SEQ_ELEM(CvPoint, contour, 0));
        return cvPoint2D32f(pt.x, pt.y);
    }
    cvMoments(contour, &moments, 0);
    m00 = cvGetSpatialMoment(&moments, 0, 0);
    com.x = cvGetSpatialMoment(&moments, 1, 0)/m00;
    com.y = cvGetSpatialMoment(&moments, 0, 1)/m00;

    if (isnan(com.x) || isnan(com.y)) {
        //myprintf ("COM has NAN; compensating\n");
        com.x = com.y = 0;
        for (j = 0; j < contour->total; j++) {
            pt = *(CV_GET_SEQ_ELEM(CvPoint, contour, j));
            com.x += pt.x;
            com.y += pt.y;
        }
        com.x /= j;
        com.y /= j;
    }


    return com;
}
CvPoint2D32f centerOfMass (CvSeq *contour, const IplImage *src, double *covMat) {
    //int i, j;
    double xsum, ysum, sum;// val;
    CvRect bound;
    CvMat submat;
    CvMoments moments;

    if (contour->total == 1) {
        CvPoint pt = *(CvPoint *) cvGetSeqElem(contour, 0);
        return cvPoint2D32f (pt.x, pt.y);
    }

    bound = cvBoundingRect(contour, 0);

    //cvGetSubRect does not allocate new memory -- it just gives you a header 
    //that allows you to treat submat as its own image, even though the data
    //remains stored in src
    cvGetSubRect(src, &submat, bound);
    cvMoments(&submat, &moments, 0);
    xsum = cvGetSpatialMoment(&moments, 1, 0);
    ysum = cvGetSpatialMoment(&moments, 0, 1);
    sum = cvGetSpatialMoment(&moments, 0, 0);
    if (covMat != NULL) {
        covMat[0] = cvGetCentralMoment(&moments, 2, 0) / sum;
        covMat[1] = covMat[2] = cvGetCentralMoment (&moments, 1, 1) / sum;
        covMat[3] = cvGetCentralMoment (&moments, 0, 2) / sum;
    }
   
    return cvPoint2D32f (bound.x + xsum/sum, bound.y + ysum/sum);
}
/*returns a copy of src
 *copies only the area within the bounding box of contour, subject to
 *all pixels within the contour are unchanged
 *all pixels outside the contour are 0
 */
void onlyInContour (IplImage *src, IplImage **dst,CvSeq *contour, CvPoint offset) {
    CvRect bound, roi;

    assert (src != NULL);
    assert (contour != NULL);
    allocateImage(dst, src);
    assert (dst != NULL);
    bound = cvBoundingRect(contour, 0);
    roi = cvGetImageROI(src);
    cvSetImageROI(src, bound);
    cvSetImageROI(*dst, bound);
    cvSetZero(*dst);
    /*
    cvDrawContours(*dst, contour, cvScalarAll(255), cvScalarAll(255), 0, CV_FILLED, 8, offset);
    cvNamedWindow("test",0);
    cvShowImage("test", *dst);
    */
    cvDrawContours(*dst, contour, cvScalarAll(1), cvScalarAll(1), 0, 1, 8, offset);
    cvDrawContours(*dst, contour, cvScalarAll(1), cvScalarAll(1), 0, CV_FILLED, 8, offset);
    cvMul(src, *dst, *dst);
    cvSetImageROI(src,roi);
    cvSetImageROI(*dst,roi);
}
CvSeq *selectContainingContour (CvSeq *firstContour, CvPoint pt) {
    return selectContainingContour(firstContour, cvPoint2D32f (pt.x, pt.y));
}
CvSeq *selectContainingContour (CvSeq *firstContour, CvPoint2D32f pt) {
    while (firstContour != NULL) {
        /*
        if (cvPointPolygonTest(firstContour, pt, 0) >= 0) {
            return firstContour;
        }
         */
        if (inRectangle(cvBoundingRect(firstContour, 0), pt)) {
            return firstContour;
        }
        firstContour = firstContour->h_next;
    }
    return NULL;
}

bool inRectangle (CvRect r, CvPoint pt) {
     return ((pt.x >= r.x) && (pt.y >= r.y) && (pt.x <= r.x + r.width) && (pt.y <= r.y + r.height));
}

bool inRectangle (CvRect r, CvPoint2D32f pt) {
    return ((pt.x >= r.x) && (pt.y >= r.y) && (pt.x <= r.x + r.width) && (pt.y <= r.y + r.height));
}


CvSeq *nearestContour (CvSeq *firstContour, CvPoint2D32f pt) {
    CvSeq *closest = firstContour;
    if (firstContour == NULL) {
        return NULL;
    }
    double dist, mindist = pointDistance(pt, centerOfMass(firstContour));
    firstContour = firstContour->h_next;
    while (firstContour != NULL) {
        if ((dist = pointDistance(pt, centerOfMass(firstContour))) < mindist) {
            mindist = dist;
            closest = firstContour;
        }
        firstContour = firstContour->h_next;
    }
    return closest;
}
CvSeq *nearestContour (CvSeq *firstContour, CvPoint pt) {
    return nearestContour(firstContour, cvPoint2D32f(pt.x, pt.y));
}

CvSeq *findContainingContour (IplImage *src, CvMemStorage *ms, CvPoint2D32f x0) {
    CvSeq  *start;
    cvFindContours(src, ms, &start, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, cvPoint(0,0));
    return selectContainingContour(start, x0);
}
CvSeq *findContainingContour (IplImage *src, CvMemStorage *ms, CvPoint x0) {
    return findContainingContour(src, ms,  cvPoint2D32f(x0.x, x0.y));
}
CvSeq *findCorrespondingContour (IplImage *src, CvMemStorage *ms, CvPoint2D32f x0) {
    CvSeq *c, *start;
    cvFindContours(src, ms, &start, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, cvPoint(0,0));
    c = selectContainingContour(start, x0);
    if (c == NULL) {
        c = nearestContour(start, x0);
    }
    return c;
}
CvSeq *findCorrespondingContour (IplImage *src, CvMemStorage *ms, CvPoint x0) {
    return findCorrespondingContour(src, ms, cvPoint2D32f(x0.x, x0.y));
}

/* std::vector<double> (y) convolveVector (vector<double> x, vector<double> kernel, int offset)
 *
 * y(j) = sum_k x(j - k + offset) * kernel(k)
 * if offset is not defined, offset is set to 1/2 kernel length
 * if x(j+k-offset) is out of range, we use the first/last in range value in x
 */

std::vector<double> convolveVector (const std::vector<double> &x, const std::vector<double> &kernel, int offset) {
    std::vector<double> y;
    double element; int index;
    int j,k;
   // std::cout << "x size = " << x.size();
   // std::cout << "kernel size = " << kernel.size();
   // std::cout << "\n";
    y.resize(x.size(), 0);
    for (j = 0; j < x.size(); ++j) {
        element = 0;
        for (k = 0; k < kernel.size(); ++k) {
            index = j - k + offset;
            index = index < 0 ? 0 : index;
            index = index >= x.size() ? x.size() - 1 : index;
            element += ((double) x[index]) * kernel[k];
        }
     //   std::cout << j << "," << element << "\t";
        y[j] = element;
    }
    return y;
}
std::vector<double> convolveVector (const std::vector<double> &x, const std::vector<double> &kernel) {
    return (convolveVector(x, kernel, kernel.size()/2));
}
std::vector<CvPoint2D32f> convolveVector (const std::vector<CvPoint2D32f> &x, const std::vector<double> &kernel, int offset = -1) {
    std::vector<double> xx;
    std::vector<double> yy;
    std::vector<CvPoint2D32f> y;
    splitPointVector(x, &xx, &yy);
    combinePointVector(convolveVector(xx, kernel, offset), convolveVector(yy, kernel, offset), &y);
    return y;
}
std::vector<CvPoint2D32f> convolveVector (const std::vector<CvPoint2D32f> &x, const std::vector<double> &kernel) {
    return (convolveVector(x, kernel, kernel.size()/2));
}


std::vector<double> gaussKernel (double sigma) {
    std::vector<double> kernel;
    int npts, anchor, j,x;
    double sum = 0;
    npts = 2*((int) (3*sigma) + 1) + 1;
    anchor = npts/2;
    sigma = 2*sigma*sigma;
    for (j = 0; j < npts; j++){
        x = (j - anchor);
        kernel.push_back(exp(-x*x/(sigma)));
        sum += kernel[j];
    }
    for (j = 0; j < npts; j++){
        kernel[j] /= sum;
    }
    return kernel;
}
std::vector<double> dgaussKernel (double sigma) {
    std::vector<double> d;
    d.push_back(1); d.push_back(-1);
    return convolveVector(gaussKernel(sigma), d);
}
std::vector<double> derivative (const std::vector<double> &x, double sigma) {
    return convolveVector(x, dgaussKernel(sigma));
}

void simplePlot (const std::vector<double> &x, IplImage **dst) {
    allocateImage (dst, cvSize(640,480), IPL_DEPTH_8U, 1);
    cvSetZero(*dst);
    double min, max, ymult,xmult;
    min = max = 0; //guarantee x-axis is included in range
    for (int j = 0; j < x.size(); ++j) {
        min = min < x[j] ? min : x[j];
        max = max > x[j] ? max : x[j];
    }
    ymult = 480.0/(max - min);
    xmult = 640.0/x.size();
    for (int j = 0; j < (x.size() - 1); ++j) {
        cvLine(*dst,
                cvPoint((int) (xmult*j),(int) (480 - ymult * (x[j] - min))),
                cvPoint((int) xmult*(j+1), (int) (480 - ymult * (x[j+1] - min))), cvScalarAll(255)
                );
    }
    cvLine(*dst, cvPoint(0, 480 + (int) (ymult*min)), cvPoint(640,480+(int) (ymult*min)),cvScalarAll(255));

}

void combinePointVector (const std::vector<double> &x, const std::vector<double> &y, std::vector<CvPoint2D32f> *ptlist) {
    ptlist->clear();
    int j, ms;
    ms = x.size() < y.size() ? x.size() : y.size();
    for (j = 0; j < ms; ++j) {
        ptlist->push_back(cvPoint2D32f(x[j], y[j]));
    }
}
