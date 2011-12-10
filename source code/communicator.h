/* 
 * File:   communicator.h
 * Author: Marc
 *
 * Created on October 26, 2009, 3:43 PM
 *
 * this is a utility class that lets you selective print out messages according to their importance
 * printout can either be to a file (instantiate communicator with a filename) or to cout (no arg constructor)
 *
 * (C) Marc Gershow; licensed under the Creative Commons Attribution Share Alike 3.0 United States License.
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/us/ or send a letter to
 * Creative Commons, 171 Second Street, Suite 300, San Francisco, California, 94105, USA.
 */

#ifndef _COMMUNICATOR_H
#define	_COMMUNICATOR_H

#include <iostream>
#include <fstream>
#include <cstring>
using namespace std;
 enum verbosityLevelT {verb_off, verb_error, verb_warning, verb_message, verb_verbose, verb_debug};

class communicator {
public:
    communicator();
    communicator(const char *filename);

    //virtual ~communicator();
    void message (const char *source, const verbosityLevelT verb, const int code, const char *msg);
    void message (const char *source, const verbosityLevelT verb, const char *msg);
    inline void setVerbosity (verbosityLevelT vbl) {
        this->vbl = vbl;
    }
    inline verbosityLevelT getVerbosity() {
        return vbl;
    }
private:
    bool fileOut;
    ofstream out;
    void init (char *filename);
    verbosityLevelT vbl;
    
    communicator(const communicator& orig);
    static string verbosityLevel(verbosityLevelT verb);

};

#endif	/* _COMMUNICATOR_H */

