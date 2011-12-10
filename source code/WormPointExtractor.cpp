/* 
 * File:   WormPointExtractor.cpp
 * Author: Marc
 * 
 * Created on November 16, 2009, 4:18 PM
 *
 * (C) Marc Gershow; licensed under the Creative Commons Attribution Share Alike 3.0 United States License.
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/us/ or send a letter to
 * Creative Commons, 171 Second Street, Suite 300, San Francisco, California, 94105, USA.
 */

#include <vector>
#include <algorithm>
#include "WormPointExtractor.h"
#include "CvUtilsPlusPlus.h"
#include "TrackPoint.h"
#include "tictoc.h"
using namespace std;

WormPointExtractor::WormPointExtractor() {
    init();

}
void WormPointExtractor::init() {

    temporalSum = NULL;
    temporalThresh = NULL;
    temp32 = NULL;
    baksub32 = curframe32 = bak32 = threshIm32 = NULL;
    stacklength = 10;
    hysteresisFactor = 1.5;
    dilationDist = 3;
    lastPtsOut.clear();
}

WormPointExtractor::WormPointExtractor(const WormPointExtractor& orig) {
}

/*
int WormPointExtractor::loadFrame(int frameNum) {
    
}
*/

void WormPointExtractor::addNewImageToStack(IplImage *newImage) {
    _TICTOC_TIC_FUNC
    message("add image to stack", verb_debug);
    IplImage *im = imstack.back(); //save the address of the last (least recently loaded) image
    imstack.pop_back();  //take that image off the stack
    if (dilationDist <= 0) {
        cloneImage(newImage, &im); //copy the new image into the old one's space
    } else {
        allocateImage(&im, newImage);
        cvDilate(newImage, im, NULL, dilationDist);
    }
    //we truncate the image so that each frame can contribute at most 3*the threshold
    //towards the total (if total length of the stack is less than 6, each im can only contribute
    //stacklength/2*thresh towards total
    
    cvThreshold(im, im, stacklength > 6 ? thresh*3 : thresh*stacklength/2, 0, CV_THRESH_TRUNC);
    imstack.insert(imstack.begin(), im); //push the new image into the front of the stack
    _TICTOC_TOC_FUNC
}


void WormPointExtractor::addFrameToStack() {
    _TICTOC_TIC_FUNC
    assert (currentFrame != NULL);
    message ("addFrameToStack", verb_debug);
    //convert current frame and background frame to 32 bit images, so we can subtract
    //them and have negative values
    allocateImage(&curframe32, cvGetSize(currentFrame), IPL_DEPTH_32F, 1);
    cvConvert (currentFrame, curframe32);
    allocateImage(&bak32, cvGetSize(currentFrame), IPL_DEPTH_32F, 1);
    cvConvert (backgroundIm, bak32);
    allocateImage(&baksub32, curframe32);
    cvSub(curframe32, bak32, baksub32, NULL);

    //if we have fewer than required number of images, pad stack with extra
    //copies of this one (usually because this is the first frame)
    IplImage *im = NULL;
    while (imstack.size() < stacklength) {
        message ("building im stack up", verb_debug);
        im = NULL;
        cloneImage(baksub32, &im); //allocate new memory
        imstack.insert(imstack.begin(), im); //put the image in the front (last to be deleted) of the stack
    }
    //if we have more than the required number of images, remove extras
    //from back (oldest in time)
    while (imstack.size() > stacklength) {
        message ("tearing stack down", verb_debug);
        im = imstack.back();
        imstack.pop_back();
        cvReleaseImage(&im);
    }

    addNewImageToStack(baksub32);
    _TICTOC_TOC_FUNC
}
/*void WormPointExtractor::threshToZero()
 *
 * take the sum of the dilated image stack (we dilate in case the animal has moved significantly)
 * dilationDist should be roughly half the maximum expected distance traveled in stacklength frames
 * then compare that sum to the threshold (or threshold im) * the number of frames
 * (NB the threshold probably can be set lower than in the single image case (this
 * is the point of doing the temporal average)
 *
 * return the CURRENT background subtracted frame * (sum > thresh ? 1 : 0)
 *
 * blur parameter is ignored
 */

void WormPointExtractor::threshToZero() {
    _TICTOC_TIC_FUNC
    message ("WPE thresh to zero", verb_debug);
    addFrameToStack();

    assert (baksub32 != NULL);
    TICTOC::timer().tic("tt0:allocating images");
    allocateImage(&temporalSum, baksub32);
    allocateImage(&temp32, temporalSum);
    allocateImage(&temporalThresh, baksub);

    TICTOC::timer().toc("tt0:allocating images");

    cvSetZero(temporalSum);
    TICTOC::timer().tic("tt0:creating temporal sum");
    for (vector<IplImage *>::iterator it = imstack.begin(); it != imstack.end(); ++it) {
        cvAdd(*it, temporalSum, temporalSum, NULL);
    }
    TICTOC::timer().toc("tt0:creating temporal sum");

    message ("wpethreshto0", verb_debug);


    TICTOC::timer().tic("tt0:thresholding and comparing");
    if (threshIm32 == NULL) {
        allocateImage(&threshIm32, temporalSum);
        cvConvertScale(thresholdCompareIm, threshIm32, imstack.size(), 0);
    }

    if (globalThresh) {
        cvCmpS(temporalSum, imstack.size()*thresh, temporalThresh, CV_CMP_GE);
    } else {
        std::stringstream s;
        s << "thresholdCompareIm: " << imStrInfo(thresholdCompareIm);
        message(s.str().c_str(), verb_debug);
        s.str("");
        s << "threshIm32: " << imStrInfo(threshIm32);
        message(s.str().c_str(), verb_debug);
        
        cvCmp(temporalSum, threshIm32, temporalThresh, CV_CMP_GE);
    }
    cvSetZero(threshIm);
    cvCopy(baksub, threshIm, temporalThresh);
    TICTOC::timer().toc("tt0:thresholding and comparing");
    message ("wpethreshto0 done", verb_debug);
     _TICTOC_TOC_FUNC
}

WormPointExtractor::~WormPointExtractor() {
    _TICTOC_TIC_FUNC
    IplImage **imsToRelease[] = {&temporalThresh, &temporalSum, &temp32, &baksub32, &curframe32, &bak32, &threshIm32};
    const int nptrs = 7;
   // cout << "releasing ims\n";
    for (int j = 0; j < nptrs; j++) {
        if (imsToRelease[j] != NULL) cvReleaseImage(imsToRelease[j]);
    }
   
}
