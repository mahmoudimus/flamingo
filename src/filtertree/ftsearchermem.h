/*
  $Id: ftsearchermem.h 5146 2010-03-24 23:05:57Z abehm $

  Copyright (C) 2010 by The Regents of the University of California
	
  Redistribution of this file is permitted under
  the terms of the BSD license.
    
  Date: 04/04/2008
  Author: Alexander Behm <abehm (at) ics.uci.edu>
*/

#ifndef _ftsearchermem_h_
#define _ftsearchermem_h_

#include "ftsearcherabs.h"
#include "ftindexermem.h"
#include "listmerger/divideskipmerger.h"

template <class FtIndexer, class Merger>
class FtSearcherMem;

template <class TFtIndexer, class TMerger>
struct FtSearcherTraits<FtSearcherMem<TFtIndexer, TMerger> > {
  typedef TFtIndexer FtIndexer;
  typedef TMerger Merger;
};

template <class FtIndexer = FtIndexerMem<>, class Merger = DivideSkipMerger<> >
class FtSearcherMem 
  : public FtSearcherAbs<FtSearcherMem<FtIndexer, Merger> > {
  
public:
  typedef FtIndexerTraits<FtIndexer> IndexerTraitsType;
  typedef typename IndexerTraitsType::InvList InvList;
  
protected:
  typedef FtSearcherAbs<FtSearcherMem<FtIndexer, Merger> > SuperClass;

public:
#ifdef DEBUG_STAT
  typedef WorkloadStats WlStatsType;
  typedef QueryStats QueryStatsType;
#endif
  
  FtSearcherMem(Merger* merger, FtIndexer* ftIndexer = NULL) : SuperClass(merger, ftIndexer) {
#ifdef DEBUG_STAT
    this->queryStats = new QueryStats();
#endif
  }
  
  void prepare_Impl() {}

  void processLeaves_Impl(const vector<FilterTreeNode<InvList>*>& leaves,
			  const Query& query,
			  const vector<unsigned>& queryGramCodes,
			  vector<unsigned>& resultStringIDs);

  string getName() {
    return "FtSearcherMem";
  }
};

template<class FtIndexer, class Merger>
void 
FtSearcherMem<FtIndexer, Merger>::
processLeaves_Impl(const vector<FilterTreeNode<InvList>*>& leaves,
		   const Query& query,
		   const vector<unsigned>& queryGramCodes,
		   vector<unsigned>& resultStringIDs) {
  
  // Step 0: Check for panic
  if((signed)this->mergeThreshold <= 0) {
    for(unsigned i = 0; i < leaves.size(); i++)
      this->searchLeafNodePanic(leaves.at(i), query, resultStringIDs);
    return;
  } 
  
#ifdef DEBUG_STAT
  this->queryStats->strLen = query.str.size();
  this->queryStats->mergeThresh = this->mergeThreshold;
  this->queryStats->simThresh = query.threshold;
#endif

  for(unsigned i = 0; i < leaves.size(); i++) {          
    // Step 1: Preprocess
#ifdef DEBUG_STAT    
    this->sutil.startTimeMeasurement();    
#endif

    vector<unsigned> candidates;  
    vector<InvList*> lists;
    leaves.at(i)->getInvertedLists(queryGramCodes, lists);
    
#ifdef DEBUG_STAT
    this->sutil.stopTimeMeasurement();
    this->queryStats->preprocTime += this->sutil.getTimeMeasurement();
#endif
    
    // Step 2.1: Merge
#ifdef DEBUG_STAT
    this->sutil.startTimeMeasurement();    
#endif
    
    this->merger->merge(lists, this->mergeThreshold, candidates);
    
#ifdef DEBUG_STAT
    this->sutil.stopTimeMeasurement();
    this->queryStats->mergeTime += this->sutil.getTimeMeasurement();
    this->queryStats->candidates += candidates.size();
#endif
    
    // Step 2.2: Postprocess
    this->postProcess(query, candidates, resultStringIDs);
  }
}

#endif
