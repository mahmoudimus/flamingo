/*
  $Id: statsgen.cc 5146 2010-03-24 23:05:57Z abehm $

  Copyright (C) 2010 by The Regents of the University of California
	
  Redistribution of this file is permitted under
  the terms of the BSD license.
    
  Date: 04/04/2008
  Author: Alexander Behm <abehm (at) ics.uci.edu>
*/

#include "statsgen.h"
#include "util/input.h"
#include "util/misc.h"

StatsGenConfig::
StatsGenConfig() {
  gramGen = NULL;
  simMetric = NULL;
  simThresh = 0.0f;

  dictSizeStart = 0;
  dictSizeStop = 0;
  dictSizeStep = 0;  
  fanout = 0;
  maxStrLen = 0;
  autoPartFilter = false;

  numberQueries = 0;  
  distinctQueries = 0;
  queriesDistrib = QD_UNIFORM;
  zipfSkew = 1;
  numberRuns = 1;  

  csfOpts = CSF_NONE;

  outputFlags = OF_WORKLOADSTATS | OF_QUERYRESULTS | OF_QUERYSTATS;  
  outputFilePrefix = string("output");
  
  reductionStart = 0;
  reductionStop = 0;
  reductionStep = 1;
  dictSamplingFrac = 0;
  queriesSamplingFrac = 0;
  sampleRatioCost = false;
  
  dropCachesBeforeEachQuery = false;
  dropCachesBeforeBuildingIndex = false;
  disableStreamBuffer = false;  
  runBuffer = 500000;
  scatteredOrg = false;
  pho = PHO_NONE;
  partSize = 0;
  clusters = 0;  
  avgStrLen = 0;
  bufferSlots = 0;
}

void 
StatsGenConfig::
clearFilters() { 
  for(unsigned i = 0; i < filterTypes.size(); i++) {
    AbstractFilter* f = filterTypes.at(i);
    if(f) delete f;
  }
  filterTypes.clear();
}  

template<>
StringContainerVector*
createDefaultStringContainer(StatsGenConfig* config) {
  return new StringContainerVector(config->autoPartFilter); 
}

template<>
StringContainerRM*
createDefaultStringContainer(StatsGenConfig* config) {
  StringContainerRM* container = new StringContainerRM(config->pho, config->partSize, config->clusters, config->autoPartFilter);
  container->createContainer("default.rm", 4096, config->avgStrLen);
  container->openContainer("default.rm", config->disableStreamBuffer);
  return container;
}
