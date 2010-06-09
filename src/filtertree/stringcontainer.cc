/*
  $Id: stringcontainer.cc 5229 2010-05-11 05:44:03Z abehm $

  Copyright (C) 2010 by The Regents of the University of California
	
  Redistribution of this file is permitted under
  the terms of the BSD license.
    
  Date: 04/04/2008
  Author: Alexander Behm <abehm (at) ics.uci.edu>
*/

#include "stringcontainer.h"

#include "common/gramgen.h"
#include "common/simmetric.h"
#include "sepia/sepia.h"
#include "util/misc.h"
#include <cfloat>
#include <cstring>

template<>
StringContainerVector*
createDefaultStringContainer() {
  return new StringContainerVector(); 
}

template<>
StringContainerRM*
createDefaultStringContainer() {
  StringContainerRM* container = new StringContainerRM();
  container->createContainer("default2.rm");
  container->openContainer("default2.rm", true);
  return container;
}

FilterStats::
FilterStats(AbstractFilter* filter)
  : filter(filter) {
  begin();
}

void 
FilterStats::
begin() {
  minKey = 0xFFFFFFFF;
  maxKey = 0;
  wtedAvgValCount = 0;
  countMap.clear();
}

void
FilterStats::
next(const string& s) {
  unsigned key = filter->getKey(s);
  countMap[key]++;
  if(key < minKey) minKey = key;
  if(key > maxKey) maxKey = key;
}

void
FilterStats::
end(float ed, unsigned strCount) {
  unsigned start = 0;
  unsigned stop = 0;        
  GramGenFixedLen gramGen(3);
  SimMetricEd sim(gramGen);
  sim.getFilterBounds("", 1, filter, start, stop);
  stop = ed * stop;
  
  // compute weighted average
  unsigned valCount = 0;
  unordered_map<unsigned, unsigned>::iterator iter;   
  for(unsigned i = minKey; i <= minKey + stop; i++) {
    iter = countMap.find(i);
    if(iter != countMap.end()) valCount += iter->second;                  
  }    
  float weight = (float)valCount / (float)strCount;
  float totalWeight = weight;
  wtedAvgValCount = (float)valCount * weight;
  
  for(unsigned i = minKey + stop + 1; i <= maxKey; i++) {
    unsigned j = i - (minKey + stop + 1);
    
    iter = countMap.find(i);
    if(iter != countMap.end()) valCount += iter->second;                  
    
    iter = countMap.find(j);
    if(iter != countMap.end()) valCount -= iter->second;            
    
    if((signed) valCount < 0) valCount = 0;
    
    weight = (float)valCount / (float)strCount;
    totalWeight += weight;
    wtedAvgValCount += (float)valCount * weight;      
  }
  wtedAvgValCount /= totalWeight;
  
  countMap.clear();
}

StatsCollector::
StatsCollector() {
  begin();
}

void 
StatsCollector::
begin() {
  avgStrLen = 0;
  strCount = 0;
  memset(charFreqs, 0, 256);  
  
  clearFilterStats();  
  filterStats.push_back( new FilterStats(new LengthFilter((unsigned)0)) );
  filterStats.push_back( new FilterStats(new CharsumFilter((unsigned)0)) );
}

void
StatsCollector::
next(const string& s) {
  for(unsigned i = 0; i < filterStats.size(); i++)
    filterStats.at(i)->next(s);
  
  avgStrLen += s.size();
  strCount++;  

  for(unsigned i = 0; i < s.size(); i++) {
    unsigned char ix = (unsigned char)s[i];
    charFreqs[ix]++;
  }
}

void 
StatsCollector::
end() {      
  avgStrLen /= (float)strCount;    
  float ed = (unsigned)(avgStrLen - floor(avgStrLen * 0.85));
  
  for(unsigned i = 0; i < filterStats.size(); i++)
    filterStats.at(i)->end(ed, strCount);
}

FilterStats*
StatsCollector::
getBestPartFilter() {
  unsigned bestIx = 0xFFFFFFFF;
  float bestValCount = FLT_MAX;
  for(unsigned i = 0; i < filterStats.size(); i++) {
    float currValCount = filterStats.at(i)->getWtedAvgValCount();
    if(currValCount < bestValCount) {
      bestIx = i;
      bestValCount = currValCount;
    }
  }
  
  if(bestIx != 0xFFFFFFFF) return filterStats.at(bestIx);
  else return NULL;
}

void 
StatsCollector::
clearFilterStats() {
  for(unsigned i = 0; i < filterStats.size(); i++)
    if(filterStats.at(i)) delete filterStats.at(i);
  filterStats.clear();
}

StatsCollector::  
~StatsCollector() {
  clearFilterStats();
}

void
StringContainerVector::
fillContainer_Impl(const char* fileName, const unsigned count, const unsigned maxLineLen) {
  this->fillContainer(this, fileName, count, maxLineLen);
}

void 
StringContainerRM::
clusterRemap(vector<string>& strs, 
	     StringAttribs* strAttribs, 
	     unsigned currentStartIndex,
	     unsigned partition) {
  
  if(strs.size() == 0) return;
  
  // TODO some bug with sepia destructor
  Sepia* sepia = new Sepia(strs, 1, 4, 10);
  sepia->build();
  Clusters* clusters = sepia->getClusters();
  
  StringAttribs* newStrAttribs = new StringAttribs[strs.size()];
  unsigned newAttribCount = 0;
  for(unsigned j = 0; j < clusters->sizeCluster(); j++) {
    const Cluster& cluster = clusters->getCluster(j);
    for(ContCluster::const_iterator iter = cluster.begin(); iter != cluster.end(); iter++) {
      newStrAttribs[newAttribCount++] = strAttribs[*iter + currentStartIndex];
    }	
  }  
  
  // copy new strAttribs back in place
  memcpy(&strAttribs[currentStartIndex], newStrAttribs, strs.size() * sizeof(StringAttribs));	
  delete newStrAttribs;
}

void 
StringContainerRM::
copyReorg(StringContainerRM* tempContainer) {
  
  CharsumFilter csFilter((unsigned)0);
  
  // fill array of string attribs for sorting
  StringAttribs* strAttribs = new StringAttribs[tempContainer->size()];
  for(unsigned i = 0; i < tempContainer->size(); i++) {   
    string currentString;
    tempContainer->retrieveString(currentString, i);    
    strAttribs[i].charsum = csFilter.getCharsum(currentString);
    strAttribs[i].stringId = i;
    strAttribs[i].length = currentString.size();
  }
  
  // sort collection
  switch(pho) {
    
  case PHO_NONE: break;
    
  case PHO_AUTO: {
    if(gatherStats) {
      FilterStats* bestFilterStats = statsCollector.getBestPartFilter();
      switch(bestFilterStats->getFilterType()) {

      case FT_LENGTH: {
	qsort(strAttribs, tempContainer->size(), sizeof(StringAttribs), cmpStringAttribsLenCharsum);  
      } break;	

      case FT_CHARSUM: {
	qsort(strAttribs, tempContainer->size(), sizeof(StringAttribs), cmpStringAttribsCharsumLen);  
      } break;	

      case FT_NONE: break;    
      }
    }
    else {
      cout << "WARNING: no physical ordering set because container has no stats" << endl;
    }
  } break;

  case PHO_LENGTH: {
    qsort(strAttribs, tempContainer->size(), sizeof(StringAttribs), cmpStringAttribsLen);  
  } break;

  case PHO_CHARSUM: {
    qsort(strAttribs, tempContainer->size(), sizeof(StringAttribs), cmpStringAttribsCharsum);  
  } break;

  case PHO_LENGTH_CHARSUM: {
    qsort(strAttribs, tempContainer->size(), sizeof(StringAttribs), cmpStringAttribsLenCharsum);  
  } break;

  case PHO_CHARSUM_LENGTH: {
    qsort(strAttribs, tempContainer->size(), sizeof(StringAttribs), cmpStringAttribsCharsumLen);  
  } break;

  default: {
    cout << "WARNING: invalid PhysOrd specified!" << endl;
  } break;

  }
  
  // cluster partitioned strings using sepia
  if(partSize && clusters) {
    unsigned currentStartIndex = 0;
    unsigned partition = 0;
    vector<string> strs;
    TIMER_START("CLUSTERING STRINGS", tempContainer->size() / partSize);
    for(unsigned i = 0; i < tempContainer->size(); i++) {        
      string s;
      tempContainer->retrieveString(s, strAttribs[i].stringId);
      if(strs.size() >= partSize) {		
	clusterRemap(strs, strAttribs, currentStartIndex, partition++);	
	currentStartIndex = i;
	strs.clear();
	TIMER_STEP();
      }
      strs.push_back(s);
    }
    clusterRemap(strs, strAttribs, currentStartIndex, partition++);
    TIMER_STOP();
  }
  
  if(gatherStats) statsCollector.begin();      
  
  // copy strings into new collection
  TIMER_START("SORTING STRINGS", tempContainer->size());
  for(unsigned i = 0; i < tempContainer->size(); i++) {  
    string currentString;
    tempContainer->retrieveString(currentString, strAttribs[i].stringId);
    if(gatherStats) statsCollector.next(currentString);
    insertString(currentString);
    TIMER_STEP();
  }
  TIMER_STOP();
  
  if(gatherStats) statsCollector.end();
  
  delete strAttribs;
}

void
StringContainerRM::
fillContainer_Impl(const char* fileName, const unsigned count, const unsigned maxLineLen) {
  if(pho != PHO_NONE) {
    StringContainerRM* tempContainer = createDefaultStringContainer<StringContainerRM>();
    this->fillContainer(tempContainer, fileName, count, maxLineLen);
    copyReorg(tempContainer);
    delete tempContainer;
    int ret = system("rm default2.rm"); // WARNING: OS specific
    if(ret != 0) cout << "Temp container file default2.rm could not be deleted" << endl;
  }
  else {
    fillContainer(this, fileName, count, maxLineLen);
  }

  writeRidMapFile();
  
  flushCache();
}
