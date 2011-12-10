/* 
 * File:   MidlineMaggotTrackPoint.h
 * Author: Marc
 *
 * Created on September 4, 2010, 12:57 PM
 *
 * (C) Marc Gershow; licensed under the Creative Commons Attribution Share Alike 3.0 United States License.
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/us/ or send a letter to
 * Creative Commons, 171 Second Street, Suite 300, San Francisco, California, 94105, USA.
 */

#ifndef _MIDLINEMAGGOTTRACKPOINT_H
#define	_MIDLINEMAGGOTTRACKPOINT_H
#include <string>
#include <vector>
#include "MaggotTrackPoint.h"
#include "cv.h"
using std::vector;
using std::string;

class MidlineMaggotTrackPoint : public MaggotTrackPoint {
public:
    /* MidlineMaggotTrackPoint(double x, double y, double area, double cov[],  int t, CvPoint offset, IplImage *src, int winSize);
     * MidlineMaggotTrackPoint(double x, double y, double area, double cov[],  int t, int ID, CvPoint offset, IplImage *src, int winSize);
     * MidlineMaggotTrackPoint(ImTrackPoint *pt) ;
     *
     * same basic constructors as an ImTrackPoint; if called on ImTrackPoint, we allocate new memory and copy the image over
     *
     * NB after construction, MTP does not have any additional information about the maggot (no contour info, etc.)
     * analyze() must be called to generate this information
     * (be sure to setAnalysisParams first)
     */
    MidlineMaggotTrackPoint(double x, double y, double area, double cov[],  int t, CvPoint offset, IplImage *src, int winSize);
    MidlineMaggotTrackPoint(double x, double y, double area, double cov[],  int t, int ID, CvPoint offset, IplImage *src, int winSize);
    MidlineMaggotTrackPoint(const ImTrackPoint *pt) ;
    MidlineMaggotTrackPoint(const MaggotTrackPoint *pt) ;
  
    static bool convertITPToMTP (vector<TrackPoint *> *v);

    
    virtual inline string name() {
        return string("MidlineMaggotTrackPoint");
    }
    /* virtual string saveDescription();
     * virtual int toDisk(FILE *f);
     * static TrackPoint *fromDisk(FILE *f);
     * virtual int sizeOnDisk();
     * virtual int makeMovieFrame (IplImage **dst);
     *
     * these functions override their counterparts in ImTrackPoint --
     * see description in ImTrackPoint.h
     */

    virtual string saveDescription();
    virtual int toDisk(FILE *f);
    static TrackPoint *fromDisk(FILE *f);
    virtual int sizeOnDisk();
    virtual ~MidlineMaggotTrackPoint();
    virtual inline uchar getTypeCode() {
        return _id_code;
    }
protected:
   
private:
    MidlineMaggotTrackPoint();
    MidlineMaggotTrackPoint(const MidlineMaggotTrackPoint& orig);

    static const uchar _id_code = 0x04;
};

#endif	/* _MIDLINEMAGGOTTRACKPOINT_H */

