/* 
 * File:   WormTrackBuilder.cpp
 * Author: Marc
 * 
 * Created on November 18, 2009, 3:39 PM
 */
#include <vector>
#include <map>
#include "WormTrackBuilder.h"
#include "trackbuilder.h"
#include "trackmatch.h"
static const int tooShort = 3;


void WormTrackBuilder::handleCollisions() {
    matchCollisionsToEmptyPoints();
    killShortCollidingTracks();
    endCollidingTracks();
}

void WormTrackBuilder::killShortCollidingTracks() {
    map<int, TrackMatch*>::iterator it;
    for (it = matches.begin(); it != matches.end(); ++it) {
        if (it->second->numMatches() > 1) {
            it->second->endShortTracks(tooShort);
        }
    }
}