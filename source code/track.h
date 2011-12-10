/*
 * Track.h
 *
 * defines the class Track
 * a track is a series of points that defines the motion of an
 * animal through a series of frames
 *
 * We store pointers to the trackpoints rather than the trackpoints themselves
 * because the trackpoints may, in principle, contain a great deal of extra
 * information
 *
 */
 
 
 #ifndef _TRACK_H
 #define _TRACK_H
 #include <vector>
#include <stdio.h>
#include "trackpoint.h"
#include "cv.h"
#include "trackbuilder.h"
#include "communicator.h"
using namespace std;
 
 class Track {
	public:
            static const bool messagesOn = true;
	Track ();
	virtual ~Track();
        /*void extendTrack (TrackPoint *pt)
         *
         *adds a point to the end of the track;
         */
	virtual void extendTrack (TrackPoint *pt);

        /* void endTrack ()
         *
         * marks track as inactive
         */
	void endTrack ();

        /*void deleteTrackPoints();
         *
         *removes all points from the track and deletes them 
         *(frees the memory pointed to by trackPoint *s)
         */
        void deleteTrackPoints();

        /*TrackPoint *nearestPointToEnd (vector<TrackPoint *> &pointList)
         *
         *returns a pointer to the track point in pointList nearest to
         *the end of the track
         *
         *this is a pointer to the track point, not a pointer to a copy of it
         * (i.e. no new memory is allocated)
         */
	TrackPoint *nearestPointToEnd (vector<TrackPoint *> &pointList);

        /* TrackPoint *nearestPointInTrack (TrackPoint *pt)
         *
         * returns a pointer to the point in the track nearest the passed in point
         *
         * this is a pointer to the track point, not a pointer to a copy of it
         * (i.e. no new memory is allocated)
         */
        TrackPoint *nearestPointInTrack (TrackPoint *pt);

        /* double distanceFromEnd (TrackPoint *pt)
         *
         * distance from the end of the track to the passed point
         */
	double distanceFromEnd (TrackPoint *pt);

        /* void draw (IplImage *dst, int x0, int y0);
         *
         * draws the track on the destination (3-color) image
         * the track is drawn offset (additively) by x0,y0
         * so if a point in the track is 300,400 and x0 = -20, y0 = -30
         * the point is drawn at 280,370 on the image
         */
        void draw (IplImage *dst, int x0, int y0);

        /*void playTrackMovie(const char *windowName, int delayms);
         *
         *plays a movie of the track points in the given window
         *delayms is the inter-frame delay
         */
        void playTrackMovie(const char *windowName, int delayms);

       

        /*double meanArea();
         *double medianArea();
         *
         *the mean and median areas of the extracted points in the track
         */
        double meanArea();
        double medianArea();

       
        virtual short int getTypeCode();

        virtual inline string name() {
            return string("Track");
        }

        virtual string saveDescription();
        /* int toDisk (FILE *f)
         * writes track to BINARY file f in the following fashion
         * int npts, (write track point) * npts
         * if writeIm is true, uses ImTrackPoint::toDisk which appends
         * the binary image to the saved point
         *
         * nonzero return value indicates error
         */
        virtual int toDisk(FILE *f);

        virtual long sizeOnDisk();
       

         /* Track *fromDisk(FILE *f, TrackPoint * (*ptLoader) (FILE *f))
         * reads track from binary file f, assuming f points to the beginning
         * of a track; uses default specified trackpoint loader
         * (e.g. &TrackPoint::fromDisk )
         *
         */
         static Track *fromDisk(FILE *f, TrackPoint *(*ptLoader) (FILE *f) = &(TrackPoint::fromDisk));

        /* inline void clearPointsOnDestruction()
         * 
         * if this function is called, then when track is destroyed,
         * it will first call delete on every trackpoint pointer
         * stored in its points vector
         *
         * this is the default behavior
         */
        inline void clearPointsOnDestruction() {
            deletePointsOnDestruction = true;
        }

        /*inline void keepPointsOnDestruction()
         * 
         *if this function is called, then when track is destroyed,
         *although the vector storing the point addresses will be destroyed,
         *the points themselves are not freed.
         *
         *use this option only if copies of the point addresses are stored
         *somewhere else and still being used
         */
        inline void keepPointsOnDestruction() {
            deletePointsOnDestruction = false;
        }

         /*static comparison methods*/
        /* Comparison function object takes two values of the same type as those contained in the range,
         * returns true if the first argument goes before the second argument in the specific strict weak ordering it defines,
         * and false otherwise.
         */
        
         static inline bool activeIsFirst (const Track *a, const Track *b) {
             return (a->active & !(b->active));
         }

         static inline bool longestIsFirst (Track *a, Track *b) {
             return (a->trackLength() > b->trackLength());
         }

         
	//accessors
	inline TrackPoint *firstPoint() {
		return (points.front());
	}
	
	inline TrackPoint *lastPoint() {
		return (points.back());
	}
	
	inline TrackPoint *nthPoint(int n) {
		return (points.at(n));
	}
	inline int trackLength () {
		return points.size();
	}
	
	inline int firstFrame() {
		return (points.front())->getFrame();
	}
	inline int lastFrame() {
		return (points.back())->getFrame();
	}
	inline bool isActive() {
		return active;
	}

         //this is a BAD BAD METHOD and will be removed very soon, just to make life easy for right now
        inline vector<TrackPoint *> *getPointVector() {
            return &points;
        }

        //prints a list of track point IDS to cout; just for debugging
        void print();
	
        string description();

	virtual void setMessageHandler (communicator *mh);
        
	protected:
            std::vector <TrackPoint *> points;
            bool active;
            bool deletePointsOnDestruction;
            inline void message(const char *msg, const verbosityLevelT verb) {
                if (messagesOn && mh != NULL) {
                    mh->message(name().c_str(), verb, msg);
                }
            }
            communicator *mh;
    private:
     static const uchar _id_code = 0x01;
	
 };
 
 
	
 #endif
 
