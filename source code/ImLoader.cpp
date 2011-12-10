/* 
 * File:   ImLoader.cpp
 * Author: Marc
 * 
 * Created on October 23, 2009, 3:24 PM
 * (C) Marc Gershow; licensed under the Creative Commons Attribution Share Alike 3.0 United States License.
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/us/ or send a letter to
 * Creative Commons, 171 Second Street, Suite 300, San Francisco, California, 94105, USA.
 */
#include <string.h>
#include <stdio.h>
#include <sstream>
#include "ImLoader.h"
#include "FrameLoader.h"
#include "cv.h"
#include "cxcore.h"
#include "highgui.h"
#include "CvUtilsPlusPlus.h"
#include "tictoc.h"
ImLoader::ImLoader() {
    padding = -1;
}


ImLoader::~ImLoader() {
}

/*
int ImLoader::getFrame(int frameNumber, IplImage** dst) {
    message ("getFrame called ", verb_debug);
    char formatString[64];
    char *fname = (char *) malloc (strlen(fileName) + strlen(extension) + 64);
    if (padding < 0) {
        strcpy (formatString, "%s%d.%s");
    } else {
        sprintf (formatString, "%%s%%0%dd.%%s", padding);
    }
    sprintf (fname, formatString, fileName, frameNumber, extension);
    string msg = "attempting to load ";
    msg += fname;
    message (msg.c_str(), verb_debug);
    IplImage *im = cvLoadImage(fname, CV_LOAD_IMAGE_GRAYSCALE);
    if (im == NULL) {
        stringstream s("failed to load ");
        s << fname << "\n";
        message (s.str().c_str(), verb_error);
        free (fname);
        return -1;
    }
    if (ar.width < 0 || ar.height < 0) {
        message ("analysis rectangle was not set", verb_warning);
        ar.x = 0;
        ar.y = 0;
        ar.width = im->width;
        ar.height = im->height;
    }
    subImage (im, dst, ar);
    
    cvReleaseImage(&im);
    free (fname);
    return 0;
}
*/


int ImLoader::loadWholeFrame(int frameNumber) {
    _TICTOC_TIC_FUNC;
    message ("load whole frame called ", verb_debug);
    char formatString[64];
    char *fname = (char *) malloc (strlen(fileName) + strlen(extension) + 64);
    if (padding <= 0) {
        strcpy (formatString, "%s%d.%s");
    } else {
        sprintf (formatString, "%%s%%0%dd.%%s", padding);
    }
    sprintf (fname, formatString, fileName, frameNumber, extension);
    string msg = "attempting to load ";
    msg += fname;
    message (msg.c_str(), verb_debug);
    if (loadIm != NULL) {
        message ("freeing already allocated loadIm", verb_debug);
        cvReleaseImage(&loadIm);
        message ("freed!", verb_debug);
    }
    loadIm = cvLoadImage(fname, CV_LOAD_IMAGE_GRAYSCALE);
    if (loadIm == NULL) {
        message ("load failed", verb_error);
        _TICTOC_TOC_FUNC;
        return -1;
    } else {
        message ("load succeeded", verb_debug);
        lastFrameLoaded = frameNumber;
        _TICTOC_TOC_FUNC;
        return 0;
    }   
}