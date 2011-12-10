/* 
 * File:   MaggotTrackExtractor.h
 * Author: Marc
 *
 * Created on November 6, 2009, 3:15 PM
 *
 * (C) Marc Gershow; licensed under the Creative Commons Attribution Share Alike 3.0 United States License.
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/us/ or send a letter to
 * Creative Commons, 171 Second Street, Suite 300, San Francisco, California, 94105, USA.
 */

#ifndef _MAGGOTTRACKEXTRACTOR_H
#define	_MAGGOTTRACKEXTRACTOR_H

#include "TrackExtractor.h"

class MaggotTrackExtractor : public TrackExtractor{
public:
    MaggotTrackExtractor();
    virtual int go();
    virtual void reanalyze(string inputname, string outputname);


    inline void setMaxAngle (double ma) {
        maxAngle = 3.14159253 / 2;
    }
    inline double getMaxAngle () {
        return maxAngle;
    }
    void fromYAML(const YAML::Node &node);
    
protected:
    virtual void createTrackBuilder();
    virtual void createPointExtractor();
    double maxAngle;
    virtual YAML::Emitter& yamlBody(YAML::Emitter& out) const;

    virtual inline string name() {
        return string("maggot track extractor");
    }
    virtual void headerinfo(std::ostream &os);
    bool reprocessed;
    string reprocessname;
};
YAML::Emitter& operator << (YAML::Emitter& out, const MaggotTrackExtractor *te);
YAML::Emitter& operator << (YAML::Emitter& out, const MaggotTrackExtractor &te);
void operator >> (const YAML::Node &node, MaggotTrackExtractor * &te);
void operator >> (const YAML::Node &node, MaggotTrackExtractor (&te));
#endif	/* _MAGGOTTRACKEXTRACTOR_H */

