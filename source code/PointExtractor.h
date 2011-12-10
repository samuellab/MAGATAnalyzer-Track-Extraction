/* 
 * File:   PointExtractor.h
 * Author: Marc
 *
 * Created on October 21, 2009, 10:47 AM
 *
 * PointExtractor defines a class that extracts animal locations from a dark field image
 *
 * It does this by creating a background that is the minimum of a set of images
 * in a range around the target image, then subtracting off this background
 * and comparing the result to a threshold (either global or local)
 *
 * We then segment the thresholded image into regions and create an ImTrackPoint
 * at the center of mass of each region.
 *
 * if winsize (see below) < 0, we don't copy an image into the ImTrackPoint, but
 * the type of the point is still ImTrackPoint and not just TrackPoint
 *
 * (C) Marc Gershow; licensed under the Creative Commons Attribution Share Alike 3.0 United States License.
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/us/ or send a letter to
 * Creative Commons, 171 Second Street, Suite 300, San Francisco, California, 94105, USA.
 */
#include <vector>
#include "cv.h"
#include "ctype.h"
#include "ImTrackPoint.h"
#include "FrameLoader.h"
#include "communicator.h"
using namespace std;
#ifndef _POINTEXTRACTOR_H
#define	_POINTEXTRACTOR_H

typedef struct {
    int nBackgroundFrames;
    int resampleInterval;
    double blurSigma;
} BackgroundParamsT;

class PointExtractor {
    static const bool messagesOn = true;
public:
    PointExtractor();
    virtual ~PointExtractor();

    /*******************************************************************
     *                  EXTRACTION PARAMETER SETTINGS                  *
     *******************************************************************/

    /* setThresholdCompareIm (const IplImage *src);
     * copies src into internal storage
     * src defines a threshold image s.t. if a pixel in the background
     * subtracted image is greater than the value of the equivalent pixel
     * in src, it is counted as above threshold and possibly part of an animal
     */
    virtual void setThresholdCompareIm (const IplImage *src);

    /* void setGlobalThreshold (int thresh)
     *
     * sets a global threshold, s.t. if a pixel in the background subtracted
     * region is greater than thresh, it is counted as above threshold and
     * possibly part of an animal
     */
    virtual void setGlobalThreshold (int thresh);

    /*setFrameRange (int firstFrame, int lastFrame)
     *we only extract points from frames between firstFrame and lastFrame
     *extraction starts at frame firstFrame and advances one frame every
     *time nextFrame is called.
     *
     *when creating a background image, PE will only request frames in this range
     */
    inline void setFrameRange (int firstFrame, int lastFrame) {
        this->firstFrame = firstFrame;
        this->lastFrame = lastFrame;
        frameNum = firstFrame;
        firstRun = true;
    }

    /* void setBackgroundParams ( int nBackgroundFrames , int resampleInterval, int blurSigma) 
     * common values in parens
     * nBackgroundFrames(5) -- how many frames to use to create the background
     * resampleInterval(200) -- background is recreated from a set of images evenly distributed
     *      through resampleInterval every resampleInterval
     *      e.g. nBackgroundFrames = 5 if resample interval = 200 and firstFrame = 0,
     *      the first background image is the minimum of frames 0,50,100,150,and 200
     *      the next background image is the minimum of frames 201 251 301 351 and 401
     *      etc. etc.
     * blurSigma(1) -- if > 0, we denoise the background image by convolving with a 2D gaussian of standard
     *      deviation sigma (in pixels)
     */
    inline void setBackgroundParams ( int nBackgroundFrames , int resampleInterval, double blurSigma) {
        bp.nBackgroundFrames = nBackgroundFrames;
        bp.resampleInterval = resampleInterval;
        bp.blurSigma = blurSigma;
    }
    /* void setFrameLoader (FrameLoader *fl)
     *
     * use this frameLoader to get images from disk
     * if point extractor has a valid analysis rect, we assign that rectangle
     * to frame loader
     * otherwise, we use frame loader's (possibly invalid) analysis rectangle
     *
     * on destruction, point extractor deletes FrameLoader
     */
    void setFrameLoader (FrameLoader *fl);

    inline void clearFrameLoader () {
        if (fl != NULL) {
            delete fl;
        }
        fl = NULL;
    }

    /* void setAreaRange (double minArea, double maxArea)
     *
     * a region must have area between minArea and maxArea to be converted
     * to a track point;  useful for eliminating small blips 
     * maxArea is less useful, because if you've actually created a blob that's
     * way too large to be anything, you've probably got larger problems on your hands
     */
    inline void setAreaRange (double minArea, double maxArea) {
        this->minArea = minArea;
        this->maxArea = maxArea;
    }

    /* void setAnalysisRect (CvRect r)
     *
     * sets the analysis rectangle, which is the area of the frame to be
     * searched for points; this is also the only area loaded by the frame loader
     * (setting analysis rectangle also sets frame loader analysis rectangle)
     */
    void setAnalysisRect (CvRect r);
    inline CvRect getAnalysisRect() {
        return analysisRect;
    }

    /* void setWinSize
     * sets the window size of the image to be excerpted for each ImTrackPoint
     * created.  If winSize < 0, no image is excerpted but an ImTrackPoint is
     * still created (with im = NULL)
     *
     * memory note:  if you have 14,400 frames and expect ~10 points per frame
     * then the total memory required to store just the images (which will be
     * most of the memory used but not all of it) is
     *
     * 230 * 10^6 bytes * (winsize/40)^2
     *
     */
    inline void setWinSize (int winSize) {
        this->winSize = winSize;
    }

    /**************************************************************************
     *                  POINT EXTRACTION FUNCTIONS                            *
     **************************************************************************/

    /*  virtual int nextFrame();
     *
     *  nextFrame causes PE to reset itself (clear all memory of previous extractions)
     *  then load the nextFrame from disk, create a new background if needed,
     *  and then background subtract and threshold the loaded image
     */
    virtual int nextFrame();
    /* virtual vector<TrackPoint *> *getPoints()
     *
     * creates ImTrackPoints representing each region in the thresholded images
     * regions with areas outside [minArea,maxArea] are not included
     *
     * outputs a vector of pointers to trackpoints; both the vector and the
     * points are dynamically allocated and reside on the heap
     */
    virtual vector<TrackPoint *> *getPoints();

    /* virtual vector<TrackPoint *> splitPoint(TrackPoint *pt, int npts, int bestArea);
     *
     * given a point, rethresholds the region containing or nearest that point in
     * an attempt to produce npts new points
     * returns a vector containing the new points (may or may not be npts long)
     * currently, no morphological operations (e.g. erosions) are used to assist
     * future versions of the code may improve this
     *
     * outputs a vector of pointers to trackpoints; both the vector and the
     * points are dynamically allocated and reside on the heap
     */
    virtual vector<TrackPoint *> *splitPoint(TrackPoint *pt, int npts, int bestArea);


    /* accessors: getFrameNumber - current frame being analyzed
     * copy orig image out; gives a copy of the image as loaded by frame loader
     * copy BS imag out; gives a copy of the background subtracted image
     */
    inline int getFrameNumber () {
        return frameNum;
    }
    int copyOrigImageOut (IplImage **dst);
    int copyBSImagOut (IplImage **dst);

    inline void setFrameNormalizationMethod (_frame_normalization_methodT fnm) {
        this->fnm = fnm;
    }
    inline _frame_normalization_methodT getFrameNormalizationMethod () {
        return fnm;
    }

    inline void setMessageHandler (communicator *mh) {
        this->mh = mh;
    }
    /* IplImage *generateForeground(int increment);
     *
     * generates the maximum of the background subtracted images
     * over the range [firstFrame,lastFrame]
     *
     * only every increment frame is used (e.g. increment = 4, then frames 0, 4, 8, etc.)
     *
     * this messes with the state of PE, is generally kludgey and is just here to mess around with
     * destroy PE immediately after using this function, and reinstantiate it if needed
     */
    IplImage *generateForeground(int increment);

    void copyOutAutoGeneratedForeground (IplImage **dst);

    /*virtual bool supportsCollisionSplits ()
     * returns true if this version of pointextractor supports splitting points
     * true for the base class;  subclasses (e.g. wormpointextractor) may disagree
     */
    virtual bool supportsCollisionSplits () {
        return true;
    }

protected:
    bool firstRun;
    IplImage *currentFrame;
    IplImage *backgroundIm;
    IplImage *baksub;
    IplImage *thresholdCompareIm;
    IplImage *threshIm;
    IplImage *foregroundIm;
    
    FrameLoader *fl;

    CvRect analysisRect;
    int firstFrame; //first frame number
    int lastFrame; //last frame number
    int frameNum;
    int increment;
    int bakValidUntil;
    BackgroundParamsT bp;

    double minArea;
    double maxArea;

    int winSize;

    bool globalThresh;
    double thresh;

    _frame_normalization_methodT fnm;
    int normFactor;

    int ncheckedout;

    CvMemStorage *tempStorage; //cleared every frame

   
    void calculateBackground();
    virtual int loadFrame(int frameNum);

    virtual void threshToZero();

    CvSeq *findContours();
    IplImage *checkoutTempIm();
    void checkinTempIm(IplImage **im);

    ImTrackPoint *contourToTrackPoint (CvSeq *c);

    //same as CTTP, but first zeros all points outside the contour, to avoid interference
    //slower, has different result iff there are other above threshold points inside the 
    //contour bounding rectangle
    ImTrackPoint *contourToTrackPointCareful (CvSeq *c);
    communicator *mh;
    inline void message(const char *msg, const verbosityLevelT verb) {
        if (messagesOn && mh != NULL) {
            mh->message(name(), verb, msg);
        }
    }
    virtual inline const char *name() {
        return "Point Extractor";
    }
private:
    vector <IplImage *> tempImages;
     void init();
};

#endif	/* _POINTEXTRACTOR_H */

