/* 
 * File:   ImTrackPoint.h
 * Author: Marc
 *
 * Created on October 26, 2009, 10:00 AM
 *
 * A track point that also carries an image with it
 * extends TrackPoint
 *
 * (C) Marc Gershow; licensed under the Creative Commons Attribution Share Alike 3.0 United States License.
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/us/ or send a letter to
 * Creative Commons, 171 Second Street, Suite 300, San Francisco, California, 94105, USA.
 */

#ifndef _IMTRACKPOINT_H
#define	_IMTRACKPOINT_H
#include <stdio.h>
#include <string>
#include "cv.h"
#include "cvtypes.h"
#include "cxcore.h"
#include "trackpoint.h"

using std::string;

class ImTrackPoint : public TrackPoint{
public:

    /* constructors
     *
     * we can make an ITP with all the parameters to make a track point + an image source (may be null), a point offset telling you
     * where the origin of the image source is relative to the coordinate system, and a window size telling the size of the image to
     * extract and store in this track point
     *
     * we can also make an ITP from a track point, and the source, an image to store, and an image origin
     * in this case, the source image is copied in its entirety, and winSize is set appropriately
     *
     * or from another ITP, in which case the point is cloned, with the image from the original point being copied into new memory for
     * this point
     *
     * special values:  src == NULL; no image copied or stored |  winSize < 0; no image copied or stored
     */
    ImTrackPoint(double x, double y, double area, double cov[],  int t, CvPoint offset, IplImage *src, int winSize);
    ImTrackPoint(double x, double y, double area,  double cov[],  int t, int ID, CvPoint offset, IplImage *src, int winSize);
    ImTrackPoint(const TrackPoint *pt, const IplImage* src, CvPoint imOrigin) ;
    ImTrackPoint(const ImTrackPoint *pt);

    /* int copyImageOut
     *
     * copies image stored in the ITP to dst, allocating new memory if needed 
     * returns 0 on success;  -1 if no image stored in the ITP
     */
    int copyImageOut (IplImage **dst);

    /* virtual int makeMovieFrame (IplImage **dst);
     *
     * creates a movie display image in dst, allocating memory if needed
     * returns 0 on success;
     * here this is a clone of copyImageOut, but subclasses may include additional
     * features
     */
    virtual int makeMovieFrame (IplImage **dst);


    /* toDisk:
     * saves point to disk in binary format;  description of this format can be found
     * by calling saveDescription
     * returns 0 on success
     * the number of bytes used to store the point is given by sizeOnDisk
     *
     */
    virtual int toDisk (FILE *f);
    virtual std::string saveDescription();
    virtual int sizeOnDisk();

    /* fromDisk:
     * static method
     * (does not require you to instantiate an ImTrackPoint to call and does
     *  not have member variables or methods)
     *
     * creates memory for a new ImTrackPoint, loads that point from disk
     * assuming that f is positioned at the beginning of a track point, and
     * returns a pointer to the created track point, which resides on the heap
     * 
     * on failure returns NULL
     */
    
    static TrackPoint *fromDisk(FILE *f);
    virtual ~ImTrackPoint();
    virtual inline std::string name() {
            return std::string("ImTrackPoint");
    }
    virtual inline uchar getTypeCode() {
                return _id_code;
    }
protected:
    /*
     * im -- IPL image associated with this point
     * (i.e. a picture of the animal at that point)
     */
    IplImage *im;
    /*
     * imOrigin -- the position of the origin (upper left) in the coordinate system
     * used to define the point location
     */
    CvPoint imOrigin;
    void setImage (CvPoint offset, IplImage *src, int winSize);


private:
    ImTrackPoint();
    ImTrackPoint(const ImTrackPoint& orig);
    static const int _id_code = 0x02;
    
};

#endif	/* _IMTRACKPOINT_H */

