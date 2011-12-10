/* 
 * File:   main.cpp
 * Author: Marc
 *
 * Created on December 6, 2010, 9:54 AM
 * Created on November 2, 2010, 2:34 PM
 * (C) Marc Gershow; licensed under the Creative Commons Attribution Share Alike 3.0 United States License.
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/us/ or send a letter to
 * Creative Commons, 171 Second Street, Suite 300, San Francisco, California, 94105, USA.
 */

#include <cstdlib>
#include <algorithm>
#include "TrackExtractor.h"
#include "MaggotTrackExtractor.h"
#include "BatchExtractor.h"
#include "yaml.h"

using namespace std;

static const char *default_tename = "default_track_extractor_settings.txx";

typedef enum filetypeT {mmf, bxx, txx, nll, notfound};

filetypeT getExtension (const char *fname, char *fname_without_extension = NULL, char *extension = NULL);

TrackExtractor *defaultTE(void);
void doExtraction (const char *arg1, const char *arg2, const char *arg0);
TrackExtractor *loadTEFromDisk (const char *tename);
void extractMMF (const char *filename, const char *tename);
void runBatch (const char *filename);
/*
 * 
 */

int main(int argc, char** argv) {

    switch(argc) {
        case 2:
            doExtraction(argv[1], NULL, argv[0]);
            break;
        case 3:
            doExtraction(argv[1], argv[2], argv[0]);
            break;
        default:
            cout << argv[0] << " [imagestack.mmf] trackextractor.txx" << endl;
            cout << argv[0] << " batchextractor.bxx" << endl;
            cout << argv[0] << " imagestack.mmf (uses default track extractor settings, stored in " << default_tename << endl;
            break;
    }
    return 0;
}

void doExtraction (const char *arg1, const char *arg2, const char *arg0) {
    const char *a1, *a2;
    filetypeT ft[2], temp;
    a1 = arg1;
    a2 = arg2;
    ft[0] = getExtension(arg1);
    ft[1] = getExtension(arg2);
    if (ft[0] > ft[1]) {
        temp = ft[0];
        ft[0] = ft[1];
        ft[1] = temp;
        a1 = arg2;
        a2 = arg1;
    }
 //   cout << " arg1 = " << arg1 << endl;
//    cout << " a1 = " << a1 << endl;
    if (arg2 == NULL) {
  //      cout << "arg2 = NULL" << endl;
    } else {
   //     cout << "arg2 = " << a2 << endl;
    }
    switch (ft[0]) {
        case mmf:
            switch (ft[1]) {
                case txx:
                    extractMMF(a1, a2);
                    break;
                case nll:
                    extractMMF(a1, NULL);
                    break;
                default:
                    cout << arg0 << " [imagestack.mmf] trackextractor.txx" << endl;
                    cout << "if one argument is an mmf, other must be a track extractor (.txx)" << endl;
                    return;
            }
            break;
        case bxx:
            switch (ft[1]) {
                case nll:
                    runBatch(a1);
                    break;
                default:
                    cout << arg0 << " batchextractor.bxx" << endl;
                    cout << "only one argument is allowed for batch files (.bxx or .bat)" << endl;
                    return;
            }
            break;
        case txx:
            switch (ft[1]) {
                case nll:
                    extractMMF(NULL, a1);
                    break;
                default:
                    cout << arg0 << " trackextractor.txx" << endl;
                    cout << "if one argument is a track extractor, other must be an mmf" << endl;
                    return;
            }
            break;
        default:
            cout << arg0 << " [imagestack.mmf] trackextractor.txx" << endl;
            cout << arg0 << " batchextractor.bxx" << endl;
            cout << arg0 << " imagestack.mmf (uses default track extractor settings, stored in " << default_tename << endl;
            break;
    }

    


    
    
}

TrackExtractor *loadTEFromDisk (const char *tename) {
    try {
        ifstream is(tename);
        if (is.bad()) {
            is.close();
            cout << "is bad" << endl;
            return defaultTE();
        }

        YAML::Parser parser(is);
        YAML::Node node;
        parser.GetNextDocument(node);
     //   std::cout << "parsed\n";
        if (node.Type() != YAML::NodeType::Map) {
            cout << "failed to parse " << tename << "node is not a map " <<endl;
            return defaultTE();
        }
        if (node.FindValue("max maggot contour angle")) { //it's a maggot
            MaggotTrackExtractor *mte = new MaggotTrackExtractor;
      //      cout << "it's a maggot" << endl;
            mte->fromYAML(node);
            return mte;
        } else {
    //        cout << "basic track extractor" << endl;
            TrackExtractor *te = new TrackExtractor;
            te->fromYAML(node);
            return te;
        }

    } catch (YAML::ParserException &e) {
        std::cout << "Parser Error " << e.what() << "\n";
        return defaultTE();
    }
   
}

void extractMMF (const char *fname, const char *tename) {
    BatchExtractor be;
    TrackExtractor *te;
    if (tename == NULL) {
        te = loadTEFromDisk(default_tename);
    } else {
        te = loadTEFromDisk(tename);
    }

    if (fname != NULL) {
        char *fst = new char [strlen(fname)];
        char *ext = new char [strlen(fname)];
        getExtension(fname, fst, ext);
        te->fstub = string(fst);
        te->extension = string(ext);
        if (te->extension.empty()) {
            te->extension = "mmf";
        }
        delete fst;
        delete ext;
    }
    transform(te->extension.begin(), te->extension.end(), te->extension.begin(), ::tolower);
    
    int ind = te->fstub.rfind("_stack");
    string outstub = te->fstub.substr(0, ind);
    te->outputname = outstub + "_tracks.bin";
    YAML::Emitter out;
    te->toYAML(out);
    cout << out.c_str() << endl << endl << endl;
    be.processOne(te->fstub, te->outputname, te);


  
     
    
}

filetypeT getExtension (const char *fname, char *fname_without_extension, char *extension) {
    if (fname == NULL) {
        return nll;
    }
    string filename(fname);
    int ind = filename.find_last_of('.');
    string fstub = filename.substr(0, ind);
    string ext = filename.substr(ind+1);
    if (fname_without_extension != NULL) {
        fstub.copy(fname_without_extension, fstub.length());
    }
    if (extension != NULL) {
        ext.copy(extension, ext.length());
    }
    if (ext.compare("mmf") == 0) {
        return mmf;
    }
    if (ext.compare("bxx") == 0 || ext.compare("bat") == 0) {
        return bxx;
    }
    if (ext.compare("txx") == 0) {
        return txx;
    }
    return notfound;
}


TrackExtractor *defaultTE(void) {
    MaggotTrackExtractor *te = new MaggotTrackExtractor();
    te->background_blur_sigma = 0;
    te->background_resampleInterval = 200;
    te->blurThresholdIm_sigma = 0;
    te->endFrame = -1;
    te->startFrame = 0;
    te->analysisRectangle = cvRect (0,0,-1,-1);
    te->extension = string(".mmf");
    te->fnm = _frame_none;
    te->imstacklength = 0;
    te->logVerbosity = verb_debug;
    te->maxArea = 2000;
    te->minArea = 5;
    te->maxExtractDist = 15;
    te->nBackgroundFrames = 5;
    te->overallThreshold = 30;
    te->padding = 0;
    te->showExtraction = true;
    te->startFrame = 0;
    te->winSize = 50;
    te->setMaxAngle(1.57);
    YAML::Emitter out;
    ofstream os(default_tename);
    te->toYAML(out);
    cout << out.c_str();
    os << out.c_str();

    return te;
}

void runBatch (const char *filename) {
    BatchExtractor *be = new BatchExtractor();
    YAML::Node node;
    try {
        ifstream is(filename);
        YAML::Parser parser(is);

        parser.GetNextDocument(node);
    //    std::cout << "parsed\n";
    } catch (YAML::ParserException &e) {
        std::cout << "Parser Error " << e.what() << "\n";
        delete be;
        return;
    }
    char errmsg[4096];
    if (be->fromYaml(node, errmsg, sizeof(errmsg))) {
        delete be;
        cout << errmsg;
        return;
    }
    be->batchProcess();
    cout << "finished processing" << endl;
}