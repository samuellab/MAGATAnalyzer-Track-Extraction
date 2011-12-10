/* 
 * File:   MaggotTrackBuilder.h
 * Author: Marc
 *
 * Created on November 6, 2009, 10:34 AM
 *
 * a simple subclass of TrackBuilder that converts tracks to maggot tracks
 * and analyzes them
 *
 * (C) Marc Gershow; licensed under the Creative Commons Attribution Share Alike 3.0 United States License.
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/us/ or send a letter to
 * Creative Commons, 171 Second Street, Suite 300, San Francisco, California, 94105, USA.
 */

#include "TrackBuilder.h"
#include "MaggotTrackPoint.h"


#ifndef _MAGGOTTRACKBUILDER_H
#define	_MAGGOTTRACKBUILDER_H

class MaggotTrackBuilder : public TrackBuilder {
public:
    MaggotTrackBuilder();
    MaggotTrackBuilder(int startFrame);
    
    /* void analyzeMaggotTracks(double targetArea, double maxAngle, double threshold);
     *
     * converts all tracks (active and finished) to maggotTracks and analyzes them according to
     * params minArea, maxArea, maxAngle. (see MaggotTrackPoint.h for details)
     */
    void analyzeMaggotTracks(double minArea, double MaxArea, double maxAngle, bool showExtraction = false);

    static MaggotTrackBuilder *fromDisk (FILE *f, TrackPoint *(*ptLoader) (FILE *f) = NULL );
protected:
    inline virtual uchar my_type_code () {
        return _id_code;
    }
    static const int minlentoshow = 500;
private:
     static const uchar _id_code = 0x02;

};

#endif	/* _MAGGOTTRACKBUILDER_H */

