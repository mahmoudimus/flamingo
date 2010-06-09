/*
  $Id: stringcontainer.h 5229 2010-05-11 05:44:03Z abehm $

  Copyright (C) 2010 by The Regents of the University of California
	
  Redistribution of this file is permitted under
  the terms of the BSD license.
    
  Date: 04/04/2008
  Author: Alexander Behm <abehm (at) ics.uci.edu>
*/

#ifndef _stringcontainer_h_
#define _stringcontainer_h_

#include "stringrm.h"
#include "util/misc.h"
#include "util/debug.h"
#include "common/filtertypes.h"
#include <math.h>
#include <vector>
#include <fstream>
#include <cstdlib>
#include <iterator>
#include <cstring>

enum PhysOrd {
  PHO_NONE,
  PHO_AUTO,
  PHO_LENGTH,
  PHO_CHARSUM,
  PHO_LENGTH_CHARSUM,
  PHO_CHARSUM_LENGTH
};

class FilterStats {
private:
  unsigned minKey;
  unsigned maxKey;
  float wtedAvgValCount;
  unordered_map<unsigned, unsigned> countMap;  
  AbstractFilter* filter;
  
public:  
  FilterStats(AbstractFilter* filter);
  
  void begin();
  void next(const string& s);
  void end(float ed, unsigned strCount);

  unsigned getMinKey() { return minKey; }
  unsigned getMaxKey() { return maxKey; }
  float getWtedAvgValCount() { return wtedAvgValCount; }
  FilterType getFilterType() { return filter->getType(); }
  
  ~FilterStats() {
    if(filter) delete filter;
  }
};

class StatsCollector {
private:
  vector<FilterStats*> filterStats;  
  float avgStrLen;
  unsigned strCount;
  unsigned maxStrLen;
  unsigned charFreqs[256];
  
public:  
  StatsCollector();
  
  void begin();
  void next(const string& s);
  void end();
  
  float getAvgStrLen() { return avgStrLen; }
  unsigned getMaxStrLen() { return maxStrLen; }
  unsigned* getCharFreqs() { return charFreqs; }
  FilterStats* getBestPartFilter();
  void clearFilterStats();
  
  ~StatsCollector();
};

// function template for creating any stringcontainer with default initialization
template<class T> 
T* createDefaultStringContainer();

template <class ConcreteStringContainer>
class StringContainerAbs {
protected:
  // collect statistical info on string collection for auto-tuning
  StatsCollector statsCollector; 
  bool gatherStats;
  
public:
  StringContainerAbs(bool gatherStats = true) { this->gatherStats = true; }
  StringContainerAbs(PhysOrd pho, unsigned partSize = 0, unsigned clusters = 0, bool gatherStats = true) { this->gatherStats = true; }
  
  // interface for filling string containers
  void fillContainer(const char* fileName, const unsigned count, const unsigned maxLineLen = 300) {
    static_cast<ConcreteStringContainer*>(this)->fillContainer_Impl(fileName, count, maxLineLen);
  }

  // commonly used fill functions
  void fillContainer(ConcreteStringContainer* target, 
		     const char* fileName, 
		     const unsigned count, 
		     const unsigned maxLineLen = 300);

  template<typename InputIterator>
  void fillContainer(InputIterator first, InputIterator last) {
    static_cast<ConcreteStringContainer*>(this)->fillContainer_Impl(first, last);
  }
  
  // interfaces for concrete string containers
  void retrieveString(string& target, const unsigned i) {
    static_cast<ConcreteStringContainer*>(this)->retrieveString_Impl(target, i);
  }
  
  void insertString(const string& s) {
    static_cast<ConcreteStringContainer*>(this)->insertString_Impl(s);
  }

  unsigned size() {
    return static_cast<ConcreteStringContainer*>(this)->size_Impl();
  }

  void flushCache() {
    return static_cast<ConcreteStringContainer*>(this)->flushCache_Impl();
  }
  
  RecordID* getRecordID(unsigned i) {
    return static_cast<ConcreteStringContainer*>(this)->getRecordID_Impl(i);
  }  
  
  StatsCollector* getStatsCollector() { 
    if(gatherStats) return &statsCollector; 
    else return NULL;
  }
};

template <class ConcreteStringContainer>
void
StringContainerAbs<ConcreteStringContainer>::
fillContainer(ConcreteStringContainer* target, const char* fileName, const unsigned count, const unsigned maxLineLen) {
  ifstream fileData(fileName);
  if (!fileData) {
    cout << "can't open input file \"" << fileName << "\"" << endl;
    return;
  }
  
  cout << "INPUTFILE: \"" << fileName << "\"" << endl;
  
  char line[maxLineLen + 1];
  bool isIgnore = false;
  
  if(gatherStats) statsCollector.begin();
  
  TIMER_START("FILLING CONTAINER", count);
  while (true) {
    fileData.getline(line, maxLineLen + 1);
    if (fileData.eof())
      break;
    if (fileData.rdstate() & ios::failbit) {
      isIgnore = true;
      while (fileData.rdstate() & ios::failbit) {      
	fileData.clear(fileData.rdstate() & ~ios::failbit);
	fileData.getline(line, maxLineLen);
      }
      cout << "open reading input file \"" << fileName << "\"" << endl
	   << "line length might exceed " << maxLineLen << " characters" << endl;
      return;
    }
    else {
      const string &s = string(line);
      target->insertString(s);
      if(gatherStats) statsCollector.next(s);
    }
    if (count != 0 && target->size() == count)
      break;
    TIMER_STEP();
  }
  TIMER_STOP();
  
  if(gatherStats) statsCollector.end();

  fileData.close();
  
  if (isIgnore)
    cout << "WARNING" << endl << "some lines in the file exceeded " 
	 << maxLineLen << " characters and were ignored" << endl;

  flushCache();  
}

// very simple string container which is just a wrapper around stl vector
class StringContainerVector : public StringContainerAbs<StringContainerVector> {
private:
  vector<string> container;
public:  
  StringContainerVector(bool gatherStats = true) : StringContainerAbs<StringContainerVector>(gatherStats) { }
  StringContainerVector(PhysOrd pho, unsigned partSize = 0, unsigned clusters = 0, bool gatherStats = true) 
    : StringContainerAbs<StringContainerVector>(pho, partSize, clusters, gatherStats) {
    if(partSize != 0 || clusters != 0 || pho != PHO_NONE) cout << "WARNING: physical ordering and clustering StringContainerVector will be ignored" << endl;
  }
  
  void fillContainer_Impl(const char* fileName, const unsigned count, const unsigned maxLineLen = 300);

  template<typename InputIterator>
  void fillContainer_Impl(InputIterator first, InputIterator last);

  void retrieveString_Impl(string& target, const unsigned i) { target = container.at(i); }
  void insertString_Impl(const string& s) { container.push_back(s); }
  unsigned size_Impl() { return container.size(); }
  void flushCache_Impl() {}
  RecordID* getRecordID_Impl(unsigned i) { cout << "WARNING: This container has no RIDs" << endl; return NULL; }
};

template<typename InputIterator>
void 
StringContainerVector::fillContainer_Impl(InputIterator first, InputIterator last) {
#ifdef TIMER_START
  cout << "FILLING CONTAINER..." << endl;
#endif
  if(gatherStats) statsCollector.begin();
  while(first != last) {    
    if(gatherStats) statsCollector.next(*first);
    insertString(*first);
    first++;
  }
  if(gatherStats) statsCollector.end();
}

typedef struct
{
  unsigned stringId;
  unsigned charsum;
  unsigned length;
} StringAttribs;

// disk-based string container consisting of the following:
// 1. string record manager
// 2. mapping from stringID -> recordID
// the mapping is stored in memory
class StringContainerRM : public StringContainerAbs<StringContainerRM> {
private:
  vector<RecordID> ridMap;
  StringRM stringRM;

  PhysOrd pho;   // physical ordering of records
  unsigned partSize; // partition size for clustering
  unsigned clusters; // number of clusters in each partition  
  char ridMapFileName[256]; // file name for ridMap
  
  // comparison function used in qsort for ordering by PHO_LENGTH
  int static cmpStringAttribsLen(const void* a, const void* b) {    
    const StringAttribs* x = static_cast<const StringAttribs* >(a);
    const StringAttribs* y = static_cast<const StringAttribs* >(b);
    if(x->length < y->length) return -1;
    if(x->length > y->length) return 1;
    return 0;
  }

  // comparison function used in qsort for ordering by PHO_CHARSUM
  int static cmpStringAttribsCharsum(const void* a, const void* b) {    
    const StringAttribs* x = static_cast<const StringAttribs* >(a);
    const StringAttribs* y = static_cast<const StringAttribs* >(b);
    if(x->charsum < y->charsum) return -1;
    if(x->charsum > y->charsum) return 1;
    return 0;
  }

  // comparison function used in qsort for ordering by PHO_LENGTH_CHARSUM  
  int static cmpStringAttribsLenCharsum(const void* a, const void* b) {    
    const StringAttribs* x = static_cast<const StringAttribs* >(a);
    const StringAttribs* y = static_cast<const StringAttribs* >(b);
    if(x->length < y->length) return -1;
    if(x->length > y->length) return 1;
    if(x->charsum < y->charsum) return -1;
    if(x->charsum > y->charsum) return 1;
    return 0;
  }
  
  // comparison function used in qsort for ordering by PHO_CHARSUM_LENGTH
  int static cmpStringAttribsCharsumLen(const void* a, const void* b) {    
    const StringAttribs* x = static_cast<const StringAttribs* >(a);
    const StringAttribs* y = static_cast<const StringAttribs* >(b);
    if(x->charsum < y->charsum) return -1;
    if(x->charsum > y->charsum) return 1;
    if(x->length < y->length) return -1;
    if(x->length > y->length) return 1;
    return 0;
  }
  
  void clusterRemap(vector<string>& strs, StringAttribs* strAttribs, unsigned currentStartIndex, unsigned partition);  
  void copyReorg(StringContainerRM* tempContainer);
  
  void writeRidMapFile() {
    fstream ridMapFile;
    ridMapFile.open(ridMapFileName, ios::out | fstream::trunc);
    unsigned ridMapSize = ridMap.size();
    ridMapFile.write((const char*)&ridMapSize, sizeof(unsigned));
    for(unsigned i = 0; i < ridMapSize; i++) {
      RecordID rid = ridMap.at(i);
      ridMapFile.write((const char*)&rid, sizeof(RecordID));
    }
    ridMapFile.close();
  }

  void readRidMapFile() {
    fstream ridMapFile;
    ridMapFile.open(ridMapFileName, ios::in);
    unsigned ridMapSize = 0;
    ridMapFile.read((char*)&ridMapSize, sizeof(unsigned));
    if(ridMapSize > 0) {
      ridMap.clear();
      for(unsigned i = 0; i < ridMapSize; i++) {
	RecordID rid;
	ridMapFile.read((char*)&rid, sizeof(RecordID));
	ridMap.push_back(rid);
      }
    }
    ridMapFile.close();
  }
  
public:    
  StringContainerRM(bool gatherStats = true) : StringContainerAbs<StringContainerRM>(gatherStats) { pho = PHO_NONE; partSize = 0; clusters = 0; memset(ridMapFileName, 0, 256); }
  StringContainerRM(PhysOrd pho, unsigned partSize = 0, unsigned clusters = 0, bool gatherStats = true)
    : StringContainerAbs<StringContainerRM>(pho, partSize, clusters, gatherStats) { 
    this->pho = pho; this->partSize = partSize; this->clusters = clusters; memset(ridMapFileName, 0, 256);
  }
  
  void fillContainer_Impl(const char* fileName, const unsigned count, const unsigned maxLineLen = 300);  

  template<typename InputIterator>
  void fillContainer_Impl(InputIterator first, InputIterator last);

  void createContainer(const char* fileName, const unsigned blockSize = 4096, const unsigned avgstrlen = 50) {
    stringRM.createStringCollection(fileName, blockSize, avgstrlen);
  }

  void openContainer(const char* fileName, bool disableStreamBuffer = false) {
    stringRM.openStringCollection(fileName, disableStreamBuffer);    
    sprintf(ridMapFileName, "ridmap_%s", fileName);
    fstream ridMapFile;
    readRidMapFile();
  }
  
  // combined create and open into one call
  void createAndOpen(const char* fileName, 
		     const unsigned blockSize = 4096, 
		     const unsigned avgstrlen = 50,  
		     bool disableStreamBuffer = false) {
    stringRM.createStringCollection(fileName, blockSize, avgstrlen);
    stringRM.openStringCollection(fileName, disableStreamBuffer);
    sprintf(ridMapFileName, "ridmap_%s", fileName);
    ridMap.clear();
    writeRidMapFile();
  }
  
  void retrieveString_Impl(string& target, const unsigned i) {
    stringRM.retrieveString(target, ridMap.at(i));
  }
  
  void insertString_Impl(const string& s) {
    RecordID tmp;
    stringRM.insertString(s, tmp);
    ridMap.push_back(tmp);
  }
  
  bool inCache(unsigned sid) { return stringRM.inCache(ridMap.at(sid)); }

  unsigned size_Impl() { return ridMap.size(); }
  void flushCache_Impl() { stringRM.flushCache(); }
  RecordID* getRecordID_Impl(unsigned i) { return &ridMap.at(i); }
};

template<typename InputIterator>
void StringContainerRM::fillContainer_Impl(InputIterator first, InputIterator last) {
#ifdef TIMER_START
  cout << "FILLING CONTAINER..." << endl;
#endif
  if(pho != PHO_NONE) {
    StringContainerRM* tempContainer = createDefaultStringContainer<StringContainerRM>();
    while(first != last) {
      tempContainer->insertString(*first);
      first++;
    }
    copyReorg(tempContainer);
    delete tempContainer;
    int ret = system("rm default2.rm"); // WARNING: OS specific
    if(ret != 0) cout << "Temp container file default2.rm could not be deleted" << endl;
  }
  else {
    if(gatherStats) statsCollector.begin();
    while(first != last) {
      if(gatherStats) statsCollector.next(*first);
      insertString(*first);
      first++;
    }
    if(gatherStats) statsCollector.end();
  }
  
  writeRidMapFile();

  flushCache();
}


#endif
