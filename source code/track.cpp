/*
 * track.cpp
 *
 * implements the class Track
 *
 * (C) Marc Gershow; licensed under the Creative Commons Attribution Share Alike 3.0 United States License.
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/us/ or send a letter to
 * Creative Commons, 171 Second Street, Suite 300, San Francisco, California, 94105, USA.
 */
  #include <vector>
#include <cassert>
#include <iostream>
#include <algorithm>
 #include "track.h"
 #include "trackpoint.h"
#include "ImTrackPoint.h"

#include "cv.h"
#include "CvUtilsPlusPlus.h"
using namespace std;

 static TrackPoint *nearestPointToList (const TrackPoint *pt, const vector <TrackPoint *> &list);

 /*
 static int nearestPointToList (const TrackPoint *pt, const vector <TrackPoint *> &list) {
	 if (list.size() <= 0)
		 return -1;
	 
	 double minDistSq = pt->distSquared(*(list.at(0)));
	 double minInd = 0;
	 double ds;
	 for (int j = 1; j < list.size(); j++) {
		 if ((ds = pt->distSquared(*(list.at(j)))) < minDistSq) {
			 minDistSq = ds;
			 minInd = j;
		 }
	 }
	 return minInd;
 }
  * */

 static TrackPoint *nearestPointToList (TrackPoint *pt, vector <TrackPoint *> &list) {

	 if (list.empty())
		 return NULL;

         TrackPoint *rv;
         vector<TrackPoint *>::iterator it;
         rv = ((list.at(0)));
	 double minDistSq = pt->distSquared(rv);
         double ds;
         for (it = list.begin(); it != list.end(); ++it) {
            if ((ds = pt->distSquared(*it)) < minDistSq) {
                minDistSq = ds;
                rv = *it;
            }
         }
         return rv;
 }

 Track::Track() {
	 active = true;
         deletePointsOnDestruction = true;
         mh = NULL;
 }
 Track::~Track() {
     if (deletePointsOnDestruction) {
         deleteTrackPoints();
     }
 }

 /*Note that we store the address of point, not the point
  *itself;
  *
  *
  */
 void Track::extendTrack (TrackPoint *pt) {
	 points.push_back(pt);
 }
 
 void Track::endTrack () {
	 active = false;
 };

  void Track::deleteTrackPoints() {
      //cout << "deleting track points\n";
      while (!points.empty()) {
          //cout << "deleting " << points.back()->name() << "\n";
          delete ((points.back()));
          points.pop_back();
      }
  }
	
 TrackPoint *Track::nearestPointToEnd (std::vector<TrackPoint *> &pointList) {
	 return nearestPointToList (points.back(), pointList);	
		 
 }

 string Track::description() {
    if (points.empty()) {
        return string ("empty track\n");
    } else {
        std::stringstream str; str << ("track of ");
        str << points.size() << " " << points.front()->name();
        if (points.size() > 1) {
            str << "s";
        }
        str << " Median area = " << medianArea() << " Mean area = " << meanArea();
        return str.str();
    }
 }
 /*
 const int Track::nearestPointToEnd (const std::vector<TrackPoint *> &pointList, double maxDist) {
	 int j = nearestPointToList (points.back(), pointList);
         if (j < 0)
             return j;
         if (distanceFromEnd(*(pointList.at(j))) < maxDist)
             return j;
         else
             return -1;         
 }

 */
 
TrackPoint *Track::nearestPointInTrack (TrackPoint *pt) {
	 return nearestPointToList (pt, points);
 }
 
double Track::distanceFromEnd (TrackPoint *pt) {
    assert(pt != NULL);
    assert(!points.empty());
    return pt->distance(*(points.back()));
 }
	
void Track::draw(IplImage* dst, int x0, int y0) {
    vector<TrackPoint *>::iterator it;
    if (points.empty()) {
        return;
    }
    for (it = points.begin(); it != points.end()-1; ++it) {
        //(*it)->draw(dst, active, x0, y0,1);
        (*it)->drawConnected(dst, active, x0, y0, *(it+1));
    }
    if (active) {
        (*it)->draw(dst, active, x0, y0,5);
    }
}
void Track::print() {
    vector<TrackPoint *>::iterator it;
    if (active){
        cout << "active track ";
    } else {
        cout << "finished track ";
    }
    cout << points.size() << " points ID Sequence: ";
    for (it = points.begin(); it != points.end(); ++it) {
        cout << (*it)->getID() << ",";
    }
    cout << "\n";
}
/* int toDisk (FILE *f)
 * writes track to BINARY file f in the following fashion
 * int npts, (write track point) * npts
 *
 * nonzero return value indicates error
 */
/*
int Track::toDisk(FILE *f, bool writeIm) {
    if (f == NULL) {
        return -1;
    }
    int tempi = (int) points.size();
    if (fwrite(&tempi, sizeof(int), 1, f) != 1) {
        return -1;
    }
    vector<TrackPoint *>::iterator it;
    for (it = points.begin(); it != points.end(); ++it) {
        ImTrackPoint *itp;
        if (writeIm && (itp = dynamic_cast<ImTrackPoint *> (*it)) != NULL) {
            if (itp->toDisk(f) != 0) {
                return -1;
            }
        } else {
            if ((*it)->toDisk(f) != 0) {
                return -1;
            }
        }
    }
    return 0;
}
*/
int Track::toDisk(FILE *f) {
    if (f == NULL) {
        return -1;
    }
    if (sizeof(int) < 4) {
        long size = sizeOnDisk();
        if (fwrite(&size, sizeof(long), 1, f) != 1) {
            message ("failed to write number of bytes in track", verb_error);
            return -1;
        }
    } else {
        int size = sizeOnDisk();
        if (fwrite(&size, sizeof(int), 1, f) != 1) {
            message ("failed to write number of bytes in track", verb_error);
            return -1;
        }
    }
    int tempi = (int) points.size();
    if (fwrite(&tempi, sizeof(int), 1, f) != 1) {
        message ("failed to write number of points in track", verb_error);
        return -1;
    }
    vector<TrackPoint *>::iterator it;
    for (it = points.begin(); it != points.end(); ++it) {
       if ((*it)->toDisk(f) != 0) {
                message ("failed to write a point in track", verb_error);
                return -1;
       }
    }
    return 0;
}

string Track::saveDescription() {
    string s;
    if (sizeof(int) < 4) {
        s = string ("(long ");
    } else {
        s = string ("(int ");
    }
    s.append (" number of bytes in track following this number) (int npts) npts x ");
    if (points.front() == NULL) {
        s.append ("\ntrack point type unknown -- information incomplete)\n");
        return s;
    }
    s.append (points.front()->name());
    s.append ("\n"); s.append(points.front()->name()); s.append(":\n");
    s.append(points.front()->saveDescription());
    return s;
}

long Track::sizeOnDisk() {
    long size = sizeof(int);
    vector<TrackPoint *>::iterator it;
    for (it = points.begin(); it != points.end(); ++it) {
        size += (*it)->sizeOnDisk();
    }
    return size;
}

Track *Track::fromDisk(FILE *f, TrackPoint *(*ptLoader) (FILE *f)) {
    if (f == NULL) return NULL;
    int npts;
    if (sizeof(int) < 4) {
        long size;
        if (fread(&size, sizeof(long), 1, f) != 1) {
            return NULL;
        }
    } else {
        int size;
        if (fread(&size, sizeof(int), 1, f) != 1) {
            return NULL;
        }
    }
    if (fread(&npts, sizeof(int), 1, f) != 1)
        return NULL;
    Track *t = new Track();
    for (int j = 0; j < npts; ++j) {
        TrackPoint *pt = ptLoader(f);
        if (pt == NULL) {
            delete t;
            return NULL;
        } else {
            t->extendTrack(pt);
        }
    }
    return t;
}

void Track::playTrackMovie(const char *windowName, int delayms) {
    IplImage *display = NULL;
    cvNamedWindow(windowName,0);
    ImTrackPoint *itp;
    for (vector<TrackPoint *>::iterator it = points.begin(); it != points.end(); ++it) {
        itp = dynamic_cast<ImTrackPoint *> (*it);
        if (itp != NULL) {
            itp->makeMovieFrame(&display);
            cvShowImage(windowName, display);
            if (cvWaitKey(delayms) > 0) {
                cout << "interrupted\n";
                break;
            }
        }
    }
    cvDestroyWindow(windowName);

}

double Track::meanArea() {
    double areasum = 0;
    vector<TrackPoint *>::iterator it;
    for (it = points.begin(); it != points.end(); ++it) {
        areasum += (*it)->getArea();
    }
    return areasum / points.size();
}
double Track::medianArea() {
    vector<double> areas;
    vector<TrackPoint *>::iterator it;
    if (points.empty()) {
        return 0;
    }
    for (it = points.begin(); it != points.end(); ++it) {
        areas.push_back((*it)->getArea());
    }
    sort(areas.begin(), areas.end());
    return (areas.at(areas.size()/2));
}

short int Track::getTypeCode() {

    short int code;
    if (~points.empty()) {
        code = points.front()->getTypeCode();
    } else {
        code = 0;
    }
    code = (code << 8) + _id_code;
    return code;
}

void Track::setMessageHandler(communicator* mh) {
    this->mh = mh;
    for (vector<TrackPoint *>::iterator it = points.begin(); it != points.end(); ++it) {
        (*it)->setMessageHandler(mh);
    }
}