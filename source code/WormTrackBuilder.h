/* 
 * File:   WormTrackBuilder.h
 * Author: Marc
 *
 * Created on November 18, 2009, 3:39 PM
 */

#ifndef _WORMTRACKBUILDER_H
#define	_WORMTRACKBUILDER_H

#include "trackbuilder.h"

class WormTrackBuilder : public TrackBuilder{
public:
    virtual void handleCollisions();
protected:
    virtual void killShortCollidingTracks();
    inline virtual uchar my_type_code () {
        return _id_code;
    }
private:
     static const uchar _id_code = 0x03;
};

#endif	/* _WORMTRACKBUILDER_H */

