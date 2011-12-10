/* 
 * File:   StackLoader.h
 * Author: Marc
 *
 * Created on November 2, 2010, 2:34 PM
 * (C) Marc Gershow; licensed under the Creative Commons Attribution Share Alike 3.0 United States License.
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/us/ or send a letter to
 * Creative Commons, 171 Second Street, Suite 300, San Francisco, California, 94105, USA.
 */

#ifndef STACKLOADER_H
#define	STACKLOADER_H

#include <cv.h>
#include "FrameLoader.h"
#include "StackReader.h"


class StackLoader : public FrameLoader{
public:
    StackLoader();
    void getBackground (int frameNumber, IplImage **dst, int frameRange);
    virtual ~StackLoader();
    virtual void writeMetaData(string binfileoutputname);
    virtual int getStackLength();
    virtual CvRect getImageRectangle();

protected:
    virtual inline void message(const char *msg, const verbosityLevelT verb) {
        if (messagesOn && mh != NULL) {
            mh->message("Stack Loader", verb, msg);
        }
    }
    virtual int loadWholeFrame(int frameNumber);
    virtual void checkAndLoadSR();
    
    StackReader *sr;
    


private:
    StackLoader(const StackLoader& orig);
    void init();

};

#endif	/* STACKLOADER_H */

