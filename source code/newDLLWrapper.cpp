/*
 * (C) Marc Gershow; licensed under the Creative Commons Attribution Share Alike 3.0 United States License.
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/us/ or send a letter to
 * Creative Commons, 171 Second Street, Suite 300, San Francisco, California, 94105, USA.
 */

#include <cstring>
#include <iostream>
#include <stdio.h>
#include "cv.h"
#include "newDLLWrapper.h"
#include "TrackExtractor.h"
#include "MaggotTrackExtractor.h"
#include "BatchExtractor.h"
#include "MaggotReprocessor.h"
int analyzeImageStack(
        char *fstub,
        char *extension,
        int padding,
        char *outputname,
        char *logname,
        int verbosityLevel,
        int startFrame,
        int endFrame,
        int ar_x0,
        int ar_y0,
        int ar_w,
        int ar_h,
        double minArea,
        double maxArea,
        double overallThreshold,
        int winSize,
        int nBackgroundFrames,
        int background_resample_interval,
        double background_blur_sigma,
        int frame_norm_method,
        char *thresholdScaleImageName,
        double blurThresholdIm_sigma,
        double maxExtractDist,
        int showExtraction,
        int isMaggot,
        double maxMaggotContourAngle
                               ) {
    std::ofstream out("c:\\marc_dll_log.txt");

    TrackExtractor *te;
    out << "creating TE" << "\n";
    if (isMaggot) {
        te = new MaggotTrackExtractor();
    } else {
        te = new TrackExtractor();
    }
    if (fstub != NULL) te->fstub = std::string(fstub);
    if (extension != NULL) te->extension = std::string(extension);
    te->padding = padding;
    out << "creating file names\n";
    if (outputname != NULL) {
        te->outputname = std::string(outputname);
        if (!te->outputname.empty()) {
            size_t find1, find2;
            find1 = te->outputname.find('.');
            if (find1 != string::npos) {
                while ((find2 = te->outputname.find('.', find1 +1)) != string::npos) {
                    find1 = find2;
                }
            }
            std::string s;
            if (find1 != string::npos) {
                s = te->outputname.substr(0, find1 - 1);
            } else {
                s = te->outputname;
            }
            s.append("_header.txt");
            te->headerinfoname = s;
        }
    }
    out << "lots of other shit\n";
    if (logname != NULL) te->logName = std::string(logname);
    te->logVerbosity = (verbosityLevelT) verbosityLevel;
    te->startFrame = startFrame;
    te->endFrame = endFrame;
    te->analysisRectangle = cvRect(ar_x0, ar_y0, ar_w, ar_h);
    te->minArea = minArea;
    te->maxArea = maxArea;
    te->overallThreshold = overallThreshold;
    te->winSize = winSize;
    te->nBackgroundFrames = nBackgroundFrames;
    te->background_resampleInterval = background_resample_interval;
    te->background_blur_sigma = background_blur_sigma;
    te->fnm = (_frame_normalization_methodT) frame_norm_method;
    if (thresholdScaleImageName != NULL) te->thresholdScaleImage = std::string(thresholdScaleImageName);
    te->blurThresholdIm_sigma = blurThresholdIm_sigma;
    te->maxExtractDist = maxExtractDist;
    te->showExtraction = showExtraction;
    if (isMaggot) {
        MaggotTrackExtractor *mte = dynamic_cast<MaggotTrackExtractor *> (te);
        if (mte == NULL) {
            out << "dynamic cast failed!\n";
        } else {
            mte->setMaxAngle(maxMaggotContourAngle);
        }
    }
    int rv;
    out << "go\n";
    if ((rv = te->go()) != 0) {
        return rv;
    }
    out << "saveOutput\n";
    if ((rv = te->saveOutput()) != 0) {
        return rv;
    }
    return rv;

}

 void * createTrackExtractor(bool isMaggot) {
    TrackExtractor *te;
    if (isMaggot) {
        te = new MaggotTrackExtractor();
    } else {
        te = new TrackExtractor();
    }
    return (void *) te;
}

 void * setExtractorFiles(void * tep,
                                      const char *fstub,
                                      const char *extension,
                                      int padding,
                                      const char *outputname,
                                      const char *logname,
                                      int verbosityLevel) {
    TrackExtractor *te = (TrackExtractor *) tep;

    if (fstub != NULL) te->fstub = std::string(fstub);
    if (extension != NULL) te->extension = std::string(extension);
    te->padding = padding;
    //create the header by adding _header.txt to end of output name minus
    //extension
    if (outputname != NULL) {
        te->outputname = std::string(outputname);
        if (!te->outputname.empty()) {
            size_t find1, find2;
            find1 = te->outputname.find('.');
            if (find1 != string::npos) {
                while ((find2 = te->outputname.find('.', find1 +1)) != string::npos) {
                    find1 = find2;
                }
            }
            std::string s;
            if (find1 != string::npos) {
                s = te->outputname.substr(0, find1 - 1);
            } else {
                s = te->outputname;
            }
            s.append("_header.txt");
            te->headerinfoname = s;
        }
    }
    if (logname != NULL) te->logName = std::string(logname);
    te->logVerbosity = (verbosityLevelT) verbosityLevel;
    return (void *) te;
}

 void * setExtractorRanges(void * tep,
                                      int startFrame,
                                      int endFrame,
                                      int ar_x0,
                                      int ar_y0,
                                      int ar_w,
                                      int ar_h) {
    TrackExtractor *te = (TrackExtractor *) tep;
    te->startFrame = startFrame;
    te->endFrame = endFrame;
    te->analysisRectangle = cvRect(ar_x0, ar_y0, ar_w, ar_h);
    return (void *) te;
}

 void * setExtractorLimits(void * tep,
                                      double minArea,
                                      double maxArea,
                                      double maxExtractDist,
                                      double maxMaggotContourAngle,
                                      int winSize) {
    TrackExtractor *te = (TrackExtractor *) tep;
    te->minArea = minArea;
    te->maxArea = maxArea;
    te->winSize = winSize;
    te->maxExtractDist = maxExtractDist;
    MaggotTrackExtractor *mte = dynamic_cast<MaggotTrackExtractor *> (te);
    if (mte != NULL) {
            mte->setMaxAngle(maxMaggotContourAngle);
    }
    return (void *) te;
}

 void * setExtractorThresholdAndBackground(void * tep,
        double overallThreshold,
        int imStackLength,
        int nBackgroundFrames,
        int background_resample_interval,
        double background_blur_sigma,
        int frame_norm_method,
        const char *thresholdScaleImageName,
        double blurThresholdIm_sigma){


    TrackExtractor *te = (TrackExtractor *) tep;
    te->overallThreshold = overallThreshold;
    te->imstacklength = imStackLength;
    te->nBackgroundFrames = nBackgroundFrames;
    te->background_resampleInterval = background_resample_interval;
    te->background_blur_sigma = background_blur_sigma;
    te->fnm = (_frame_normalization_methodT) frame_norm_method;
    if (thresholdScaleImageName != NULL) te->thresholdScaleImage = std::string(thresholdScaleImageName);
    te->blurThresholdIm_sigma = blurThresholdIm_sigma;
    return (void *) te;
}

void * setExtractorShowProgress (void * tep,
                                              bool showExtraction){


    TrackExtractor *te = (TrackExtractor *) tep;
    te->showExtraction = showExtraction;
    return (void *) te;
}

void * createBatchExtractor () {
    BatchExtractor *be = new BatchExtractor();
    return (void *) be;
}

void * setDefaultTrackExtractor (void * bep, void * tep) {
    BatchExtractor *be = (BatchExtractor *) bep;
    TrackExtractor *te = (TrackExtractor *) tep;
    be->setDefaultTrackExtractor(te);
    return (void *) be;
}

void * addImageStackToBatchExtractor (void * bep, void * tep, const char *fstub, const char *outname) {
    BatchExtractor *be = (BatchExtractor *) bep;
    TrackExtractor *te = (TrackExtractor *) tep;
    std::string fs, on;
    if (fstub == NULL) {
        assert (te != NULL);
        fs = te->fstub;
    } else {
        fs = std::string(fstub);
    }
    if (outname == NULL) {
        assert (te != NULL);
        on = te->outputname;
    } else {
        on = std::string(outname);
    }
    be->addImageStack(fs, on, te);
    return (void *) be;
}

void * batchExtractorToYAML (void * bep, char *description_destination, int maxChars) {

    BatchExtractor *be = (BatchExtractor *) bep;
    YAML::Emitter out;
    be->toYaml(out);
    strncpy (description_destination, out.c_str(), maxChars);

    return (void *) be;
}

void * saveBatchExtractorToDisk (void * bep, char *fname){
    BatchExtractor *be = (BatchExtractor *) bep;
    YAML::Emitter out;
    be->toYaml(out);
    ofstream os(fname);
    os << out.c_str();
    std::cout << out.GetLastError() << "\n";
    assert(out.good());
    return (void *) be;
}


void * loadBatchExtractorFromDisk (const char *fname, char *errorMessage, int maxErrorChars) {
    BatchExtractor *be = new BatchExtractor();
    YAML::Node node;
    try {
        ifstream is(fname);
        YAML::Parser parser(is);
        
        parser.GetNextDocument(node);
        std::cout << "parsed\n";
        
        
    } catch (YAML::ParserException &e) {
        std::cout << "Parser Error " << e.what() << "\n";
        if (errorMessage != NULL) {
            strncpy(errorMessage, e.what(), maxErrorChars);
        }
        delete be;
        return (void *) NULL;
    }
    
    if (be->fromYaml(node, errorMessage, maxErrorChars)) {
        delete be;
        
        return (void *) NULL;
        
    }
    if (errorMessage != NULL) {
        strncpy(errorMessage, "no error", maxErrorChars);
    }
    return (void *) be;
}


void * loadBatchExtractorFromString (const char *str, char *errorMessage, int maxErrorChars) {
    BatchExtractor *be = new BatchExtractor();
    YAML::Node node;
    std::string s(str);
 
    istringstream is(s);
    try {
        
        YAML::Parser parser(is);
        
        parser.GetNextDocument(node);
        std::cout << "parsed\n";
         

    } catch (YAML::ParserException &e) {
        std::cout << "Parser Error " << e.what() << "\n";
         if (errorMessage != NULL) {
            string errmsg = string("parser error: ") + e.what();
            strncpy(errorMessage, errmsg.c_str(), maxErrorChars);
        }
        delete be;
        return (void *) NULL;
    }
    if (be->fromYaml(node, errorMessage, maxErrorChars)) {
        delete be;
        return (void *) NULL;
    }
    if (errorMessage != NULL) {
        strncpy(errorMessage, "no error", maxErrorChars);
    }
    return (void *) be;

}

void * runBatchExtractor (void * bep){
    BatchExtractor *be = (BatchExtractor *) bep;
    be->batchProcess();
    return (void *) be;
}

void * runBatchExtractorToMaggot (void * bep, double maxMaggotAngle){
    BatchExtractor *be = (BatchExtractor *) bep;
    be->batchToMaggot(maxMaggotAngle);
    return (void *) be;
}

void clearBatchExtractor (void * bep) {
    BatchExtractor *be = (BatchExtractor *) bep;
    be->setDeleteTEOnDestruction(true);
    delete be;
 }

void * runBatchForeground (void * bep) {
    BatchExtractor *be = (BatchExtractor *) bep;
    be->batchForeground();
    return (void *) be;
}

void reprocessMaggotFile (const char *existingFile, const char *newFile, int minArea, int maxArea, double maxMaggotAngle, int verbosity, int showExtraction) {
    MaggotReprocessor::reprocessFile(string(existingFile), newFile != NULL ? string(newFile) : string(""),minArea, maxArea, maxMaggotAngle, (verbosityLevelT) verbosity, (bool) showExtraction);
}