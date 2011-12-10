/* trackbuilder.cpp
 *
 * implements the TrackBuilder class
 *
 * to build a track, we have the following sequences of activities
 * (1) tell trackbuilder you're starting a new frame
        this will clear memory and information used to build the previous
	frame
 * (2) tell trackbuilder information about the current frame
 *   for the base class, this is just a list of points, but
 *   for subclasses, could also include an image, etc.
 * (3) tell trackbuilder to stitch tracks together using extendTracks()
 *   we declare this method to be virtual, so that subclasses may
 *   use different algorithms
 *
 * (C) Marc Gershow; licensed under the Creative Commons Attribution Share Alike 3.0 United States License.
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/us/ or send a letter to
 * Creative Commons, 171 Second Street, Suite 300, San Francisco, California, 94105, USA.
 */
#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <algorithm>
#include <ctime>
#include "track.h"
#include "trackpoint.h"
#include "trackbuilder.h"
#include "trackmatch.h"
#include "communicator.h"
#include "PointExtractor.h"
#include "cv.h"
#include "CvUtilsPlusPlus.h"
#include "MaggotTrackBuilder.h"
#include "tictoc.h"
#include "MidlineMaggotTrackPoint.h"
using namespace std;

TrackBuilder::~TrackBuilder() {
// we don't destroy any of the created tracks here, on the assumption that we may have
// already outputted them somewhere else/may not be uniquely stored here/etc
    nextFrame(0); //frees the storage allocated in matches
}
//this destroys all the created tracks
void TrackBuilder::deleteAllTracks() {
  //  cout << "deleteAllTracks called\n";
    vector<Track *>::iterator it;
    for (it = activetracks.begin(); it != activetracks.end(); ++it) {
        delete (*it);
    }
    activetracks.clear();
    for (it = finishedtracks.begin(); it != finishedtracks.end(); ++it) {
        delete (*it);
    }
    finishedtracks.clear();
 //   cout << "deleteAllTracks finished\n";
}
TrackBuilder::TrackBuilder() {
    Init(0);
}
TrackBuilder::TrackBuilder(int startFrame) {
    Init(startFrame);
}
void TrackBuilder::Init (int startFrame) {
    frameNumber = startFrame;
    pe = NULL;
    maxDist = 1E6;
    mh = NULL;
    nextFrame(startFrame);
}

static bool trackEndsEarlier (Track *t1, Track *t2);
static bool trackEndsEarlier (Track *t1, Track *t2) {
    return t1->lastFrame() < t2->lastFrame();
}

Track *TrackBuilder::createTrack(TrackPoint *pt) {
    _TICTOC_TIC_FUNC;
    Track *tr;
    tr = new Track();
    tr->extendTrack(pt);
    activetracks.push_back(tr);
    message("created track", verb_debug);
    _TICTOC_TOC_FUNC;
    return tr;
}

void TrackBuilder::nextFrame() {
    nextFrame(frameNumber + 1);
}

void TrackBuilder::nextFrame(int frameNum) {
    _TICTOC_TIC_FUNC;
    map<int, TrackMatch *>::iterator it;
    frameNumber = frameNum;
    newPoints.clear();
    for (it = matches.begin(); it != matches.end(); ++it) {
        delete it->second;
    }
    matches.clear();
//    oneToOne = false;
    if (messagesOn) {
        stringstream s; s << "started new frame " << frameNum << " " << activetracks.size() << " active tracks " << finishedtracks.size() << " finished tracks";
        message(s.str().c_str(), verb_message);
    }
    _TICTOC_TOC_FUNC;

}

int TrackBuilder::completeStep() {
    _TICTOC_TIC_FUNC;
    if (pe == NULL) {
        message("Complete Step called, but point extractor is NULL, so I can't get points\n", verb_error);
        return -1;
    }
    int rval;
    if ((rval = pe->nextFrame()) != 0) {
        _TICTOC_TOC_FUNC;
        return rval;
    }
    nextFrame(pe->getFrameNumber());
    vector<TrackPoint *> *v = pe->getPoints();
    addPoints(v);
    delete v;
    extendTracks();
    _TICTOC_TOC_FUNC;
    return 0;
}

void TrackBuilder::extractionImage(IplImage** dst, const char* screenMessage) {
    _TICTOC_TIC_FUNC;
    static clock_t time = clock();
    message ("extractionImage called", verb_debug);
    IplImage *tmp = NULL;
    pe->copyBSImagOut(&tmp);
    allocateImage(dst, cvGetSize(tmp), IPL_DEPTH_8U, 3);
    cvConvertImage (tmp, *dst, 0);
    drawTracks(*dst, -pe->getAnalysisRect().x, -pe->getAnalysisRect().y);
    CvFont font;
    cvInitFont(&font, CV_FONT_HERSHEY_DUPLEX, 1.0, 1.0, 0, 2, 8);
    stringstream s;
    int dt = clock() - time;
    s << "Frame_" << frameNumber << "_";
    s << dt << "_ms";
    time = clock();
    string msg; s >> msg;
    CvSize sz; int baseline;
    cvGetTextSize(msg.c_str(), &font, &sz, &baseline);
    CvPoint p = cvPoint (5, (*dst)->height - sz.height - baseline);
    s.str(""); s << "attempting to write text to screen ";
    s << msg << " at " << p.x << "," << p.y;

    message (s.str().c_str(), verb_verbose);
    if (screenMessage == NULL) {
        cvPutText(*dst, msg.c_str(), p, &font, CV_RGB(255,0,0));
    } else {
        cvPutText(*dst, screenMessage, p, &font, CV_RGB(255,0,0));
    }
    cvReleaseImage(&tmp);
    _TICTOC_TOC_FUNC;
}

void TrackBuilder::addPoint(double x, double y) {
    TrackPoint *pt = new TrackPoint(x,y,0,NULL, frameNumber);
    addPoint(pt);
}

void TrackBuilder::addPoint(TrackPoint *pt) {
    _TICTOC_TIC_FUNC
    assert (pt != NULL);
    pt->setFrame(frameNumber);
    newPoints.push_back(pt);
    matches.insert (pair<int, TrackMatch *>(pt->getID(), new TrackMatch(pt)));
    if (messagesOn) {
        stringstream s; s << "added point at " << pt->getLocation().x <<" , "<<pt->getLocation().y ;
        message (s.str().c_str(), verb_debug);
    }
    _TICTOC_TOC_FUNC;
}
 void TrackBuilder::addPoints (vector<TrackPoint *> *ptvec) {
     _TICTOC_TIC_FUNC;
     stringstream s;
     if (messagesOn) {
          s << "adding " << ptvec->size() << " points to track builder";
         message (s.str().c_str(), verb_message);
     }
     vector<TrackPoint *>::iterator it;
     if (messagesOn) {
         s.str(""); s << "adding ptIDs ";
     }
     for (it = ptvec->begin(); it != ptvec->end(); ++it) {
         addPoint(*it);
         if (messagesOn) {
             s << (*it)->getID()<< " ";
         }
     }
     if (messagesOn) {
         message (s.str().c_str(), verb_verbose);
     }
     _TICTOC_TOC_FUNC;
 }

void TrackBuilder::extendTracks() {
    _TICTOC_TIC_FUNC;
    stringstream s;
    //match every track to the closest track point, regardless of distance
    matchTracksToClosest();

    if (messagesOn) {
        s << "before endTooFar " << activetracks.size() << " active " << finishedtracks.size() << " finished ";
        message (s.str().c_str(), verb_verbose);
    }
    //end any tracks that are closest to a point farther than maxDist away
    endTooFar();

     if (messagesOn) {
        s.str(""); s << ("after endTooFar");
        s << activetracks.size() << " active " << finishedtracks.size() << " finished ";
        message (s.str().c_str(), verb_verbose);
     }
    //in this implementation, try to resolve by going to a nearby point,
    //then end all colliding tracks if unsuccessful 
    handleCollisions();

    if (messagesOn) {
        s.str(""); s << ("after handleCollisions");
        s << activetracks.size() << " active " << finishedtracks.size() << " finished ";
        message (s.str().c_str(), verb_verbose);
    }
    
    extendExactMatches();

    if (messagesOn) {
        s.str(""); s << ("after extendExactMatches");
        s << activetracks.size() << " active " << finishedtracks.size() << " finished ";
        message (s.str().c_str(), verb_verbose);
    }

    //start new tracks from empty points
    startNewTracks();

    if (messagesOn) {
        s.str(""); s << "after startNewTracks";
        s << activetracks.size() << " active " << finishedtracks.size() << " finished ";
        message (s.str().c_str(), verb_verbose);
    }
    //move inactive tracks to the finished tracks vector
    pruneInactive();

    if (messagesOn) {
        s.str(""); s << ("after pruneInactive");
        s << activetracks.size() << " active " << finishedtracks.size() << " finished ";
        message (s.str().c_str(), verb_verbose);
    }
    _TICTOC_TOC_FUNC;
}


void TrackBuilder::pruneInactive() {
    _TICTOC_TIC_FUNC;
    vector<Track *>::iterator it;
    for (it = activetracks.begin(); it != activetracks.end(); ) {
        if (!(*it)->isActive()) {
            finishedtracks.push_back(*it);
            it = activetracks.erase(it); //erase automatically returns an iterator to the next valid entry
        } else {
            ++it;
        }
    }
    _TICTOC_TOC_FUNC;
}

/*Iterates through all active tracks and adds each one to the next point that is closest
 *to the end of the track; regardless of maxDist
 */
void TrackBuilder::matchTracksToClosest() {
    _TICTOC_TIC_FUNC;
    map<int, TrackMatch *>::iterator matchIterator;
    vector<Track *>::iterator trackIterator;
    TrackPoint *pt;
    if (messagesOn) {
        stringstream s;
        s << "trying to match " << matches.size() << " points to " << activetracks.size() << "active tracks";
        message (s.str().c_str(), verb_message);
    }
    
    for (trackIterator = activetracks.begin(); trackIterator != activetracks.end(); ++trackIterator){
        pt = (*trackIterator)->nearestPointToEnd(newPoints);
        if (pt == NULL) {
            message ("nearestPointToEnd returned NULL.  Did you add points to track builder?", verb_error);
            continue;
        }
        matchIterator = matches.find(pt->getID());
        if (matchIterator == matches.end()) {
            message ("Point in newPoints is not contained in matches map", verb_warning);
            matchIterator = matches.insert(matchIterator,  pair<int, TrackMatch *>(pt->getID(), new TrackMatch (pt)));
        }
        matchIterator->second->addTrack(*trackIterator);
    }
    _TICTOC_TOC_FUNC;
}

/* endTooFar()
* iterates through all track matches, and sets to inactive any
* tracks that are farther than maxDist from the nearest new point
*/
void TrackBuilder::endTooFar() {
    _TICTOC_TIC_FUNC;
    map<int, TrackMatch*>::iterator it;
    for (it = matches.begin(); it != matches.end(); ++it) {
        it->second->cutByDistance(maxDist);
    }
    _TICTOC_TOC_FUNC;
}
void TrackBuilder::extendExactMatches() {
    _TICTOC_TIC_FUNC;
    map<int, TrackMatch*>::iterator it;
    for (it = matches.begin(); it != matches.end(); ++it) {
        if (it->second->numMatches() == 1) {
           // it->second->cutByDistance(maxDist); WRONG!!!!
            it->second->extendTrack();
        }
    }
    _TICTOC_TOC_FUNC;
}
void TrackBuilder::startNewTracks() {
    _TICTOC_TIC_FUNC;
    map<int, TrackMatch*>::iterator it;
    for (it = matches.begin(); it != matches.end(); ++it) {
        if (it->second->numMatches() == 0) {
            createTrack(it->second->getPointPtr());
        }
    }
    _TICTOC_TOC_FUNC;
    
}
/*handles track collisions by attempting to match colliding tracks to
 * nearby empty points,
 * then by attempting to split the colliding point by rethresholding
 * then by ending collisions
 *
 * feel free to subclass and then overwrite with a better algorithm
 */
void TrackBuilder::handleCollisions() {
    _TICTOC_TIC_FUNC;
    matchCollisionsToEmptyPoints();
    splitCollisionPoints();
    endCollidingTracks();
    _TICTOC_TOC_FUNC;
}




/* splitCollisionPoints
 * iterates through collision matches, attempts to rethreshold
 * to get correct number of matching points
 * extends or ends tracks depending on result of this thresholding
 * either way, the offending track match is deleted
 */
void TrackBuilder::splitCollisionPoints() {
   map<int, TrackMatch*>::iterator it;
   vector<int> todelete;
   if (pe == NULL) return;
   _TICTOC_TIC_FUNC;
   for (it = matches.begin(); it != matches.end(); ++it ) {
        if (it->second->numMatches() > 1) {
            splitCollisionPoint(it->second);
            todelete.push_back(it->first); //store the keys of track matches that are collisions and should be deleted
        } 
    }
   for (vector<int>::iterator vit = todelete.begin(); vit != todelete.end(); ++vit) {
       matches.erase(*vit); //delete by key
   }
/*Map has the important property that inserting a new element into a map does not invalidate iterators that point to existing
 * elements. Erasing an element from a map also does not invalidate any iterators,
 * except, of course, for iterators that actually point to the element that is being erased.
 */
   _TICTOC_TOC_FUNC;
}

void TrackBuilder::splitCollisionPoint(TrackMatch *m) {
     message ("split collision called\n", verb_warning);
    verbosityLevelT vbl;
  //  if (mh != NULL){
   //     vbl = mh->getVerbosity();
   //     mh->setVerbosity(verb_debug);
  //  }
    if (pe == NULL) return; //nothing we can do if we don't have access to the source image
    if (!pe->supportsCollisionSplits()) return; //don't do anything if the pointextractor doesn't support collision splitting
     _TICTOC_TIC_FUNC;
    vector<TrackPoint *> *v = pe->splitPoint(m->getPointPtr(), m->numMatches(), (int) m->meanAreasIn());
    //clever -- use a new track builder to extend these subset of points (may only be one in which case it gets ended)
    TrackBuilder tbtemp(frameNumber);
    tbtemp.setMessageHandler(mh);
    tbtemp.setConnectionDistance(this->maxDist);
    tbtemp.activetracks = *(m->getTracksPtr());
    tbtemp.addPoints(v);
    delete (v);
    tbtemp.extendTracks(); //this does not end us in an infinite recursive loop, because pe = NULL for new TB
  //  if (mh != NULL){
  //      mh->setVerbosity(vbl);
  //  }
    _TICTOC_TOC_FUNC;
}


void TrackBuilder::endCollidingTracks() {
    _TICTOC_TIC_FUNC;
    map<int, TrackMatch*>::iterator it;
    for (it = matches.begin(); it != matches.end(); ++it) {
        if (it->second->numMatches() > 1) {
            it->second->endTracks();
        }
    }
    _TICTOC_TOC_FUNC;

}


/*Iterates through all track-point matches and looks
 *for tracks that collide to the same point
 *
 *if there is another point within maxDist of either colliding track
 *and that point has no matching tracks, then we match the closest colliding
 *track with that point instead
 */
void TrackBuilder::matchCollisionsToEmptyPoints() {
     _TICTOC_TIC_FUNC;
    map<int, TrackMatch *>::iterator it1, it2;
    TrackMatch *tm2;
    double dt, d;
    //iterate through all track matches, looking for collisions
    for (it1 = matches.begin(); it1 != matches.end(); ++it1) {
        if (it1->second->isCollision()) {
            tm2 = NULL;
            d = maxDist;
            //iterate looking for empty points, and find the closest empty
            //point that is within maxDist of the end of one of the tracks
            for (it2 = matches.begin(); it2 != matches.end(); ++it2) {
                if (it1 == it2 || it2->second->numMatches() > 0) {
                    continue;
                }
                if ((dt = it1->second->minDistFromTracksToPt(it2->second->getPointPtr())) < d) {
                    d = dt;
                    tm2 = it2->second;
                }
            }
            //if we found a matching empty point, then we shift the closest track to it
            if (tm2 != NULL) {
                Track *tr = it1->second->popClosestTrack(tm2->getPointPtr());
                tm2->addTrack(tr);
            }


        }
    }
    _TICTOC_TOC_FUNC;
}

void TrackBuilder::drawTracks(IplImage *dst, int x0, int y0) {
    vector<Track *>::iterator it;
    for (it = finishedtracks.begin(); it != finishedtracks.end(); ++it) {
        (*it)->draw(dst, x0, y0);
    }
    for (it = activetracks.begin(); it != activetracks.end(); ++it) {
        (*it)->draw(dst, x0, y0);
    }
}

void TrackBuilder::printTracks(bool active) {
    vector<Track *>::iterator it;
    if (active) {
         for (it = activetracks.begin(); it != activetracks.end(); ++it) {
            (*it)->print();
        }
    } else {
        for (it = finishedtracks.begin(); it != finishedtracks.end(); ++it) {
            (*it)->print();
        }
    }
}

void TrackBuilder::endAllTracks() {
    for (vector<Track *>::iterator it = activetracks.begin(); it != activetracks.end(); ++it) {
        (*it)->endTrack();
    }
    pruneInactive();
}

static inline int writeFloat(FILE *f, double x) {
    float g = x;
    return ((fwrite(&g, sizeof(float), 1, f) == 1) ? 0 : -1);
}
static inline int writeInt (FILE *f, int x) {
    return ((fwrite(&x, sizeof(int), 1, f) == 1) ? 0 : -1);
}
string TrackBuilder::saveDescription() {
    string s("int type code (MSB (0, byte track point, 8 byte track, 8 byte builder) LSB; int ntracks, n x ");
    if (!finishedtracks.empty()) {
        s.append(finishedtracks.front()->name());
        s.append("\n");s.append(finishedtracks.front()->name());s.append(":\n");
        s.append(finishedtracks.front()->saveDescription());
    } else if (!activetracks.empty()) {
        s.append(activetracks.front()->name());
        s.append("\n");s.append(activetracks.front()->name());s.append(":\n");
        s.append(activetracks.front()->saveDescription());
    } else {
        s.append ("track type unknown\n");
    }
    return s;
}
int TrackBuilder::toDisk(FILE* f) {
    if (f == NULL) return -1;
    int code = getTypeCode();
    message("Saving tracks to disk", verb_message);
    if (fwrite(&code, sizeof(int), 1, f) != 1) {
        message ("failed to write code to disk (first write operation)", verb_error);
        return -1;
    }
    int ntracks = activetracks.size() + finishedtracks.size();

    if (fwrite(&ntracks, sizeof(int), 1, f) != 1) {
        message ("failed to write number of tracks to disk (second write operation)", verb_error);
        return -1;
    }

    stringstream s;
    s << "starting to write " << ntracks << " tracks to disk";
    message(s.str().c_str(), verb_message);


    int tnum = 0;
    vector<Track *>::iterator it;
    for (it = activetracks.begin(); it != activetracks.end(); ++it) {
       // int j = 0; cout << ++j;
        ++tnum;
        if ((*it)->toDisk(f) != 0) {
            s.str("");
            s << "write failed on (active) track # " << tnum << " / " << ntracks << " total tracks";
            message(s.str().c_str(), verb_error);
            return -1;
        }
    }
     for (it = finishedtracks.begin(); it != finishedtracks.end(); ++it) {
        //int j = 0; cout << ++j;
        ++tnum;
        if ((*it)->toDisk(f) != 0) {
            s.str("");
            s << "write failed on (finished) track # " << tnum << " / " << ntracks << " total tracks";
            message(s.str().c_str(), verb_error);
            return -1;
        }
    }
     return 0;     
}
ptLoaderT TrackBuilder::pointLoader(int code) {
    switch(code >> 16) {
        case 1:
            return TrackPoint::fromDisk;
            break;
        case 2:
            return ImTrackPoint::fromDisk;
            break;
        case 3:
            return MaggotTrackPoint::fromDisk;
            break;
        case 4:
            return MidlineMaggotTrackPoint::fromDisk;
            break;
        default:
            cout << "unrecognized code " << code;
            return NULL;
            break;
    }
    return NULL;
}

int TrackBuilder::loadTracksFromDisk(FILE* f, TrackPoint *(*ptLoader) (FILE *f),int minLenToLoad) {
    if (f == NULL) return -1;
    
    int code;
   // cout << "code = " << code;
    if (fread(&code, sizeof(int), 1, f) != 1) { return -1; }
 //   printf ("code = %x\n", code);
    if (ptLoader == NULL) {
        ptLoader = pointLoader(code);
    }
    if (ptLoader == NULL) return -1;
    int ntracks;
    if (fread(&ntracks, sizeof(int), 1, f) != 1) {
        return -1;
    }
    vector<Track *> tracks;
    Track *tr;
    for (int j = 0; j < ntracks; ++j) {
        tr = Track::fromDisk(f, ptLoader);
        if (tr == NULL) {
            for (vector<Track *>::iterator it = tracks.begin(); it != tracks.end(); ++it) {
                delete (*it);
            }
            return -1;
        } else {
          //  cout << "track size " << tr->trackLength() << " min size " << minLenToLoad << "  ";
            if (tr->trackLength() >= minLenToLoad) {
            //    cout << "track loaded\n";
                tracks.push_back(tr);
            } else {
              //  cout << "track deleted\n";
                delete (tr);
            }
        }
    }
    assignTracks(tracks);
    return 0;
}

TrackBuilder *TrackBuilder::fromDisk (FILE *f, TrackPoint *(*ptLoader) (FILE *f) ) {
    TrackBuilder *tb = new TrackBuilder;
    if (tb->loadTracksFromDisk(f, ptLoader) != 0) {
        delete tb;
        return NULL;
    } else {
        return tb;
    }
}


void TrackBuilder::assignTracks(const vector<Track *> &tracks) {
    activetracks = tracks;
    vector<Track *>::iterator it = max_element(activetracks.begin(), activetracks.end(), trackEndsEarlier);
    int lastFrame = (*it)->lastFrame();
    for (it = activetracks.begin(); it != activetracks.end(); ++it) {
        if ((*it)->lastFrame() < lastFrame) {
            (*it)->endTrack();
        }
    }
    pruneInactive();
    frameNumber = lastFrame;
}

vector<Track *> *TrackBuilder::getTracksOut (whichTracksOutT whichTracks) {
    vector<Track *> *v;
    switch (whichTracks) {
        case get_active_tracks:
            v = new vector<Track *> (activetracks);
            break;
        case get_finished_tracks:
        case get_all_tracks:
            v = new vector<Track *> (finishedtracks);
            break;
    }
    if (whichTracks == get_all_tracks) {
        for (vector<Track *>::iterator it = activetracks.begin(); it!= activetracks.end(); ++it) {
            v->push_back(*it);
        }
    }
    return v;
}

int TrackBuilder::getTypeCode() {
    int code;
    if (!activetracks.empty()) {
        code = activetracks.front()->getTypeCode();        
    } else {
        if (!finishedtracks.empty()) {
            code = finishedtracks.front()->getTypeCode();        
        }else {
            code = 0;
        }
    } 
    code = (code << 8) + _id_code;
}

void TrackBuilder::setMessageHandler(communicator* mh) {
    this->mh = mh;
    vector<Track *>::iterator it;
     for (it = activetracks.begin(); it != activetracks.end(); ++it) {
      (*it)->setMessageHandler(mh);
    }
     for (it = finishedtracks.begin(); it != finishedtracks.end(); ++it) {
        (*it)->setMessageHandler(mh);
    }
}
