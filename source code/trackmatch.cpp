/* 
 * File:   trackmatch.cpp
 * Author: Marc
 * 
 * Created on October 13, 2009, 3:44 PM
 *
 * (C) Marc Gershow; licensed under the Creative Commons Attribution Share Alike 3.0 United States License.
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/us/ or send a letter to
 * Creative Commons, 171 Second Street, Suite 300, San Francisco, California, 94105, USA.
 */

#include "trackmatch.h"
#include <vector>

using namespace std;

 TrackMatch::TrackMatch(TrackPoint *pt) {
     this->pt = pt;
 }
 
 TrackMatch::TrackMatch(TrackPoint *pt, Track *tr) {
     this->pt = pt;
     tracks.push_back(tr);
 }
 
 bool TrackMatch::isCollision() {
     return numMatches() > 1;
 }

 int TrackMatch::numMatches() {
    return tracks.size();
 }

 void TrackMatch::addTrack (Track *tr) {
     int s = tracks.size();
     tracks.push_back(tr);
     s = tracks.size();
 }

 void TrackMatch::removeTracks() {
     tracks.clear();
 }

 void TrackMatch::endTracks () {
     vector<Track *>::iterator it;
     for (it = tracks.begin(); it != tracks.end(); ++it) {
         (*it)->endTrack();
     }
 }

 void TrackMatch::endShortTracks(int maxLen) {
     vector<Track *>::iterator it;
     for (it = tracks.begin(); it != tracks.end(); ) {
         if ((*it)->trackLength() <= maxLen) {
             (*it)->endTrack();
             it = tracks.erase(it);
         } else {
             ++it;
         }
     }
 }

 void TrackMatch::cutByDistance (double maxDist) {
     vector<Track *>::iterator it;
     if (tracks.empty())
         return;
     for (it = tracks.begin(); it != tracks.end(); ) {
         if ((*it)->distanceFromEnd (pt) > maxDist) {
             (*it)->endTrack();
             it = tracks.erase(it);
         } else {
             ++it;
         }
     }
 }

 void TrackMatch::extendTrack() {
     tracks.front()->extendTrack(pt);
 }


 double TrackMatch::minDistFromTracksToPt (TrackPoint *pt) {
     if (tracks.empty())
         return -1;
     vector<Track *>::iterator it;
     double dt, d = (tracks.front())->distanceFromEnd(pt);
     for (it = tracks.begin() + 1; it != tracks.end(); ++it) {
         if ((dt = (*it)->distanceFromEnd(pt)) < d){
             d = dt;
         }
     }
     return d;
 }

 double TrackMatch::meanAreasIn() {
     vector<Track *>::iterator it;
     double areasum = 0;
     for (it = tracks.begin(); it != tracks.end(); ++it) {
         areasum += (*it)->lastPoint()->getArea();
     }
     return areasum/numMatches();
 }

 Track *TrackMatch::popClosestTrack (TrackPoint *pt) {
     if (tracks.empty())
         return NULL;
     double dt, d = (tracks.front())->distanceFromEnd(pt);
     vector<Track *>::iterator itmin = tracks.begin();
     Track *tr;
     for (vector<Track *>::iterator it = tracks.begin() + 1; it != tracks.end(); ++it) {
         if ((dt = (*it)->distanceFromEnd(pt)) < d){
             d = dt;
             itmin = it;
         }
     }
     tr = *itmin;
     tracks.erase(itmin);
     return tr;
 }
TrackMatch::~TrackMatch() {
}



/*
TrackMatch::TrackMatch() {
}

TrackMatch::TrackMatch(const TrackMatch& orig) {
}

TrackMatch::~TrackMatch() {
}
*/
