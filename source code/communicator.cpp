/* 
 * File:   communicator.cpp
 * Author: Marc
 * 
 * Created on October 26, 2009, 3:43 PM
 *
 * (C) Marc Gershow; licensed under the Creative Commons Attribution Share Alike 3.0 United States License.
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/us/ or send a letter to
 * Creative Commons, 171 Second Street, Suite 300, San Francisco, California, 94105, USA.
 */

#include "communicator.h"
#include <fstream>
#include <iostream>
using namespace std;

communicator::communicator() {
    fileOut = false;
    vbl = verb_warning;
}
communicator::communicator(const char *filename) {
    out.open(filename);
    fileOut = true;
}

/*
communicator::~communicator() {

}
*/

 void communicator::message (const char *source, const verbosityLevelT verb, const int code, const char *msg) {
     if (verb > vbl) {
         return;
     }
     if (fileOut) {
         if (out.is_open()){
             out << verbosityLevel(verb) << source << " : " << code << " " << msg << "\n";
             out.flush();
         }
     } else {
         cout << verbosityLevel(verb) << source << " : " << code << " " << msg << "\n";
         cout.flush();
     }
 }
 void communicator::message (const char *source, const verbosityLevelT verb, const char *msg) {
     if (verb > vbl) {
         return;
     }
     if (fileOut) {
         if (out.is_open()){
             out << verbosityLevel(verb) << source << " : " << msg << "\n";
             out.flush();
         }
     } else {
         cout << verbosityLevel(verb) << source << " : " << msg << "\n";
         cout.flush();
     }
 }

 string communicator::verbosityLevel(verbosityLevelT verb) {
     switch(verb) {
         case verb_off:
             return string("Off : ");
             break;
         case verb_error:
             return string("Error : ");
             break;
         case verb_warning:
             return string("Warning : ");
             break;
         case verb_message:
             return string("Message : ");
             break;
         case verb_verbose:
             return string("Verbose Message : ");
             break;
         case verb_debug:
             return string("Debug Message : ");
             break;
     }
 }