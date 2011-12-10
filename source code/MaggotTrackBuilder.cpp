/* 
 * File:   MaggotTrackBuilder.cpp
 * Author: Marc
 * 
 * Created on November 6, 2009, 10:34 AM
 * (C) Marc Gershow; licensed under the Creative Commons Attribution Share Alike 3.0 United States License.
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/us/ or send a letter to
 * Creative Commons, 171 Second Street, Suite 300, San Francisco, California, 94105, USA.
 */
#include <vector>
#include "Track.h"
#include "TrackBuilder.h"
#include "MaggotTrackBuilder.h"
#include "MaggotTrackPoint.h"
#include "tictoc.h"
#include "highgui.h"

using namespace std;

MaggotTrackBuilder::MaggotTrackBuilder() {
}
MaggotTrackBuilder::MaggotTrackBuilder(int startFrame) : TrackBuilder(startFrame){
    
}
void MaggotTrackBuilder::analyzeMaggotTracks(double minArea, double maxArea, double maxAngle, bool showExtraction) {
    //vector<Track *>::iterator it;
    _TICTOC_TIC_FUNC;
    message("AMT called", verb_debug);
    int j = 0;
    for (vector<Track *>::iterator it = activetracks.begin(); it != activetracks.end(); ++it){
        stringstream s; s << "track # " << ++j << "is an active ";
        s << (*it)->description();
        message (s.str().c_str(), verb_message);
     //   cout << j <<"/" <<activetracks.size() + finishedtracks.size() << "\t";
        s.str("");
        if (showExtraction && (*it)->trackLength() > minlentoshow) {
            s << "track # " << j;
            cvNamedWindow(s.str().c_str(), 0);
        }
        MaggotTrackPoint::analyzeTrack((*it)->getPointVector(), minArea, maxArea, maxAngle, s.str());
        if (!s.str().empty()) {
            cvDestroyWindow(s.str().c_str());
        }
    }
    for (vector<Track *>::iterator it = finishedtracks.begin(); it != finishedtracks.end(); ++it) {
        
        stringstream s; s << "track # " << ++j << "is a finished ";
        s << (*it)->description();
        message (s.str().c_str(), verb_message);
    //    cout << j <<"/" <<activetracks.size() + finishedtracks.size() << "\t";
        s.str("");
        if (showExtraction && (*it)->trackLength() > minlentoshow) {
            s << "track # " << j;
            cvNamedWindow(s.str().c_str(), 0);
        }
        MaggotTrackPoint::analyzeTrack((*it)->getPointVector(), minArea, maxArea, maxAngle, s.str());
        if (!s.str().empty()) {
            cvDestroyWindow(s.str().c_str());
        }
    }
    _TICTOC_TOC_FUNC;
}

MaggotTrackBuilder *MaggotTrackBuilder::fromDisk (FILE *f, TrackPoint *(*ptLoader) (FILE *f) ) {
    MaggotTrackBuilder *tb = new MaggotTrackBuilder;
    if (tb->loadTracksFromDisk(f, ptLoader) != 0) {
        delete tb;
        return NULL;
    } else {
        return tb;
    }
}
