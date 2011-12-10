/* 
 * File:   trackmatch.h
 * Author: Marc
 *
 * Created on October 13, 2009, 3:44 PM
 *
 * A TrackMatch stores a single TrackPoint and any number of tracks that
 * may connect to it.
 *
 * This class is used by track builder
 *
 * (C) Marc Gershow; licensed under the Creative Commons Attribution Share Alike 3.0 United States License.
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/us/ or send a letter to
 * Creative Commons, 171 Second Street, Suite 300, San Francisco, California, 94105, USA.
 */

#ifndef _TRACKMATCH_H
#define	_TRACKMATCH_H
#include <vector>
#include "track.h"
using namespace std;

class TrackMatch {
public:
    TrackMatch(TrackPoint *pt);
    TrackMatch(TrackPoint *pt, Track *tr);

    virtual ~TrackMatch();
    bool isCollision();
    int numMatches();

    void addTrack (Track *tr);

    //removes all tracks from the matching list
    
    void removeTracks();

    //marks all tracks as inactive
    void endTracks ();

    //endShortTracks (len <= maxLen)
    //marks short tracks as inactive and removes them from the match
    void endShortTracks(int maxLen);

    //extends the first track in the track vector
    void extendTrack();

    void cutByDistance (double maxDist);

    double minDistFromTracksToPt (TrackPoint *pt);

    Track *popClosestTrack (TrackPoint *pt);

    double meanAreasIn();

    /*  inline TrackPoint *getPointPtr ()
     *  returns a pointer to pt;  no new memory is allocated
     */
    inline TrackPoint *getPointPtr () {
        return pt;
    }
    /* inline vector<Track*> *getTracksPtr ()
     * returns a pointer to the vector of tracks;  no new memory is allocated
     */
    inline vector<Track*> *getTracksPtr () {
        return &tracks;
    }

     /*static comparison methods*/
     /* Comparison function object takes two values of the same type as those contained in the range,
      * returns true if the first argument goes before the second argument in the specific strict weak ordering it defines,
      * and false otherwise.
      */
  //  static inline bool collisionsToBack(TrackMatch a, TrackMatch b) {
    //    return (b.isCollision() && !a.isCollision());
    //}
    static inline bool collisionsToBack(TrackMatch *a, TrackMatch *b) {
        return (b->isCollision() && !a->isCollision());
    }

    protected:
     TrackPoint *pt;
     vector <Track*> tracks;

    private:
    TrackMatch(TrackMatch& orig);


};

#endif	/* _TRACKMATCH_H */

