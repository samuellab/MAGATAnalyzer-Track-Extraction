/* 
 * File:   ContourPoint.h
 * Author: Marc
 *
 * like a track point but also stores an angle
 * for the far far future, maybe we should make a new kind of point that's
 * like a super CV point, but less than a track point.  then trackpoint could
 * subclass this one
 * Created on November 3, 2009, 9:40 AM
 *
 * (C) Marc Gershow; licensed under the Creative Commons Attribution Share Alike 3.0 United States License.
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/us/ or send a letter to
 * Creative Commons, 171 Second Street, Suite 300, San Francisco, California, 94105, USA.
 */

#ifndef _CONTOURPOINT_H
#define	_CONTOURPOINT_H

#include "trackpoint.h"
#include "cv.h"
class ContourPoint : public TrackPoint {
public:
    ContourPoint(CvPoint pt, int ID);
    void setAngle (const ContourPoint prev, const ContourPoint next);
    void setAngle (const CvPoint prev, const CvPoint next);
    inline void setAngle (double angle) {
        theta = angle;
    }
    inline double getAngle () {
        return theta;
    }
    static inline bool sharpestIsFirst(const ContourPoint &a, const ContourPoint &b) {
        return (a.theta < b.theta);
    }
    inline CvPoint intLocation () {
        return cvPoint ((int)x,(int)y);
    }

private:
    double theta;

};

#endif	/* _CONTOURPOINT_H */

