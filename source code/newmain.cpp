/* 
 * File:   newmain.cpp
 * Author: Marc
 *
 * Created on October 26, 2009, 1:34 PM
 *
 * (C) Marc Gershow; licensed under the Creative Commons Attribution Share Alike 3.0 United States License.
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/us/ or send a letter to
 * Creative Commons, 171 Second Street, Suite 300, San Francisco, California, 94105, USA.
 */
#include <ctime>
#include <stdlib.h>
#include <vector>
#include <algorithm>
#include <iosfwd>
#include "Track.h"
#include "ImLoader.h"
#include "trackbuilder.h"
#include "ImTrackPoint.h"
#include "PointExtractor.h"
#include "cv.h"
#include "highgui.h"
#include "CvUtilsPlusPlus.h"
#include "communicator.h"
#include "MaggotTrackPoint.h"
#include "MaggotTrackExtractor.h"
#include "MaggotTrackBuilder.h"
#include "BatchExtractor.h"
#include "tictoc.h"
#include "newDLLWrapper.h"
/*
 * 
 */

static const char *fname = "testOutputAgain2.bin";
static const char *fstub = "D:\\phototaxis\\1405\\1405_";

void testCEExtraction () ;
void testTE();
void testFileLoad();
void testTELoad ();
void testMaggotAnalysis ();
void testTEYAML();
void testBEYAML();
void testForegroundBatch();
void testBatch();
void cvWindowTest();
void cvMultTest();
int main(int argc, char** argv) {
   // cvMultTest();
//    cvWindowTest();
  //  return 0;
   // testForegroundBatch();
    testBatch();
   // cout << "test batch returned\n";
  //  testMaggotAnalysis();
 //   testTE();
    //testTELoad();
    //testFileLoad();
    //testCEExtraction();
    return (EXIT_SUCCESS);
}

void testTE() {
     YAML::Emitter out;
    MaggotTrackExtractor mte;
//    mte.logVerbosity = verb_debug;
    mte.analysisRectangle = cvRect(2592/2 - 900, 1944/2 - 900, 1800, 1800);
    mte.thresholdScaleImage = "d:\\ashleymap.bmp";
    mte.fstub = fstub;
    mte.extension = "jpg";
    mte.startFrame = 0;
    mte.endFrame = 4800;
    mte.overallThreshold = 20;
    mte.winSize = 45;
    mte.maxExtractDist = 20;
    mte.background_blur_sigma = 1;
    mte.background_resampleInterval = 200;
    mte.nBackgroundFrames = 5;

    mte.outputname = "retestOutputReAgain.bin";
    mte.headerinfoname = "retestOutputReAgain_hdr.txt";
   // mte.logName = "restestOutputReAgain.log";

    mte.setMaxAngle(3.14159253/2);
    mte.showExtraction = true;

   
    mte.toYAML(out);
    std::cout << out.c_str();

    mte.go();
    mte.saveOutput();    
}
void testTEYAML() {
    /*
  //  YAML::Emitter out;
    MaggotTrackExtractor mte;
//    mte.logVerbosity = verb_debug;
    mte.analysisRectangle = cvRect(2592/2 - 900, 1944/2 - 900, 1800, 1800);
    mte.thresholdScaleImage = "d:\\ashleymap.bmp";
    mte.fstub = fstub;
    mte.extension = "jpg";
    mte.startFrame = 0;
    mte.endFrame = 4800;
    mte.overallThreshold = 20;
    mte.winSize = 45;
    mte.maxExtractDist = 20;
    mte.background_blur_sigma = 1;
    mte.background_resampleInterval = 200;
    mte.nBackgroundFrames = 5;

    mte.outputname = "retestOutputReAgain.bin";
    mte.headerinfoname = "retestOutputReAgain_hdr.txt";
   // mte.logName = "restestOutputReAgain.log";

    mte.setMaxAngle(3.14159253/2);
    mte.showExtraction = true;
*/

 //   mte.toYAML(out);
//    ofstream os("testyaml.txt");

 //   os << out.c_str();

//   os.close();
    MaggotTrackExtractor te;
    try {
        ifstream is("testyaml2.txt");
        YAML::Parser parser(is);
        YAML::Node node;
        parser.GetNextDocument(node);

        std::cout << "parsed\n";
        te.fromYAML(node);

        
        //te.fromYAML(node);
    } catch (YAML::ParserException &e) {
        std::cout << "Parser Error " << e.what() << "\n";
    }
    std::cout << "from yaml\n";

    YAML::Emitter out1;
    //te.toYAML(out1);
    out1 << te;

   // TrackExtractor *g = &te;
//    out1 << g;

    std:cout << out1.c_str();

    BatchExtractor be;

    be.setDefaultTrackExtractor(&te);
    be.addImageStack("test name 1", "test out 1");
    be.addImageStack("test name 2", "test out 2");
    be.addImageStack("special test name 3", "test out 3", &te);

    std::cout << "made a new batch extractor\n";

   // is.close();
    {
        YAML::Emitter out2;
        be.toYaml(out2);
        std::cout << "to yaml\n";

        ofstream os2("testbeyaml.txt");
        os2 << out2.c_str();
        std::cout << out2.GetLastError() << "\n";
        assert(out2.good());
     }

    

    std::cout << "to disk\n";

   // os2.close();

    std::cout << "closed\n";
    //mte.go();
    //mte.saveOutput();

}
void testBEYAML() {

    TrackExtractor te;
    BatchExtractor be;

    try {
        ifstream is("dennisparams.txt");
        YAML::Parser parser(is);
        YAML::Node node;
        parser.GetNextDocument(node);

        std::cout << "parsed\n";
        te.fromYAML(node);

    } catch (YAML::ParserException &e) {
        std::cout << "Parser Error " << e.what() << "\n";
    }

    be.setDefaultTrackExtractor(&te);

    std::cout << "loaded track extractor\n";
    string inname("\\\\labnas1\\Share\\Dennis Data\\Gradient behavior for 18 best odors\\Air control and VS1-files\\Trans-3-hexenol\\VS1_\\VS1_");
    be.addImageStack(inname, string(""),NULL);
    inname = string("\\\\labnas1\\Share\\Dennis Data\\Gradient behavior for 18 best odors\\Air control and VS1-files\\Geranyl acetate\\VS1_\\VS1_");
    be.addImageStack(inname, string(""),NULL);
    

     // is.close();
    {
        YAML::Emitter out2;
        be.toYaml(out2);
        std::cout << "to yaml\n";

        ofstream os2("testbeyaml2.txt");
        os2 << out2.c_str();
        std::cout << out2.GetLastError() << "\n";
        assert(out2.good());
     }



    std::cout << "to disk\n";

}
void testForegroundBatch() {
    BatchExtractor be;
    try {
        ifstream is("\\\\labnas1\\Share\\Dennis Data\\Gradient behavior for 18 best odors\\Air control and VS1-files\\foregroundbe.txt");
        YAML::Parser parser(is);
        YAML::Node node;
        parser.GetNextDocument(node);

        std::cout << "parsed\n";
        be.fromYaml(node);

    } catch (YAML::ParserException &e) {
        std::cout << "Parser Error " << e.what() << "\n";
    }
    be.batchForeground();
}
void testBatch() {
    BatchExtractor be;
    try {
        ifstream is("e:\\analysis.bat");
        YAML::Parser parser(is);
        YAML::Node node;
        parser.GetNextDocument(node);

        std::cout << "parsed\n";
        be.fromYaml(node);

    } catch (YAML::ParserException &e) {
        std::cout << "Parser Error " << e.what() << "\n";
    }
    YAML::Emitter out;
    be.toYaml(out);
    std::cout << out.c_str();
    //be.batchToMaggot(3.1416 / 1.99);
}

void testTELoad () {
    FILE *f = fopen ("retestOutputReAgain.bin", "rb");
    if (f == NULL) {
        printf("failed to open file\n");
    }
    TrackBuilder *tb = TrackBuilder::fromDisk(f, MaggotTrackPoint::fromDisk);
    fclose(f);

    if (tb != NULL) {
        printf("tracks loaded from disk!\n");
    } else {
        printf ("read failed\n");
        return;
    }

    cvNamedWindow("Your Mom");
    vector<Track *> *v = tb->getTracksOut(get_all_tracks);
    sort(v->begin(), v->end(), &(Track::longestIsFirst));

    for (vector<Track *>::iterator it = v->begin(); it != v->end(); ++it) {
        cout << "playing track movie length " << (*it)->trackLength() << "\n";
        (*it)->playTrackMovie("Your Mom", 50);
        if (toupper(cvWaitKey(1000)) == 'Q') {
            break;
        };
    }
    cvWaitKey(-1);
    delete tb;
    return;
    
}



void testCEExtraction () {
   // char *fstub = "\\\\labnas1\\Share\\David\\Image Data\\david\\20091023\\temporal ramp 1\\n2_g15";
   // char *fstub = "\\\\labnas1\\Share\\Dennis Data\\Gradient behavior for 18 best odors\\movie files for 18 odors\\Acetal\\Valve gradient\\VS2_\\VS2_";
    char *extension = "jpg";
    int firstFrame = 0;
    int lastFrame = 4800;
    int threshold = 20;
    communicator mh;

    IplImage *ashley = NULL, *foreground = NULL, *foreIm = NULL;
    ashley = cvLoadImage("d:\\ashleymap.bmp", 0);
    allocateImage(&foreground, ashley);
    cvConvertScale(ashley, foreground, threshold/255.0, 0);
    blurIm(foreground, foreground, 10);
    clock_t cl;
    cl = clock();

    mh.setVerbosity(verb_warning);

    CvRect ar = cvRect(2592/2 - 900, 1944/2 - 900, 1800, 1800);
   // ar = cvRect(0,0,2592,1944);
    ImLoader *fl = new ImLoader();
    fl->setFileName(fstub, extension);
    fl->setAnalysisRect(ar);
    fl->setMessageHandler(&mh);

    PointExtractor *pe = new PointExtractor();
    pe->setMessageHandler(&mh);
    BackgroundParamsT bp;
    double blurSigma = 1;
    int nBackgroundFrames = 5;
    int resampleInterval = 200;

    
    int winsize = 40;

    pe->setBackgroundParams(nBackgroundFrames, resampleInterval, blurSigma);
    pe->setFrameRange(firstFrame, lastFrame);
    pe->setAnalysisRect(ar);
    pe->setFrameLoader(fl);
    pe->setWinSize(winsize);
 //   pe->setGlobalThreshold(threshold);
    subImage(foreground, &foreIm, ar);
    pe->setThresholdCompareIm(foreIm);
    pe->setAreaRange(20, 1E4);


   // cvSaveImage("foreground.bmp", pe->generateForeground(20));
  //  return;

    TrackBuilder *tb = new TrackBuilder();
    tb->setMessageHandler(&mh);
    double maxDist = 20;
    tb->setConnectionDistance(maxDist);
    tb->setPointExtractor(pe);
    IplImage *bs = NULL, *display = NULL;
  //  cvNamedWindow("Your Mom",0);
    while (tb->completeStep() == 0/*pe->nextFrame() == 0*/) {
        /*
        tb->nextFrame(pe->getFrameNumber());
        vector<TrackPoint *> *v = pe->getPoints();
    //    printf ("v[1] ID x y %d %g %g\n", v->front()->getID(), v->front()->getLocation().x,v->front()->getLocation().y);
       // vector<TrackPoint *> *w (v);

        //printf ("w[1] ID x y %d %g %g\n", w->front()->getID(), w->front()->getLocation().x,w->front()->getLocation().y);
        tb->addPoints(v);
        tb->extendTracks();
        pe->copyBSImagOut(&bs);
        allocateImage(&display, cvGetSize(bs), IPL_DEPTH_8U, 3);
        mh.message("new main", verb_verbose, "converting display image");
        cvConvertImage (bs, display, 0);
        tb->drawTracks(display, -ar.x, -ar.y);
         * */

       // tb->printTracks(true);
        //tb->printTracks(false);
      //  tb->extractionImage(&display);
     //   cvShowImage("Your Mom", display);
     //   cvWaitKey(1);
        if (!(pe->getFrameNumber()%100)) {
            printf ("%d frames done; %g ms / frame\n", pe->getFrameNumber(), (1.0*clock() - cl)/pe->getFrameNumber());
        }
    }
    vector<Track *> *v = tb->getTracksOut(get_all_tracks);
    sort(v->begin(), v->end(), &(Track::longestIsFirst));
    /*
    for (vector<Track *>::iterator it = v->begin(); it != v->end(); ++it) {
        cout << "playing track movie length " << (*it)->trackLength();
        (*it)->playTrackMovie("Your Mom", 100);
        if (toupper(cvWaitKey(-1)) == 'Q') {
            break;
        };
    }
    */
    cout << tb->saveDescription();
    FILE *f = fopen (fname, "wb");
    if (f == NULL) {
        printf("failed to open file\n");
    }
    if (tb->toDisk(f) == 0) {
        printf("tracks saved to disk!\n");
    } else {
        printf ("write failed\n");
    }
    fclose(f);
    cvWaitKey(-1);
    delete pe;
    delete tb;
    delete v;
    cvReleaseImage(&bs);
    cvReleaseImage(&display);
    
}

void testFileLoad() {
    FILE *f = fopen (fname, "rb");
    if (f == NULL) {
        printf("failed to open file\n");
    }
    TrackBuilder *tb = TrackBuilder::fromDisk(f, ImTrackPoint::fromDisk);
    fclose(f);
     
    if (tb != NULL) {
        printf("tracks loaded from disk!\n");
    } else {
        printf ("read failed\n");
        return;
    }

    cvNamedWindow("Your Mom");
    vector<Track *> *v = tb->getTracksOut(get_all_tracks);
    sort(v->begin(), v->end(), &(Track::longestIsFirst));
//    MaggotTrackPoint::convertITPToMTP(v->front()->getPointVector())

    for (vector<Track *>::iterator it = v->begin(); it != v->end(); ++it) {
        cout << "analyzing maggot track\n";
        cout << "mean area is " << (*it)->meanArea() <<" median area is " << (*it)->medianArea();
        MaggotTrackPoint::analyzeTrack((*it)->getPointVector(), (*it)->medianArea(), 3.14159253/2, 35);
        cout << "playing track movie length " << (*it)->trackLength() << "\n";
        (*it)->playTrackMovie("Your Mom", 50);
        if (toupper(cvWaitKey(1000)) == 'Q') {
            break;
        };
    }
     cvWaitKey(-1);
    delete tb;
    return;
            /*
    CvRect ar = cvRect(2592/2 - 900, 1944/2 - 900, 1800, 1800);

    IplImage *dst = NULL;
    allocateImage(&dst, cvSize(ar.width, ar.height), IPL_DEPTH_8U, 3);
    tb->drawTracks(dst, -ar.x, -ar.y);
    cvNamedWindow("Your mom's mom",0);
    cvShowImage("Your mom's mom", dst);
    cvWaitKey(-1);
    delete tb;
    */

}

void testMaggotAnalysis () {

    //char *fname = "\\\\LABNAS1\\Share\\Phototaxis\\Data Extracted\\Spatial\\Linear\\063010_1600_CS_tracks.bin";
    //char *fnameout = "\\\\LABNAS1\\Share\\Phototaxis\\Data Extracted\\Spatial\\Linear\\063010_1600_CS_tracks_suck.bin";

    char *fname = "G:\\New Maggot Extraction Test\\071910_1415_SQUARE_CS_tracks.bin";
    char *fnameout = "G:\\New Maggot Extraction Test\\071910_1415_SQUARE_CS_tracks_redone.bin";

    reprocessMaggotFile(fname, fnameout, 5, 10000, 3.142/2, 3, 1);

    return;
    MaggotTrackBuilder mtb;

  //  cvNamedWindow("gaussian", 0);
   // cvNamedWindow("dgaussian", 0);
    vector<double> g = gaussKernel(3);
    vector<double> dg = dgaussKernel(3);
    IplImage *img = NULL, *imdg = NULL;
    simplePlot(g, &img);
    simplePlot(dg, &imdg);
   // cvShowImage("gaussian", img);
   // cvShowImage ("dgaussian", imdg);
    
    //FILE *f = fopen ("\\\\labnas1\\Share\\Marc Data\\odor gradients\\Ethyl Acetate 4 pct 10 2000\\20010205\\CS2\\CS2_tracks.bin", "rb");
     FILE *f = fopen (fname, "rb");

    if (f == NULL) {
        cout << "couldn't open read file\n";
        return;
    }
//    TrackBuilder *tb = (MaggotTrackBuilder::fromDisk(f, MaggotTrackPoint::fromDisk));
  //   fclose(f);
   // if (tb == NULL) {
     //   cout << "read failed\n";
    //}
    //mtb = dynamic_cast<MaggotTrackBuilder *> (tb);
   

    if (mtb.loadTracksFromDisk(f,NULL,2000) != 0) {
        cout << "reading MTB failed\n";
        return;
    } else {
        cout << "tracks loaded\n";
    }
   //  cvWaitKey(-1);
    communicator mh;
    mh.setVerbosity(verb_warning);
    mtb.setMessageHandler(&mh);
    mtb.analyzeMaggotTracks(5, 2000, 3.14159253/2, true);
    
    //f = fopen("\\\\labnas1\\Share\\Marc Data\\odor gradients\\Ethyl Acetate 4 pct 10 2000\\20010205\\CS2\\CS2_tracks_2.bin", "wb");
     f = fopen (fnameout, "wb");

    if (f == NULL) {
        cout << "couldn't open write file\n";
        return;
    }
    if (mtb.toDisk(f)) {
        cout << "write failed\n";
    } else {
        cout << "write succeeded\n";
    }
    fclose(f);
    cout << mtb.saveDescription();
    cout << TICTOC::timer().generateReport();
}

void cvWindowTest() {
    cvNamedWindow("test", 0);
    cvWaitKey(-1);
    void *ptr1, *ptr2;
    ptr1 = cvGetWindowHandle("test");
    ptr2 = cvGetWindowHandle("testes");
    cout << ((int) ptr1) << " " << ((int) ptr2) << "\n";
    cvWaitKey(-1);
    clock_t last = clock();
    while(toupper(cvWaitKey(1)) != 'Q') {
        while (clock() - last < 1000)
        {;
        }
        last = clock();
        cout << ((int) ptr1) << " " << ((int) ptr2) << "\n";
    }
}

void cvMultTest() {
    IplImage *im = NULL;
    allocateImage(&im, cvSize(1024,768), IPL_DEPTH_8U, 1);
    IplImage *mult = NULL, *dst = NULL;
    allocateImage(&mult, im);
    allocateImage(&dst, im);
    cvSet (im, cvScalarAll(0), NULL);
    cvRectangle(im, cvPoint(0,0), cvPoint(100,100), cvScalarAll(25),-1);
    cvRectangle(im, cvPoint(100,100), cvPoint(200,200), cvScalarAll(50),-1);
    cvRectangle(im, cvPoint(200,200), cvPoint(300,300), cvScalarAll(100),-1);
    cvRectangle(im, cvPoint(300,300), cvPoint(400,400), cvScalarAll(200),-1);
    cvRectangle(im, cvPoint(400,400), cvPoint(500,500), cvScalarAll(255),-1);
    cvSet (mult, cvScalarAll(200), NULL);
    cvMul(mult, im, dst, 1.0/100.0);
    cvNamedWindow("squares",0);
    cvShowImage("squares", im);
    cvNamedWindow("mult",0);
    cvShowImage("mult",dst);
    cvWaitKey(-1);
    cvReleaseImage(&im);
    cvReleaseImage(&dst);
    cvReleaseImage(&mult);
}
