/* 
 * File:   MaggotTrackExtractor.cpp
 * Author: Marc
 * 
 * Created on November 6, 2009, 3:15 PM
 * (C) Marc Gershow; licensed under the Creative Commons Attribution Share Alike 3.0 United States License.
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/us/ or send a letter to
 * Creative Commons, 171 Second Street, Suite 300, San Francisco, California, 94105, USA.
 */

#include "MaggotTrackExtractor.h"
#include "MaggotTrackBuilder.h"
#include "communicator.h"
#include "tictoc.h"

void splitFileName (const string filename, string &stub, string &extension);

MaggotTrackExtractor::MaggotTrackExtractor() {
    maxAngle = 3.14159253 / 1.99;
    reprocessed = false;
}

int MaggotTrackExtractor::go() {
    _TICTOC_TIC_FUNC;
    int rv;
    rv = TrackExtractor::go();
    if (rv != 0 || aborted) {
        _TICTOC_TOC_FUNC;
        return rv;
    }
    if (!outputname.empty()) {
        mh->message("Top Level (Maggot Track Extractor)", verb_warning, " Saving Track Extraction Results to disk before analyzing maggot tracks");
        saveOutput();
    }
    MaggotTrackBuilder *mtb = dynamic_cast<MaggotTrackBuilder *> (tb);
    if (mtb == NULL) {
        mh->message("Top Level (Maggot Track Extractor)", verb_error, " Failed to cast track builder to maggot track builder\n");
        return -1;
    }
    mtb->analyzeMaggotTracks(minArea, maxArea, maxAngle, showExtraction);
    _TICTOC_TOC_FUNC;
    return 0;
}
void MaggotTrackExtractor::reanalyze(string inputname, string outputname) {
    this->outputname = outputname;
    string basename = outputname;
    size_t ind = basename.find_last_of('.');
    basename.erase(ind);
    logName = basename + "_log.txt";
    headerinfoname = basename + "_header.txt";
    aborted = false;
     if (logName.empty()) {
        mh = new communicator();
    } else {
        mh = new communicator(logName.c_str());
    }
    assert(mh != NULL);
    mh->setVerbosity(logVerbosity);

    reprocessed = true;
    reprocessname = inputname;

    if (outputname.empty()) {
        string ext;
        splitFileName(inputname, outputname, ext);
        outputname.append("_reextracted");
        outputname.append(ext);
    }

    FILE *f = fopen(inputname.c_str(), "rb");
    stringstream s;
    if (f == NULL) {
        s << "couldn't open " << inputname;
        message(s.str().c_str(), verb_error);
        return;
    }
    tb = MaggotTrackBuilder::fromDisk(f);
    if (tb == NULL) {
        s << "couldn't load " << inputname << "\n";
        message(s.str().c_str(), verb_error);
        return;
    }
    tb->setMessageHandler(mh);
    fclose(f);

    //te->fstub = fstub;
    MaggotTrackBuilder *mtb = dynamic_cast<MaggotTrackBuilder *> (tb);
    if (mtb == NULL) {
        mh->message("Top Level (Maggot Track Extractor)", verb_error, " Failed to cast track builder to maggot track builder\n");
        return;
    }
    mtb->analyzeMaggotTracks(minArea, maxArea, maxAngle, showExtraction);


    message("finished analyzing maggot tracks", verb_message);


}



void MaggotTrackExtractor::createTrackBuilder() {
    tb = new MaggotTrackBuilder(startFrame);
}
YAML::Emitter& MaggotTrackExtractor::yamlBody(YAML::Emitter& out) const {
    TrackExtractor::yamlBody(out);
    out << YAML::Key << "max maggot contour angle" << YAML::Value <<  maxAngle;
    return out;
}
void MaggotTrackExtractor::fromYAML(const YAML::Node &node) {
    TrackExtractor::fromYAML(node);
    const YAML::Node *pname = node.FindValue("max maggot contour angle");
    if (pname != NULL) {
        node["max maggot contour angle"] >> maxAngle;
    }

}

YAML::Emitter& operator << (YAML::Emitter& out, const MaggotTrackExtractor *te) {
    std::cout << "out << MTE*\n";
    if (te == NULL) {
        return (out << YAML::Null);
    } else {
        return out << (*te);
    }
}
YAML::Emitter& operator << (YAML::Emitter& out, const MaggotTrackExtractor& te) {
    std::cout << "out << MTE\n";
    return te.toYAML(out);
}
void operator >> (const YAML::Node &node, MaggotTrackExtractor *(&te)) {
    if (YAML::IsNull(node)) {
        te = NULL;
    } else {
        te = new MaggotTrackExtractor();
        te->fromYAML(node);
    }
}

void MaggotTrackExtractor::createPointExtractor() {
    pe = new PointExtractor();
}

void operator >> (const YAML::Node &node, MaggotTrackExtractor &te) {
    if (YAML::IsNull(node)) {
        return;
    } else {
        te.fromYAML(node);
    }
}

void MaggotTrackExtractor::headerinfo(std::ostream &os) {
    if (!reprocessed) {
        TrackExtractor::headerinfo(os);
        return;
    }
    message("writing modified header for reanalyzed file", verb_message);
    os << outputname << "\ncontains the the reprocessing of:\n " << reprocessname << "\nstored in the following format:\n";
    os << tb->saveDescription();
}

void splitFileName (const string filename, string &stub, string &extension) {
    size_t dot = filename.rfind('.');
    stub = filename.substr(0, dot-1);
    extension = filename.substr(dot+1);
}