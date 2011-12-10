/* trackpoint.h
 *
 * defines the class TrackPoint
 *
 * minimal information for tracking an animal; the location and frame #
 *
 * (C) Marc Gershow; licensed under the Creative Commons Attribution Share Alike 3.0 United States License.
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/us/ or send a letter to
 * Creative Commons, 171 Second Street, Suite 300, San Francisco, California, 94105, USA.
 */


 #ifndef _TRACKPOINT_H
 #define _TRACKPOINT_H

#include <stdio.h>
#include <cmath>
#include <string>
#include "cv.h"
#include "yaml.h"
#include "communicator.h"

//using namespace std;


/*ptLoc is a utility structure that stores two doubles
 * as an x,y point
 * this really should be a CvPoint2D32f and may be updated to that in a future
 * release
 */

using std::string;

struct ptLoc {
    double x;
    double y;
};

class TrackPoint {

	 public:

             static const bool messagesOn = true;

            /*TrackPoint (double x, double y, int t)
             *
             * creates a track point with location x,y and frame t
             * the ID code is generated automatically and is guaranteed to be
             * different from all other automatically generated IDs
             */
//            TrackPoint (double x, double y, int t);

            /*TrackPoint (double x, double y, int t, int ID)
             *
             * creates a track point with location x,y and frame t
             * the ID code is passed in by the creator and should
             * be different from any other ID code, or track match may break
             */
  //          TrackPoint (double x, double y, int t, int ID);


            /*TrackPoint (double x, double y, double cov[], int t)
             *
             * creates a track point with location x,y and frame t
             * the ID code is generated automatically and is guaranteed to be
             * different from all other automatically generated IDs
             *
             * cov[] is either the covariance matrix, an array of 4 doubles
             * or NULL
             */
            TrackPoint (double x, double y,  double area, const double cov[], int t);

            virtual ~TrackPoint();
            /*TrackPoint (double x, double y, int t, int ID)
             *
             * creates a track point with location x,y and frame t
             * the ID code is passed in by the creator and should
             * be different from any other ID code, or track match may break
             *
             * cov[] is either the covariance matrix, an array of 4 doubles
             * or NULL
             */
            TrackPoint (double x, double y,  double area, const double cov[], int t,int ID);

            /*TrackPoint (TrackPoint *pt)
             *
             * copies data in *pt to a new track point
             */
            TrackPoint (const TrackPoint *pt);
            /*returns the location as a cvPoint of doubles
             *
             */
            inline CvPoint2D32f getLocation() {
                return cvPoint2D32f(x,y);
            }
            //finds the location relative to the image origin point x0,y0
            //e.g. if x,y = 100, 80 and x0,y0 = 20,10, then returns 80,70
            inline CvPoint2D32f getLocation (double x0, double y0) {
                return cvPoint2D32f(x-x0, y-y0);
            }
            inline CvPoint2D32f getLocation (CvPoint pt) {
                return cvPoint2D32f(x-pt.x, y-pt.y);
            }
             inline CvPoint2D32f getLocation (CvPoint2D32f pt) {
                return cvPoint2D32f(x-pt.x, y-pt.y);
            }
            inline void setCovariance(const double *cov) {
                memcpy(&(this->cov), cov, 4*sizeof(double));
            }

            inline void getCovariance(double *cov) {
                memcpy(cov, &(this->cov), 4*sizeof(double));
            }

            inline void setArea (double area) {
                this->area = area;
            }

            inline double getArea () {
                return area;
            }

            /* changes the point's location
             *
             */
            void setLocation(double x, double y);


            /*getFrame, setFrame
             *
             * gets/sets frame number
             */
            inline int getFrame() {
		return frameNum;
            }

            void setFrame(int t);

            /*getID()
             *
             * returns ID number; no setID function by design
             */
            inline int getID() {
                return idNum;
            }

           

            /* int toDisk (FILE *f)
             * f is a pointer to a BINARY output file
             * writes the trackpoint to disk in the format
             * int frame, float x, float y
             *
             * nonzero return value indicates an error
             *
             * subclasses should override this function to output additional
             * information
             */
            virtual int toDisk(FILE *f);

            /* sizeOnDisk
             *
             * returns the number of bytes that will be written to file
             * when toDisk is called
             */
            virtual int sizeOnDisk();
            /*  static TrackPoint *fromDisk(FILE *f);
             *  reads int frame, float x, float y from disk (assumes f is
             *  open to a track point) and stuffs them into a new trackpoint
             *
             *  note that the ID# is unique but different from the ID# of
             *  the TP when it was saved
             *
             *  subclasses should override this function to match toDisk
             */
            static TrackPoint *fromDisk(FILE *f);

             /*virtual string saveDescription()
             *
             * provides a description of how the string is stored on disk
             */
            virtual std::string saveDescription();

            virtual inline std::string name() {
                return std::string("TrackPoint");
            }


            virtual void draw(IplImage *dst, bool active, int x0 = 0, int y0 = 0, int ptrad = 1);

            virtual void drawConnected(IplImage *dst, bool active, int x0, int y0, TrackPoint *pt);

            /* distance, distSquared
             * distance to another point; distSquared is faster
             *
             */
            inline const double distSquared (const TrackPoint &pt2) {
                return ((pt2.x - x) * (pt2.x - x) + (pt2.y - y) * (pt2.y - y));
            }

            inline const double distance (const TrackPoint &pt2){
                return sqrt (distSquared(pt2));
            }

            inline double distSquared (const TrackPoint *pt2) {
                return ((pt2->x - x) * (pt2->x - x) + (pt2->y - y) * (pt2->y - y));
            }

            inline double distance (const TrackPoint *pt2){
                return sqrt (distSquared(pt2));
            }

            //finds the angle of vertex a(this)c
            inline double vertexAngle (const TrackPoint *a, const TrackPoint *c) {
                return acos(((x - a->x)*(x - c->x) + (y - a->y)*(y - c->y))/(distance(a)*distance(c)));
            }
            inline double vertexAngle (const TrackPoint &a, const TrackPoint &c) {
                return acos(((x - a.x)*(x - c.x) + (y - a.y)*(y - c.y))/(distance(a)*distance(c)));
            }

            YAML::Emitter& toYAML (YAML::Emitter& out);

            virtual inline uchar getTypeCode() {
                return _id_code;
            }
            virtual void setMessageHandler (communicator *mh) {
                this->mh = mh;
            }
	 protected:

            int idNum; // unique id number used for lookup tables
            double x; // x location
	    double y; // y location
	    int frameNum; // frame this point came from (time point)
            double area; //contour area
            double cov[4]; //covariance matrix

            static int lastID; //used to generate new ID numbers by incrementing every time a point is created
            void Init(double x, double y,  double area, const double cov[], int t, int ID); //does the heavy lifting for the constructor
            inline void message(const char *msg, const verbosityLevelT verb) {
                if (messagesOn && mh != NULL) {
                    mh->message(name().c_str(), verb, msg);
                }
            }
            communicator *mh;


        private:
            TrackPoint(); // keep anyone from instantiating a track point with no location
            static const uchar _id_code = 0x01;
 };

 #endif


