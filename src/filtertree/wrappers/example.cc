/*
  $Id: example.cc 5160 2010-03-25 23:07:11Z abehm $

  Copyright (C) 2010 by The Regents of the University of California
	
  Redistribution of this file is permitted under
  the terms of the BSD License.
    
  Date: 09/06/2008
  Author: Alexander Behm <abehm (at) ics.uci.edu>
*/

#include "wrappers.h"

// create a dummy dictionary
extern void readString(vector<string>& data, const string& filenameData, unsigned count, unsigned maxLineLen);
std::vector<string> dictionary;
void initDictionary();

// EXAMPLES
void wrapperSimpleExampleEd();
void wrapperSimpleExampleEdNorm();
void wrapperDiscardListsExample();
void wrapperOnDiskSimpleExample();

int main() {
  
  initDictionary();
  
  wrapperSimpleExampleEd();
  wrapperSimpleExampleEdNorm();
  wrapperDiscardListsExample();
  wrapperOnDiskSimpleExample();
  
  return 0;
}

void initDictionary() {
  // params: target vector, filename, number strings to read, max line length
  readString(dictionary, "../data/female_names.txt", 4000, 20);
}

// USAGE OF WRAPPERS FOR UNCOMPRESSED INDEXES
// List of Wrappers:
// WrapperSimpleEd
// WrapperSimpleEdNorm
// WrapperSimpleJacc
// WrapperSimpleCos
// WrapperSimpleDice
// see typedefs in wrapperuncompressed.h
// ALSO POSSIBLE TO SPECIFY SIMMETRIC AS TEMPLATE PARAMETER: WrapperSimple<SimilarityMetric>
void wrapperSimpleExampleEd() {
  cout << "----- EXAMPLE: WrapperSimpleEd -----" << endl;
  
  StringContainerVector strContainer;
  strContainer.fillContainer("../data/female_names.txt", 4000);

  // create wrapper and build index
  // params: stringcontainer, use partitioning filter?, gramlength, use pre- and postfixing or string?
  WrapperSimpleEd wrapper(&strContainer, 2, true, true);
  wrapper.buildIndex();

  // perform search
  float editDistance = 1.0f;
  string queryString = "kathrin";
  vector<unsigned> resultStringIDs;
  wrapper.search(queryString, editDistance, resultStringIDs);
  cout << "SIMILAR STRINGS: " << endl;
  for(unsigned i = 0; i < resultStringIDs.size(); i++) {
    string tmp;
    strContainer.retrieveString(tmp, resultStringIDs.at(i));
    cout << tmp << endl;
  }

  // save index
  wrapper.saveIndex("wrapperIndex.ix");

  // load index
  wrapper.loadIndex("wrapperIndex.ix");

  // perform search on loaded index
  resultStringIDs.clear();
  wrapper.search(queryString, editDistance, resultStringIDs);
  cout << "SIMILAR STRINGS: " << endl;
  for(unsigned i = 0; i < resultStringIDs.size(); i++) {
    string tmp;
    strContainer.retrieveString(tmp, resultStringIDs.at(i));
    cout << tmp << endl;
  }

  cout << "-----------------------------------------" << endl << endl;  
}

// USAGE OF WRAPPERS FOR UNCOMPRESSED INDEXES
// List of Wrappers:
// WrapperSimpleEd
// WrapperSimpleEdNorm
// WrapperSimpleJacc
// WrapperSimpleCos
// WrapperSimpleDice
// see typedefs in wrapperuncompressed.h
// ALSO POSSIBLE TO SPECIFY SIMMETRIC AS TEMPLATE PARAMETER: WrapperSimple<SimilarityMetric>
void wrapperSimpleExampleEdNorm() {
  cout << "----- EXAMPLE: WrapperSimpleEdNorm -----" << endl;
  
  StringContainerVector strContainer;
  strContainer.fillContainer(dictionary.begin(), dictionary.end());

  // create wrapper and build index
  // params: stringcontainer, use partitioning filter?, gramlength, use pre- and postfixing or string?
  WrapperSimpleEdNorm wrapper(&strContainer, 2, true, true);
  wrapper.buildIndex();
  
  // perform search
  float normEditDistance = 0.9f;
  string queryString = "kathrin";
  vector<unsigned> resultStringIDs;
  wrapper.search(queryString, normEditDistance, resultStringIDs);
  cout << "SIMILAR STRINGS: " << endl;
  for(unsigned i = 0; i < resultStringIDs.size(); i++) {
    string tmp;
    strContainer.retrieveString(tmp, resultStringIDs.at(i));
    cout << tmp << endl;
  }

  // save index
  wrapper.saveIndex("wrapperIndex.ix");

  // load index
  wrapper.loadIndex("wrapperIndex.ix");
  
  // perform search on loaded index
  resultStringIDs.clear();
  wrapper.search(queryString,  normEditDistance, resultStringIDs);
  cout << "SIMILAR STRINGS: " << endl;
  for(unsigned i = 0; i < resultStringIDs.size(); i++) {
    string tmp;
    strContainer.retrieveString(tmp, resultStringIDs.at(i));
    cout << tmp << endl;
  }

  cout << "-----------------------------------------" << endl << endl;  
}


// USAGE OF WRAPPERS FOR COMPRESSED INDEXES USING GLOBAL HOLES COMPRESSION
// List of Wrappers:

// Using Long-List-First hole selection (workload independent)
// WrapperDiscardListsLLFEd;
// WrapperDiscardListsLLFEdNorm;
// WrapperDiscardListsLLFJacc;
// WrapperDiscardListsLLFCos;
// WrapperDiscardListsLLFDice;

// Using Long-List-First hole selection (workload independent)
// WrapperDiscardListsSLFEd;
// WrapperDiscardListsSLFEdNorm;
// WrapperDiscardListsSLFJacc;
// WrapperDiscardListsSLFCos;
// WrapperDiscardListsSLFDice;

// Using Random-List hole selection (workload independent)
// WrapperDiscardListsRandomEd;
// WrapperDiscardListsRandomEdNorm;
// WrapperDiscardListsRandomJacc;
// WrapperDiscardListsRandomCos;
// WrapperDiscardListsRandomDice;

// Using Panic-Cost-Plus hole selection (workload dependent)
// WrapperDiscardListsPanicCostEd;
// WrapperDiscardListsPanicCostEdNorm;
// WrapperDiscardListsPanicCostJacc;
// WrapperDiscardListsPanicCostCos;
// WrapperDiscardListsPanicCostDice;

// Using Time-Cost-Plus hole selection (workload dependent)
// WrapperDiscardListsTimeCostEd;
// WrapperDiscardListsTimeCostEdNorm;
// WrapperDiscardListsTimeCostJacc;
// WrapperDiscardListsTimeCostCos;
// WrapperDiscardListsTimeCostDice;

// see typedefs in wrapperholesglobal.h
// ALSO POSSIBLE TO SPECIFY SIMMETRIC AS TEMPLATE PARAMETER: WrapperDiscardListsLLF<SimilarityMetric>
void wrapperDiscardListsExample() {
  cout << "----- EXAMPLE: WrapperDiscardLists ----" << endl;

  StringContainerVector strContainer;
  strContainer.fillContainer(dictionary.begin(), dictionary.end());
  
  // create wrapper and build index using 0.5 as reduction ratio
  WrapperDiscardListsLLFEd wrapper(&strContainer, 2, true, 0.5);
  wrapper.buildIndex();

  // perform search
  float editDistance = 1.0f;
  string queryString = "kathrin";
  vector<unsigned> resultStringIDs;
  wrapper.search(queryString, editDistance, resultStringIDs);
  cout << "SIMILAR STRINGS: " << endl;
  for(unsigned i = 0; i < resultStringIDs.size(); i++) {
    string tmp;
    strContainer.retrieveString(tmp, resultStringIDs.at(i));
    cout << tmp << endl;
  }

  // save index
  wrapper.saveIndex("wrapperIndex.ix");

  // load index
  wrapper.loadIndex("wrapperIndex.ix");

  // perform search on loaded index
  resultStringIDs.clear();
  wrapper.search(queryString, editDistance, resultStringIDs);
  cout << "SIMILAR STRINGS: " << endl;
  for(unsigned i = 0; i < resultStringIDs.size(); i++) {
    string tmp;
    strContainer.retrieveString(tmp, resultStringIDs.at(i));
    cout << tmp << endl;
  }

  cout << "-----------------------------------------" << endl << endl;  
}


// USAGE OF WRAPPERS FOR DISK-BASED INDEXES
// List of Wrappers:

// wrappers using simple merging algorithm
// WrapperOnDiskSimpleEd
// WrapperOnDiskSimpleEdNorm
// WrapperOnDiskSimpleJacc
// WrapperOnDiskSimpleCos
// WrapperOnDiskSimpleDice

// wrappers using adaptive merging algorithm
// WrapperOnDiskAdaptEd
// WrapperOnDiskAdaptEdNorm
// WrapperOnDiskAdaptJacc
// WrapperOnDiskAdaptCos
// WrapperOnDiskAdaptDice

// see typedefs in wrapperondisk.h
// ALSO POSSIBLE TO SPECIFY SIMMETRIC AS TEMPLATE PARAMETER: WrapperSimple<SimilarityMetric>
void wrapperOnDiskSimpleExample() {
  cout << "----- EXAMPLE: WrapperOnDisk -----" << endl;
  
  StringContainerRM strContainer;
  strContainer.createAndOpen("wrapperContainer.rm");
  strContainer.fillContainer(dictionary.begin(), dictionary.end());
  
  // create wrapper and build index
  // params: stringcontainer, use partitioning filter?, gramlength, use pre- and postfixing or string?
  WrapperOnDiskSimpleEd wrapper(&strContainer, "invLists.ix", 2, true, true);
  wrapper.buildIndex();

  // perform search
  float editDistance = 1.0f;
  string queryString = "kathrin";
  vector<unsigned> resultStringIDs;
  wrapper.search(queryString, editDistance, resultStringIDs);
  cout << "SIMILAR STRINGS: " << endl;
  for(unsigned i = 0; i < resultStringIDs.size(); i++) {
    string tmp;
    strContainer.retrieveString(tmp, resultStringIDs.at(i));
    cout << tmp << endl;
  }

  // save index
  wrapper.saveIndex("wrapperIndex.ix");

  // load index
  wrapper.loadIndex("wrapperIndex.ix");

  // perform search on loaded index
  resultStringIDs.clear();
  wrapper.search(queryString, editDistance, resultStringIDs);
  cout << "SIMILAR STRINGS: " << endl;
  for(unsigned i = 0; i < resultStringIDs.size(); i++) {
    string tmp;
    strContainer.retrieveString(tmp, resultStringIDs.at(i));
    cout << tmp << endl;
  }

  cout << "-----------------------------------------" << endl << endl;  
}
