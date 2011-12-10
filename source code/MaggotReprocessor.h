/* 
 * File:   MaggotReprocessor.h
 * Author: Marc
 *
 * Created on October 6, 2010, 3:28 PM
 * (C) Marc Gershow; licensed under the Creative Commons Attribution Share Alike 3.0 United States License.
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/us/ or send a letter to
 * Creative Commons, 171 Second Street, Suite 300, San Francisco, California, 94105, USA.
 */

#ifndef MAGGOTREPROCESSOR_H
#define	MAGGOTREPROCESSOR_H

#include <string>
#include <map>
#include <vector>
#include "communicator.h"
class MaggotReprocessor {
public:
    MaggotReprocessor();
    virtual ~MaggotReprocessor();
    int processOne(bool showExtraction, verbosityLevelT verbosity = verb_warning);
    void processAll(bool showExtraction, verbosityLevelT verbosity = verb_warning);
    inline void setAngles (int minArea, int maxArea) {
        this->minArea = minArea;
        this->maxArea = maxArea;
    }
    void addFiles (std::string existingFile, std::string outputFile);
    void addFiles (std::string existingFile);
    
    static void reprocessFile (std::string existingFile, std::string outputName, int minArea, int maxArea, double maxMaggotContourAngle, verbosityLevelT verbosity = verb_warning, bool showExtraction = false);
    

protected:
    std::vector<std::pair<std::string, std::string> > filenames;
    double maxMaggotContourAngle;
    int minArea;
    int maxArea;
    void init();
private:
    MaggotReprocessor(const MaggotReprocessor& orig);

};

#endif	/* MAGGOTREPROCESSOR_H */

