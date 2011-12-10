/* 
 * File:   ContourPoint.cpp
 * Author: Marc
 * 
 * Created on November 3, 2009, 9:40 AM
 *
 * like a track point but also stores an angle
 * for the far far future, maybe we should make a new kind of point that's
 * like a super CV point, but less than a track point.  then trackpoint could
 * subclass this one
 *
 * but CV 2.0 probably has this anyway
 * 
 * (C) Marc Gershow; licensed under the Creative Commons Attribution Share Alike 3.0 United States License.
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/us/ or send a letter to
 * Creative Commons, 171 Second Street, Suite 300, San Francisco, California, 94105, USA.
 */

#include "ContourPoint.h"

ContourPoint::ContourPoint(CvPoint pt, int ID) : TrackPoint(pt.x, pt.y, -1, NULL, -1, ID) {
    theta = NAN;
}
void ContourPoint::setAngle (const CvPoint prev, const CvPoint next) {
    theta = vertexAngle(ContourPoint(prev,0), ContourPoint(next,0));
}

void ContourPoint::setAngle (const ContourPoint prev, const ContourPoint next) {
     theta = vertexAngle(prev, next);
}

