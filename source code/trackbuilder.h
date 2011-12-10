/* trackbuilder.h
 *
 * defines the TrackBuilder class
 *
 * to build a track, we have the following sequences of activities
 * (1) nextFrame()
 * tell trackbuilder you're starting a new frame this will clear memory
 * and information used to build the previous frame
 *
 * (2) addpoints(points)
 * give trackbuilder a list of new points to stitch into tracks
 *
 *
 * (3) extendTracks()
 *   tell trackbuilder to stitch tracks together
 *   we declare this method to be virtual, so that subclasses may
 *   use different algorithms
 *
 * alternately, if TrackBuilder has been provided a pointer to a point
 * extractor we can use completeStep() to get points from the point extractor
 * start a new frame, and stitch them together
 *
 * (C) Marc Gershow; licensed under the Creative Commons Attribution Share Alike 3.0 United States License.
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/us/ or send a letter to
 * Creative Commons, 171 Second Street, Suite 300, San Francisco, California, 94105, USA.
 */
 
 #ifndef _track_builder_h
 #define _track_builder_h
 
 #include <vector>
#include <map>
#include <sstream>
 #include "cv.h"
#include "communicator.h"
#include "trackpoint.h"
#include "PointExtractor.h"
using namespace std;

// class TrackPoint;
 class Track;
 class TrackMatch;

 enum whichTracksOutT {get_all_tracks, get_active_tracks, get_finished_tracks};
 
 typedef TrackPoint *(*ptLoaderT) (FILE *f);

 class TrackBuilder {
    const static bool messagesOn = true;
    public:
        /* TrackBuilder
         *
         * startFrame - the first frame to be analyzed; default is 0
         */
        TrackBuilder(int startFrame);
        TrackBuilder();

        /*~TrackBuilder();
         *
         * on destruction, no tracks are destroyed, on the assumption that
         * they may have been outputted elsewhere/used in different contexts
         */
        ~TrackBuilder();

        /* void deleteAllTracks()
         *
         * this deletes all tracks stored in the track builder
         */
        void deleteAllTracks();
        
        /*************************************************************
         *              TRACK BUILDING PARAMETERS                    *
         *************************************************************/

       /* void setConnectionDistance (double maxDist)
        * double getConnectionDistance
        *
        * maximum distance between two consecutive points in a track
        * 
        */
        inline void setConnectionDistance (double maxDist) {
            this->maxDist = maxDist;
        }
        inline double getConnectionDistance () {
            return maxDist;
        }

        /* inline void setPointExtractor (PointExtractor *pe)
         * inline void deletePointExtractor ()
         *
         * setting the point extractor allows TB to resolve collisions
         * by splitting points: requesting PE to re-examine a particular
         * region and create multiple track points from that region if possible
         *
         * if PE is set, it is important for it to remained synced with TB
         * (on the same frame) as tracks are being built.  For this reason,
         * you may want to use completeStep() to build tracks if pe != NULL
         */
        
        inline void setPointExtractor (PointExtractor *pe) {
            this->pe = pe;
        }

        inline void deletePointExtractor () {
            delete pe;
            pe = NULL;
        }

        /*************************************************************
         *              TRACK BUILDING FUNCTIONS                     *
         *************************************************************/

        
     

        /* void nextFrame();
         * void nextFrame (int frameNum);
         *
         * resets the state of the TrackBuilder to ready it to accept
         * points for stitching in a new frame
         */

        void nextFrame();
        void nextFrame (int frameNum);


        /* void addPoint (double x, double y);
         * void addPoint (TrackPoint *pt);
         * void addPoints (vector<TrackPoint *> *ptvec);
         *
         * adds (a) point(s) to the list of points to be incorporated into tracks
         * for the current frame
         * in the case that (x,y) is passed, a new TrackPoint is created on the heap
         * and a pointer to it stored
         *
         * in all other cases, the pointer to the track point is stored; no new memory
         * is allocated.  therefore:
         * DO NOT PASS POINTERS TO STATICALLY ALLOCATED POINTS to this function
         * or you will cause a severe problem down the road
         */
        
        void addPoint (double x, double y);
        void addPoint (TrackPoint *pt);
        void addPoints (vector<TrackPoint *> *ptvec);

        /*  extendTracks();
         *
         * after points are added to the track builder, this call
         * stitches the new points into existing tracks, creates tracks
         * out of points that don't match any others, and ends tracks with no
         * matches
         */
        virtual void extendTracks();


        /* int completeStep();
         * 
         * if point extractor has been set, then this executes the entire 
         * extraction for a single frame;
         * advances both PE and TB one frame
         * gets points from PE and gives them to TB
         * extends tracks
         *
         * returns non-zero on error (e.g. if PE failed to load frame)
         */
        int completeStep();

        /******************************************************************
         *                  ACCESS/OUTPUT FUNCTIONS                       *
         ******************************************************************/

        

        /* void drawTracks(IplImage *dst, int x0, int y0);
         * void extractionImage(IplImage **dst);
         * 
         * draw tracks draws all the extracted tracks in dst.
         * x0,y0 is added to every point as an offset
         *
         * extractionImage copies the background subtracted image
         * into *dst (allocating memory if needed) and draws tracks
         * on top of it
         */


        void extractionImage(IplImage **dst, const char *screenMessage = NULL);
        void drawTracks(IplImage *dst, int x0, int y0);

        /* vector<Track *> *getTracksOut (whichTracksOutT whichTracks)
         *
         * returns a vector of Track pointers.  The vector either contains
         * all tracks, active tracks, or finished tracks
         * No new memory is allocated for tracks here (pointers to original tracks
         * are stored in the vector), but a unique vector is created on the heap
         * caller is responsible for deleting this vector when finished
         */
        vector<Track *> *getTracksOut (whichTracksOutT whichTracks);

        /* virtual string saveDescription();
         * virtual int toDisk(FILE *f);
         * static TrackBuilder *fromDisk (FILE *f, TrackPoint *(*ptLoader) (FILE *f) = &(TrackPoint::fromDisk));
         *
         * toDisk saves the trackbuilder to the open binary file f
         * a description of the format can be found by calling saveDescription
         * fromDisk loads the trackbuilder from disk
         * you must pass in the correct TrackPoint::fromDisk static method.
         * e.g. if you are loading ImTrackPoints use ImTrackPoint::fromDisk
         *
         */
        virtual string saveDescription();
        virtual int toDisk(FILE *f);
        static TrackBuilder *fromDisk (FILE *f, TrackPoint *(*ptLoader) (FILE *f) = NULL);

        int loadTracksFromDisk (FILE *f, TrackPoint *(*ptLoader) (FILE *f) = NULL, int minLenToLoad = 0);

        virtual void setMessageHandler (communicator *mh); 
        void printTracks(bool active);

        virtual int getTypeCode();

        static ptLoaderT pointLoader(int code);

    protected:

        std::vector <Track *> activetracks;
        std::vector <Track *> finishedtracks;
        std::vector <TrackPoint *> newPoints;
        std::map<int, TrackMatch *> matches;
        PointExtractor *pe;
        
      //  bool oneToOne;
       // bool noCollisions;
        int frameNumber;

        double maxDist;

        communicator *mh; 

        /*matchTracksToClosest();
         *Iterates through all active tracks and adds each one to the next point that is closest
         *to the end of the track; regardless of maxDist
         */
        virtual void matchTracksToClosest();

        /* endTooFar()
         * iterates through all track matches, and sets to inactive any
         * tracks that are farther than maxDist from the nearest new point
         *
         */
        virtual void endTooFar();

        /* extendExactMatches()
         * iterates through all track matches, and extends all one point-one
         * track matches, then removes these matches from the matches map
         * 
         */
        virtual void extendExactMatches();


        virtual void matchCollisionsToEmptyPoints();

        virtual void startNewTracks();

        virtual void endCollidingTracks();

        virtual void pruneInactive();
        virtual void handleCollisions();

        virtual void splitCollisionPoint(TrackMatch *m);
        virtual void splitCollisionPoints();
        virtual Track *createTrack(TrackPoint *pt);

        virtual void endAllTracks();


        void assignTracks(const vector<Track *> &tracks);


        virtual void Init (int startFrame);

        inline void message(const char *msg, const verbosityLevelT verb) {
            if (messagesOn && mh != NULL) {
                //char s[128];
//                std::stringstream s("TrackBuilder Frame: ");
  //              s << frameNumber;
                //sprintf(s, "Track Builder Frame: %d", frameNumber);
                mh->message("Track Builder Frame", verb, frameNumber, msg);
            }
        }
        inline virtual uchar my_type_code () {
            return _id_code;
        }
    private:
     static const uchar _id_code = 0x01;
 };




 
 
 
 #endif
