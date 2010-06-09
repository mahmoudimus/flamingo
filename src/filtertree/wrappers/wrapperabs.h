/*
  $Id: wrapperabs.h 5146 2010-03-24 23:05:57Z abehm $

  Copyright (C) 2010 by The Regents of the University of California
	
  Redistribution of this file is permitted under
  the terms of the BSD License.
    
  Date: 09/06/2008
  Author: Alexander Behm <abehm (at) ics.uci.edu>
*/

#ifndef _wrapperabs_h_
#define _wrapperabs_h_

#include "filtertree/ftindexermem.h"
#include "filtertree/ftsearchermem.h"
#include "filtertree/ftindexerondisk.h"
#include "filtertree/ftsearcherondisk.h"
#include "common/simmetric.h"
#include "common/gramgen.h"
#include "common/query.h"

// this wrapper should NOT be instantiated because it has no defined constructor
// sub-classes should define constructors
template<class Indexer, class Searcher, class SimilarityMetric>
class WrapperAbs {
protected:
  GramGen* gramGen;
  DivideSkipMerger<> merger;
  SimilarityMetric* simMetric;
  Indexer* indexer;
  Searcher* searcher;
  
public:
  void buildIndex(const string& dataFile, const unsigned linesToRead = 0) {
    indexer->buildIndex(dataFile, linesToRead, true);
  }
  
  void buildIndex() {
    indexer->buildIndex();
  }
  
  void saveIndex(const char* indexFileName) {
    indexer->saveIndex(indexFileName);
  }
  
  void loadIndex(const char* indexFileName) {
    Indexer* oldIndexer = indexer;
    indexer = new Indexer(oldIndexer->strContainer);
    indexer->loadIndex(indexFileName);
    searcher->setFtIndexer(indexer);
    delete oldIndexer;
  }
  
  void search(const string& query, const float simThreshold, vector<unsigned>& results) {
    Query q(query, *simMetric, (float)simThreshold);
    searcher->search(q, results);
  }
  
  virtual ~WrapperAbs() {
    if(gramGen) delete gramGen;
    if(simMetric) delete simMetric;
    if(indexer) delete indexer;    
    if(searcher) delete searcher;
  }
};

#endif 
