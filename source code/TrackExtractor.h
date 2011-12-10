/* 
 * File:   TrackExtractor.h
 * Author: Marc
 *
 * Created on November 6, 2009, 10:48 AM
 *
 * Gets all the parameters for analyzing an image stack and saving the results to
 * disk into one happy place
 *
 * You're happy right?
 *
 * OK, look, I know you're not supposed to make data members public
 * but in this one case, where we're just grouping a bunch of analysis parameters
 * together, I'm going to do it, so we don't have to write a bunch of stupid accessors
 *
 * (C) Marc Gershow; licensed under the Creative Commons Attribution Share Alike 3.0 United States License.
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/us/ or send a letter to
 * Creative Commons, 171 Second Street, Suite 300, San Francisco, California, 94105, USA.
 */

#ifndef _TRACKEXTRACTOR_H
#define	_TRACKEXTRACTOR_H

#include <cstring>
#include <ctime>
#include "communicator.h"
#include "cxtypes.h"
#include "PointExtractor.h"
#include "TrackBuilder.h"
using std::string;


class TrackExtractor {
public:

    /********************************************************************
     *       EXTRACTION PARAMETERS (PUBLIC DATA)                        *
     ********************************************************************/

     /*****************************
     *  FILE INPUT/OUTPUT PARAMS *
     *****************************/

    string fstub; //input
    string extension; //input
    int padding;
    string outputname; //output
    string headerinfoname; //output
    string logName; //if empty, log goes to cout
    verbosityLevelT logVerbosity; //set to off for no messages out


    /**********************************
     *  REGION EXTRACTION PARAMS      *
     **********************************/

   
    int startFrame;
    int endFrame;
    CvRect analysisRectangle;

    double minArea;
    double maxArea;
    double overallThreshold; //if thresholdScaleImage is empty, this is the global threshold
    
    int winSize; //size of image to save with every track Point; -1 for off
    int nBackgroundFrames;
    int background_resampleInterval;
    double background_blur_sigma;

    _frame_normalization_methodT fnm; //whether to rescale images to take into account illumination variations

    //thresholdScaleImage is a grayscale image with values 0 to 255
    //if t is the overall threshold, then the thresholdCompareImage is created by
    //t * thresholdScaleImage / 255;
     string thresholdScaleImage;
    
    
    double blurThresholdIm_sigma;

    //for worm (not maggot) extraction,
    //if > 1, we extract from a stack of consecutive frames, reducing noise
    //at the expense of time.  
    int imstacklength;
    /****************************************
     *      TRACK STITCHING PARAMS          *
     ****************************************/

    double maxExtractDist;

    /****************************************
     *      OPTIONAL PARAMS                 *
     ****************************************/
    bool showExtraction; //whether to show the points as they are extracted; slows stuff down

    /***************************************************************************************/

    TrackExtractor();
    virtual ~TrackExtractor();
    void reset(); //clears track builder, pointextractor, and communicator to get ready for next pass
    virtual int go();
    virtual int saveOutput();
    virtual YAML::Emitter& toYAML(YAML::Emitter& out) const;
    void fromYAML(const YAML::Node &node);
    void makeForeground(string outname);
    // a kludge so we can get dennis's data analyzed; fix later
    virtual inline void setTrackBuilder (TrackBuilder *tb) {
        this->tb = tb;
    }
    virtual void setMessageHandler(communicator* mh);
protected:
    /* virtual void createTrackBuilder();
     * virtual void createPointExtractor();
     * having these methods virtual lets us have subclasses create different
     * implementations of trackbuilder and pointextractor
     * whill still using the same go() method
     */
    virtual void createTrackBuilder();
    virtual void createPointExtractor();
    communicator *mh;
    TrackBuilder *tb;
    PointExtractor *pe;
    int getThresholdImage();
    virtual YAML::Emitter& yamlBody(YAML::Emitter& out) const;
    virtual void startMessage();
    virtual void endMessage();
    bool aborted;
    void ready();
    clock_t starttime;
    virtual inline string name() {
        return string("track extractor");
    }
    inline void message(const char *msg, const verbosityLevelT verb) {
        if (mh != NULL) {
            mh->message(name().c_str(), verb, msg);
        }
    }
    virtual void headerinfo (std::ostream &os);


};

YAML::Emitter& operator << (YAML::Emitter& out, const TrackExtractor *te);
YAML::Emitter& operator << (YAML::Emitter& out, const TrackExtractor &te);
void operator >> (const YAML::Node &node, TrackExtractor * &te);
void operator >> (const YAML::Node &node, TrackExtractor (&te));
#endif	/* _TRACKEXTRACTOR_H */

