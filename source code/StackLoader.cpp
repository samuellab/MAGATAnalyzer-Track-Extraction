/* 
 * File:   StackLoader.cpp
 * Author: Marc
 * 
 * Created on November 2, 2010, 2:34 PM
 * (C) Marc Gershow; licensed under the Creative Commons Attribution Share Alike 3.0 United States License.
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/us/ or send a letter to
 * Creative Commons, 171 Second Street, Suite 300, San Francisco, California, 94105, USA.
 */

#include <sstream>

#include "StackLoader.h"
#include "tictoc.h"
#include "StackReader.h"
using namespace std;
StackLoader::StackLoader() {
    init(); 
}
void StackLoader::init() {
    sr = NULL;
}

void StackLoader::checkAndLoadSR() {
    if (sr == NULL) {
        string fname = string(fileName) + string(".") + string (extension);
        string msg = "loading image stack " + fname;
        message(msg.c_str(), verb_message);
        sr = new StackReader(fname.c_str());
        
        if (sr->isError()) {
            msg = "stack loader error: " + sr->getError();
            message (msg.c_str(), verb_error);
        }
        if (!sr->dataFileOk()) {
            msg = "failed to open image stack " + fname;
            message(msg.c_str(), verb_error);

        }
    }
    if (sr != NULL) {
        stringstream ss;
        ss << "stack has "  << sr->getTotalFrames() << " total frames";
        message(ss.str().c_str(), verb_message);
    } else {
        message ("error opening stack reader", verb_error);
    }
}

int StackLoader::loadWholeFrame(int frameNumber) {
    _TICTOC_TIC_FUNC
    checkAndLoadSR();
    if (sr == NULL) {
        message("stack reader unexpectedly null", verb_error);
    }
    sr->getFrame(frameNumber, &loadIm);
    if (loadIm == NULL) {
        _TICTOC_TOC_FUNC;
        return -1;
    }
    _TICTOC_TOC_FUNC
    return 0;
}

int StackLoader::getStackLength() {
    checkAndLoadSR();
    return sr->getTotalFrames();
}

void StackLoader::getBackground(int frameNumber, IplImage** dst, int frameRange) {
    _TICTOC_TIC_FUNC
    checkAndLoadSR();
    if (sr == NULL) {
        message("stack reader unexpectedly null", verb_error);
    }
    sr->getBackground(frameNumber, dst, frameRange);
    _TICTOC_TOC_FUNC
}

StackLoader::StackLoader(const StackLoader& orig) {
}

StackLoader::~StackLoader() {
    if (sr != NULL) {
        sr->closeInputFile();
        delete sr;
        sr = NULL;
    }
}

void StackLoader::writeMetaData(string binfileoutputname) {
     int lastval = binfileoutputname.find_last_of('.');
     string outname = binfileoutputname.substr(0,lastval) + string(".mdat");     
     checkAndLoadSR();
     string msg = "attempting to create meta data file: " + outname;
     message(msg.c_str(), verb_message);
     if (sr == NULL) {
        message("stack reader unexpectedly null", verb_error);
        return;
    }
     sr->createSupplementalDataFile(outname.c_str());
     message("meta data file created", verb_message);
}

CvRect StackLoader::getImageRectangle() {
     _TICTOC_TIC_FUNC
    checkAndLoadSR();
    if (sr == NULL) {
        message("stack reader unexpectedly null", verb_error);
    }
    CvRect r = sr->getLargestROI();
    return r;
    _TICTOC_TOC_FUNC
}
