/* 
 * File:   MaggotReprocessor.cpp
 * Author: Marc
 * 
 * Created on October 6, 2010, 3:28 PM
 * (C) Marc Gershow; licensed under the Creative Commons Attribution Share Alike 3.0 United States License.
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/us/ or send a letter to
 * Creative Commons, 171 Second Street, Suite 300, San Francisco, California, 94105, USA.
 */

#include "MaggotReprocessor.h"
#include "MaggotTrackBuilder.h"
#include "MaggotTrackExtractor.h"
#include <string>
#include <vector>
#include <map>

using namespace std;

static void splitFileName (const string filename, string &stub, string &extension);

MaggotReprocessor::MaggotReprocessor() {
    init();
}

MaggotReprocessor::MaggotReprocessor(const MaggotReprocessor& orig) {
}

MaggotReprocessor::~MaggotReprocessor() {
}

void MaggotReprocessor::init() {
    minArea = 5;
    maxArea = 10000;
    maxMaggotContourAngle = 3.142/2;
}

int MaggotReprocessor::processOne(bool showExtraction, verbosityLevelT verbosity) {
    string inputname;
    string outputname;
    if (filenames.empty()) {
        return 0;
    }
    inputname = filenames.back().first;
    outputname = filenames.back().second;
    filenames.pop_back();

    MaggotTrackExtractor te;
    te.showExtraction = showExtraction;
    te.logVerbosity = verbosity;
    te.maxArea = maxArea;
    te.minArea = minArea;
    te.reanalyze(inputname, outputname);
    te.saveOutput();

    return filenames.size();
}

void MaggotReprocessor::addFiles(std::string existingFile, std::string outputFile) {
    filenames.push_back(pair<string, string>(existingFile, outputFile));
}

void MaggotReprocessor::addFiles(std::string existingFile) {
    filenames.push_back(pair<string, string>(existingFile, string("")));
}

void MaggotReprocessor::processAll(bool showExtraction, verbosityLevelT verbosity) {
    while (processOne(showExtraction, verbosity) > 0) {
        //all taken care of in processOne
        ;
    }
}

void MaggotReprocessor::reprocessFile(std::string existingFile, std::string outputName, int minArea, int maxArea, double maxMaggotContourAngle, verbosityLevelT verbosity, bool showExtraction) {
    MaggotReprocessor mrp;
    mrp.setAngles(minArea, maxArea);
    mrp.maxMaggotContourAngle = maxMaggotContourAngle;
    mrp.addFiles(existingFile, outputName);
    mrp.processAll(showExtraction, verbosity);
}

