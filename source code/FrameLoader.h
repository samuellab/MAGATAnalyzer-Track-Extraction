/* 
 * File:   FrameLoader.h
 * Author: Marc
 *
 * Created on October 22, 2009, 1:43 PM
 *
 * Frame Loader is an abstract class that defines an interface for
 * loading images;
 * Example subclass: ImLoader, loads still images
 * To be implemented: movie loader, live loader ?
 *
 * (C) Marc Gershow; licensed under the Creative Commons Attribution Share Alike 3.0 United States License.
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/us/ or send a letter to
 * Creative Commons, 171 Second Street, Suite 300, San Francisco, California, 94105, USA.
 */

#ifndef _FRAMELOADER_H
#define	_FRAMELOADER_H

#include "cv.h"
#include "cvtypes.h"
#include "communicator.h"

enum _frame_normalization_methodT {_frame_none, _frame_wholeImage, _frame_excerptedRect};

class FrameLoader {
    

public:
    static const bool messagesOn = true;
    FrameLoader();
    //
    virtual ~FrameLoader();


    /* int getFrame (int frameNumber, IplImage **dst)
     * this function, which must be implemented in each subclass
     * gets a particular frame, allocates memory in *dst if needed,
     * and copies the portion of frame in the analysis rectangle
     * into dst
     *
     * returns nonzero on an error
     */
    virtual int getFrame(int frameNumber, IplImage **dst);

     /* int getFrame (int frameNumber, IplImage **dst)
     * this function, which must be implemented in each subclass
     * gets a particular frame, allocates memory in *dst if needed,
     * and copies the portion of frame in the analysis rectangle
     * into dst
     *
     * returns nonzero on an error
     */
    virtual int getFrame(int frameNumber, IplImage **dst, _frame_normalization_methodT fnm, double normTarget);

    virtual int getFrameNormFactor (int frameNumber,  _frame_normalization_methodT fnm);

    /* virtual void setFileName (const char *fileName, const char *extension)
     *
     * sets the filename and extension used to load images
     */
    virtual void setFileName (const char *fileName, const char *extension);

    /* set, get analysis rect
     * analysis rect is a CvRect defining the area of the whole image
     * to be loaded
     *
     * default (if ar not set) is to load the whole image
     */
    virtual void setAnalysisRect (CvRect r);
    virtual inline CvRect getAnalysisRect () {
        return ar;
    }
    inline void setMessageHandler (communicator *mh) {
        this->mh = mh;
    }

protected:
    char *fileName;
    char *extension;
    IplImage *loadIm;
    IplImage *convertedIm;
    CvRect ar;
    communicator *mh;
    virtual inline void message(const char *msg, const verbosityLevelT verb) {
        if (messagesOn && mh != NULL) {
            mh->message("Frame Loader", verb, msg);
        }
    }
    int lastFrameLoaded;
    /* loadWholeFrame
     * should load an 8-bit grayscale image comprising all the image (not just the analysis rectangle)
     * to loadIm
     *
     * then should set lastFrameLoaded to frameNumber.  Should reload image regardless
     * of whether lastFrameLoaded == frameNumber.
     *
     * return 0 for success, < 0 for failure
     */
    virtual int loadWholeFrame (int frameNumber) = 0;
    void checkAr();
private:
    FrameLoader(const FrameLoader& orig);
    void Init();

};

#endif	/* _FRAMELOADER_H */

