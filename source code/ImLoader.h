/* 
 * File:   ImLoader.h
 * Author: Marc
 *
 * Created on October 23, 2009, 3:24 PM
 *
 * subclass of FrameLoader that implements loading frames from an
 * image (e.g. jpg or tiff) stack
 *
 * (C) Marc Gershow; licensed under the Creative Commons Attribution Share Alike 3.0 United States License.
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/us/ or send a letter to
 * Creative Commons, 171 Second Street, Suite 300, San Francisco, California, 94105, USA.
 */

#ifndef _IMLOADER_H
#define	_IMLOADER_H

#include "FrameLoader.h"

class ImLoader : public FrameLoader {
public:
    ImLoader();
    virtual ~ImLoader();

    /* set, get Padding
     *
     * padding describes the way numbers are appended on to the file stub
     *
     * say filename is "foo_" and extension is ".jpg"
     * if padding = 4, then frame 69 is found in
     * "foo_0069.jpg"
     *
     * if padding <= 0 (default behavior) then frame 69 would be
     * "foo_69.jpg"
     */
    virtual inline void setPadding (int padding) {
        this->padding = padding;
    }
    virtual inline int getPadding() {
        return padding;
    }
protected:
    int padding;
    virtual int loadWholeFrame (int frameNumber);
    virtual inline void message(const char *msg, const verbosityLevelT verb) {
        if (messagesOn && mh != NULL) {
            mh->message("Im Loader", verb, msg);
        }
    }
private:
      ImLoader(const ImLoader& orig);

};

#endif	/* _IMLOADER_H */

