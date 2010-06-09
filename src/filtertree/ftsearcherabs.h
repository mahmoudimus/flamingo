/*
  $Id: ftsearcherabs.h 5146 2010-03-24 23:05:57Z abehm $

  Copyright (C) 2010 by The Regents of the University of California
	
  Redistribution of this file is permitted under
  the terms of the BSD license.
    
  Date: 04/04/2008
  Author: Alexander Behm <abehm (at) ics.uci.edu>
*/

#ifndef _ftsearcherabs_h_
#define _ftsearcherabs_h_

#include "ftindexerabs.h"
#include "statsutil.h"
#include "filtertreenode.h"
#include "common/filtertypes.h"
#include "common/query.h"

template<class FtSearcherConcrete>
struct FtSearcherTraits;

template <class FtSearcherConcrete>
class FtSearcherAbs {
public:
  typedef FtSearcherTraits<FtSearcherConcrete> TraitsType;
  typedef typename TraitsType::FtIndexer FtIndexer;
  typedef typename TraitsType::Merger Merger;

  typedef FtIndexerTraits<FtIndexer> IndexerTraitsType;  
  typedef typename IndexerTraitsType::InvList InvList;
  
protected:  
#ifdef DEBUG_STAT
  QueryStats* queryStats;
  StatsUtil sutil;
#endif
  
  unsigned mergeThreshold;
  GramGen* gramGen;
  StatsUtil sutilLocal;
  
  Merger* merger;
  FtIndexer* ftIndexer;
  
  void getAffectedLeavesRec(FilterTreeNode<InvList>* node,
			    const Query& query,
			    const vector<unsigned>& queryGramCodes,
			    const vector<AbstractFilter*>* filterTypes,
			    const unsigned filterId,
			    vector<FilterTreeNode<InvList>*>& leaves);
  
  void searchLeafNodePanic(FilterTreeNode<InvList>* leaf,
			   const Query& query,
			   vector<unsigned>& resultStringIDs);
  
  void postProcess(const Query& query,
		   const vector<unsigned>& candidates, 
		   vector<unsigned>& resultStringIDs);

  void processLeaves(const vector<FilterTreeNode<InvList>*>& leaves,
		     const Query& query,
		     const vector<unsigned>& queryGramCodes,
		     vector<unsigned>& resultStringIDs) {
    static_cast<FtSearcherConcrete*>(this)->processLeaves_Impl(leaves, 
							       query, 
							       queryGramCodes,
							       resultStringIDs);
  }
  
public:
  FtSearcherAbs(Merger* merger, FtIndexer* ftIndexer = NULL);
    
  void prepare() { static_cast<FtSearcherConcrete*>(this)->prepare_Impl(); }
  void search(const Query& query, vector<unsigned>& resultStringIDs);
  void setFtIndexer(FtIndexer* ftIndexer) { 
    this->ftIndexer = ftIndexer; if(ftIndexer) gramGen = ftIndexer->getGramGen(); 
  }

  string getName() {
    return "FtSearcherAbs";
  }
  
#ifdef DEBUG_STAT
  QueryStats* getQueryStats() { return queryStats; }
#endif
  
  virtual ~FtSearcherAbs() {
#ifdef DEBUG_STAT
    if(queryStats) delete queryStats;
#endif
  }
};

template<class FtSearcherConcrete>
FtSearcherAbs<FtSearcherConcrete>::
FtSearcherAbs(Merger* merger, FtIndexer* ftIndexer)
  : merger(merger), ftIndexer(ftIndexer) {
  if(ftIndexer) gramGen = ftIndexer->getGramGen();
  else gramGen = NULL;
}

template<class FtSearcherConcrete>
void 
FtSearcherAbs<FtSearcherConcrete>::
getAffectedLeavesRec(FilterTreeNode<InvList>* node,
		     const Query& query,
		     const vector<unsigned>& queryGramCodes,
		     const vector<AbstractFilter*>* filterTypes,
		     const unsigned filterId,
		     vector<FilterTreeNode<InvList>*>& leaves) {
  
  if(filterTypes->size() == 0) {
    leaves.push_back( node->children.at(0).node );
    return;
  }
  
  if(node->isLeaf) leaves.push_back(node);
  else {
    AbstractFilter* filter = filterTypes->at(filterId);
    
    // get query bounds for this filter
    unsigned lbound, ubound;
    query.sim.getFilterBounds(query.str, query.threshold, filter, lbound, ubound);
    
    unsigned nodeLbound = filter->getFilterLbound();
    
    // binary search to first affected child
    vector<KeyNodePair<InvList> >& children = node->children;
    typename vector<KeyNodePair<InvList> >::iterator iter;
    KeyNodePair<InvList> lboundKnp(lbound, NULL);
    iter = lower_bound(children.begin(), children.end(), lboundKnp);
    
    // continue recursing into children as long as their keys overlap with lbound, ubound
    while(iter != children.end() && nodeLbound < ubound) {
      getAffectedLeavesRec(iter->node, query, queryGramCodes, filterTypes, filterId+1, leaves);
      nodeLbound = iter->key;
      iter++;
    }    
  }
}

template<class FtSearcherConcrete>
void 
FtSearcherAbs<FtSearcherConcrete>::
searchLeafNodePanic(FilterTreeNode<InvList>* leaf,
		    const Query& query,
		    vector<unsigned>& resultStringIDs) { 
  
#ifdef DEBUG_STAT
  this->sutil.startTimeMeasurement();
#endif
  
  if(ftIndexer->filterTypes.size() > 0) {
    // search the stringids in this leaf if there are any
    if(leaf->distinctStringIDs) {
      string dictString;
      Array<unsigned>* stringIDs = leaf->distinctStringIDs->getArray( this->ftIndexer->getInvListsFile() );
      for(unsigned i = 0; i < stringIDs->size(); i++) {
	unsigned stringId = stringIDs->at(i);      
	ftIndexer->strContainer->retrieveString(dictString, stringId);
	if ( query.sim(dictString, query.str, query.threshold))
	  resultStringIDs.push_back(stringId);
      }
      leaf->distinctStringIDs->clear();
    }
  }
  else {
    // search all stringids
    string dictString;
    for(unsigned i = 0; i < ftIndexer->strContainer->size(); i++) {    
      ftIndexer->strContainer->retrieveString(dictString, i);
      if ( query.sim(dictString, query.str, query.threshold))
	resultStringIDs.push_back(i);
    }
  }
  
#ifdef DEBUG_STAT
  this->sutil.stopTimeMeasurement();
  this->queryStats->panicTime += this->sutil.getTimeMeasurement();
  this->queryStats->panics++;
#endif
}

template<class FtSearcherConcrete>
void 
FtSearcherAbs<FtSearcherConcrete>::
search(const Query& query, vector<unsigned>& resultStringIDs) {
  
  // create gram codes from query
  vector<unsigned> queryGramCodes;
  gramGen->decompose(query.str, queryGramCodes);
  vector<AbstractFilter*>* filterTypes = &ftIndexer->filterTypes;
  
#ifdef DEBUG_STAT
  this->queryStats->reset();
  this->sutil.startTimeMeasurement();
#endif

  mergeThreshold = query.sim.getMergeThreshold(query.str, 
					       queryGramCodes, 
					       query.threshold,
					       ftIndexer->getCompressionArgs());
#ifdef DEBUG_STAT
  this->sutil.stopTimeMeasurement();
  this->queryStats->threshTime += this->sutil.getTimeMeasurement();
#endif    
  
  // get affected leaves and process them
  vector<FilterTreeNode<InvList>*> leaves;
  getAffectedLeavesRec(ftIndexer->filterTreeRoot,
		       query,
		       queryGramCodes,
		       filterTypes,
		       0,
		       leaves);
  
  processLeaves(leaves,
		query,
		queryGramCodes,
		resultStringIDs);
  
#ifdef DEBUG_STAT
  this->queryStats->totalTime = 
    this->queryStats->threshTime +
    this->queryStats->preprocTime +
    this->queryStats->mergeTime +
    this->queryStats->postprocTime +
    this->queryStats->panicTime;
#endif
}

template<class FtSearcherConcrete>
void 
FtSearcherAbs<FtSearcherConcrete>::
postProcess(const Query& query,
	    const vector<unsigned>& candidates, 
	    vector<unsigned>& resultStringIDs) {
  
#ifdef DEBUG_STAT
  this->sutil.startTimeMeasurement();
#endif

  for(unsigned i = 0; i < candidates.size(); i++) {
    unsigned stringId = candidates.at(i);
    string dictString;
    this->ftIndexer->strContainer->retrieveString(dictString, stringId);
    if (query.sim(dictString, query.str, query.threshold))
      resultStringIDs.push_back(stringId);
  }        

#ifdef DEBUG_STAT
  this->sutil.stopTimeMeasurement();
  this->queryStats->postprocTime += this->sutil.getTimeMeasurement();
#endif
}

#endif
