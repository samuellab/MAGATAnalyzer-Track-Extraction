/* 
 * File:   newDLLWrapper.h
 * Author: Marc
 *
 * Created on November 9, 2009, 9:04 AM
 *
 * (C) Marc Gershow; licensed under the Creative Commons Attribution Share Alike 3.0 United States License.
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/us/ or send a letter to
 * Creative Commons, 171 Second Street, Suite 300, San Francisco, California, 94105, USA.
 */


#ifndef _NEWDLLWRAPPER_H
#define	_NEWDLLWRAPPER_H

#include "trackextractor.h"

#ifndef EXPORT
#ifdef BUILD_DLL
/* DLL export */
#define EXPORT __declspec(dllexport)
#else
/* EXE import */
#define EXPORT __declspec(dllimport)
#endif
#endif

#ifdef	__cplusplus
extern "C" {
#endif


EXPORT int analyzeImageStack(
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
                               );

EXPORT void * createTrackExtractor(bool isMaggot);

EXPORT void * setExtractorFiles(void * tep,
                                      const char *fstub,
                                      const char *extension,
                                      int padding,
                                      const char *outputname,
                                      const char *logname,
                                      int verbosityLevel);

EXPORT void * setExtractorRanges(void * tep,
                                      int startFrame,
                                      int endFrame,
                                      int ar_x0,
                                      int ar_y0,
                                      int ar_w,
                                      int ar_h);

EXPORT void * setExtractorLimits(void * tep,
                                      double minArea,
                                      double maxArea,
                                      double maxExtractDist,
                                      double maxMaggotContourAngle,
                                      int winSize);

EXPORT void * setExtractorThresholdAndBackground(void * tep,
        double overallThreshold,
        int imStackLength, 
        int nBackgroundFrames,
        int background_resample_interval,
        double background_blur_sigma,
        int frame_norm_method,
        const char *thresholdScaleImageName,
        double blurThresholdIm_sigma);


EXPORT void * setExtractorShowProgress (void * tep,
                                              bool showExtraction);

EXPORT void * createBatchExtractor ();

EXPORT void * setDefaultTrackExtractor (void * bep, void * tep);

EXPORT void * addImageStackToBatchExtractor (void * bep, void * tep, const char *fstub, const char *outname);

EXPORT void * batchExtractorToYAML (void * bep, char *description_destination, int maxChars);

EXPORT void * saveBatchExtractorToDisk (void * bep, const char *fname);

EXPORT void * loadBatchExtractorFromDisk (const char *fname, char *errorMessage, int maxErrorChars);

EXPORT void * loadBatchExtractorFromString (const char *str, char *errorMessage, int maxErrorChars);

EXPORT void * runBatchExtractor (void * bep);

EXPORT void * runBatchExtractorToMaggot (void * bep, double maxMaggotAngle /* = -1 */);

EXPORT void clearBatchExtractor (void * bep);

EXPORT void * runBatchForeground (void * bep);

EXPORT void reprocessMaggotFile (const char *existingFile, const char *newFile, int minArea, int maxArea, double maxMaggotAngle, int verbosity, int showExtraction);

#ifdef	__cplusplus
}
#endif

#endif	/* _NEWDLLWRAPPER_H */

