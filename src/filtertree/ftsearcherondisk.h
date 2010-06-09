/*
  $Id: ftsearcherondisk.h 5229 2010-05-11 05:44:03Z abehm $

  Copyright (C) 2010 by The Regents of the University of California
	
  Redistribution of this file is permitted under
  the terms of the BSD license.
    
  Date: 09/06/2008
  Author: Alexander Behm <abehm (at) ics.uci.edu>
*/

#ifndef _ftsearcherondisk_h_
#define _ftsearcherondisk_h_

#include "ftsearcherabs.h"
#include "ftindexerondisk.h"
#include "listmerger/ondiskmergersimple.h"
#include "listmerger/ondiskmergeradapt.h"
#include "util/misc.h"

template<class FtIndexer, class Merger>
class FtSearcherOnDisk;

template<class TFtIndexer, class TMerger>
struct FtSearcherTraits<FtSearcherOnDisk<TFtIndexer, TMerger> > {
  typedef TFtIndexer FtIndexer;
  typedef TMerger Merger;
};

template<class FtIndexer = FtIndexerOnDisk<>, class Merger = OnDiskMergerSimple<> >
class FtSearcherOnDisk : public FtSearcherAbs<FtSearcherOnDisk<FtIndexer, Merger> > {

private:
  void setMergerEstimationParams(unsigned numberListProbes, unsigned numberStringProbes, bool clearBuffers = true);
  void clearOSBuffers();
  
  static StringContainerRM* strRM;
  static bool cmpCandis(unsigned a, unsigned b) { 
    bool aCached = strRM->inCache(a);
    bool bCached = strRM->inCache(b);
    if(aCached && bCached) return a < b; 
    else if(aCached) return true;
    else if(bCached) return false;
    else return a < b;
  }
  
public:
  typedef FtIndexerTraits<FtIndexer> IndexerTraitsType;
  typedef typename IndexerTraitsType::InvList InvList;
  typedef FtSearcherAbs<FtSearcherOnDisk<FtIndexer, Merger> > SuperClass;
  
#ifdef DEBUG_STAT
  typedef OnDiskWorkloadStats WlStatsType;
  typedef OnDiskQueryStats QueryStatsType;
#endif

  FtSearcherOnDisk(Merger* merger, FtIndexer* ftIndexer = NULL) : SuperClass(merger, ftIndexer) {
#ifdef DEBUG_STAT
    this->queryStats = new QueryStatsType();
#endif
  }
  
  void prepare_Impl();

  void processLeaves_Impl(const vector<FilterTreeNode<InvList>*>& leaves,
			  const Query& query,
			  const vector<unsigned>& queryGramCodes,
			  vector<unsigned>& resultStringIDs);

  void searchAllLeafNodesPanic(const vector<FilterTreeNode<InvList>*>& leaves,
			       const Query& query,
			       vector<unsigned>& resultStringIDs);

  string getName() {
    return "FtSearcherOnDisk";
  }
};

template<class FtIndexer, class Merger>
StringContainerRM* FtSearcherOnDisk<FtIndexer, Merger>::strRM;

template<class FtIndexer, class Merger>
void
FtSearcherOnDisk<FtIndexer, Merger>::
searchAllLeafNodesPanic(const vector<FilterTreeNode<InvList>*>& leaves,
			const Query& query,
			vector<unsigned>& resultStringIDs) {
  
#ifdef DEBUG_STAT
  this->sutil.startTimeMeasurement();  
#endif
  
  if(this->ftIndexer->filterTypes.size() > 0) {
    
    vector<unsigned> candidates;
    for(unsigned i = 0; i < leaves.size(); i++) {
      FilterTreeNode<InvList>* leaf = leaves.at(i);      

      // add the stringids in this leaf if there are any
      if(leaf->distinctStringIDs) {
	Array<unsigned>* stringIDs = leaf->distinctStringIDs->getArray( this->ftIndexer->getInvListsFile() );	
	for(unsigned j = 0; j < stringIDs->size(); j++) {
	  if(this->ftIndexer->csfOpts != CSF_NONE) {
	    if(this->ftIndexer->charsumFilter->passesFilter(this->merger->queryCharsumStats, &this->merger->charsumStats[stringIDs->at(j)], (unsigned)query.threshold)) {
	      candidates.push_back(stringIDs->at(j));
	    }
	  }
	  else {
	    candidates.push_back(stringIDs->at(j));
	  }	  
	}
	leaf->distinctStringIDs->clear();
      }      
    }   
    
    if(leaves.size() > 1) sort(candidates.begin(), candidates.end());

    // retrieve strings and compute similarities
    for(unsigned i = 0; i < candidates.size(); i++) {
      unsigned stringId = candidates.at(i);
      string dictString;
      this->ftIndexer->strContainer->retrieveString(dictString, stringId);
      if ( query.sim(dictString, query.str, query.threshold) )
	resultStringIDs.push_back(stringId);
    }
  }
  else {
    // search all stringids
    string dictString;
    for(unsigned i = 0; i < this->ftIndexer->strContainer->size(); i++) {    
      this->ftIndexer->strContainer->retrieveString(dictString, i);
      if ( query.sim(dictString, query.str, query.threshold) )
	resultStringIDs.push_back(i);
    }
  }
  
#ifdef DEBUG_STAT
  this->sutil.stopTimeMeasurement();
  this->queryStats->panicTime += this->sutil.getTimeMeasurement();
  this->queryStats->panics++;
#endif
}

template<class FtIndexer, class Merger>
void
FtSearcherOnDisk<FtIndexer, Merger>::
prepare_Impl() {
  
  // PUBMED DATASET without caching
  //if(this->merger->estimationParamsNeeded())
  //  this->merger->setEstimationParams(0.000119582f, 19.5736f, 28.065f);
  
  // GOOGLE DATASET without caching
  //if(this->merger->estimationParamsNeeded()) 
  //  this->merger->setEstimationParams(8.79494e-05, 18.6529, 23.8775);

  // GOOGLE DATASET with caching
  if(this->merger->estimationParamsNeeded()) 
    this->merger->setEstimationParams(4.34583e-06, 0.00537823, 0.00711457);
  
  if(this->merger->estimationParamsNeeded() ) setMergerEstimationParams(5000, 5000, false);
  
  this->merger->setGramGen(this->gramGen);
  this->merger->charsumStats = this->ftIndexer->charsumStats;
  this->merger->charsumFilter = this->ftIndexer->charsumFilter;  
  this->merger->gramcode2gram = &this->ftIndexer->gramcode2gram;
  this->merger->numberSeeks = 0;
}

template<>
void
FtSearcherOnDisk<class FtIndexerOnDisk<StringContainerRM, Array<PosID> >, OnDiskMergerAdapt<Array<PosID> > >::
prepare_Impl();

template<class FtIndexer, class Merger>
void
FtSearcherOnDisk<FtIndexer, Merger>::
clearOSBuffers() {
  int i;
  i = system("sync");
  i = system("echo 3 > /proc/sys/vm/drop_caches");
}

template<class FtIndexer, class Merger>
void
FtSearcherOnDisk<FtIndexer, Merger>::
setMergerEstimationParams(unsigned numberListProbes, unsigned numberStringProbes, bool clearBuffers) {
  if(clearBuffers) clearOSBuffers();

  srand(150);
  
  StatsUtil sutil;
  
  vector<FilterTreeNode<InvList>* > leaves;
  this->ftIndexer->filterTreeRoot->getSubTreeLeaves(this->ftIndexer->filterTreeRoot, leaves);

  vector<GramListOnDisk<InvList>* > probeGramLists;
  for(unsigned i = 0; i < leaves.size(); i++) {
    FilterTreeNode<InvList>* leaf = leaves.at(i);
    typename FilterTreeNode<InvList>::GramMap::iterator iter;
    set<unsigned> sizesAdded;
    for(iter = leaf->gramMap.begin(); iter != leaf->gramMap.end(); iter++) {
      GramListOnDisk<InvList>* gl = dynamic_cast<GramListOnDisk<InvList>*>(iter->second);
      if(sizesAdded.find(gl->listSize) == sizesAdded.end()) {
	probeGramLists.push_back(gl);
	sizesAdded.insert(gl->listSize);
      }
    }
  }
  
  vector<float> xvals;
  vector<float> yvals;
  for(unsigned i = 0; i < numberListProbes; i++) {
    unsigned index = random() % probeGramLists.size();
    GramListOnDisk<InvList>* gl = probeGramLists.at(index);    
    unsigned bytes = gl->listSize * InvList::elementSize();
    char* data = new char[bytes];
    if(clearBuffers) clearOSBuffers();
    sutil.startTimeMeasurement();    
    this->ftIndexer->invListsFile.seekg(gl->startOffset);
    this->ftIndexer->invListsFile.read(data, bytes);
    sutil.stopTimeMeasurement();
    float t = sutil.getTimeMeasurement();
    delete[] data;    
    xvals.push_back(gl->listSize);
    yvals.push_back(t);
  }

  float slope;
  float intercept;
  linearRegression(xvals, yvals, slope, intercept);

  // now get average time for retrieving a string from the StringContainer
  float avgTime = 0.0f;
  for(unsigned i = 0; i < numberStringProbes; i++) {
    unsigned stringid = random() % this->ftIndexer->strContainer->size();    
    if(clearBuffers) clearOSBuffers();
    sutil.startTimeMeasurement();    
    string tmp;
    this->ftIndexer->strContainer->retrieveString(tmp, stringid);
    sutil.stopTimeMeasurement();
    avgTime += sutil.getTimeMeasurement();
  }
  avgTime /= numberStringProbes;
  
  // ensure the slope cannot be negative
  slope = max(slope, 0.0f);
  
  // set the estimation parameters
  this->merger->setEstimationParams(slope, intercept, avgTime);

  //cout << "SLOPE: " << slope << endl;
  //cout << "INTERCEPT: " << intercept << endl;
  //cout << "AVGTIME: " << avgTime << endl;

  if(clearBuffers) clearOSBuffers();
}

template<class FtIndexer, class Merger>
void
FtSearcherOnDisk<FtIndexer, Merger>::
processLeaves_Impl(const vector<FilterTreeNode<InvList>*>& leaves,
		   const Query& query,
		   const vector<unsigned>& queryGramCodes,
		   vector<unsigned>& resultStringIDs) {
  
  // gather query's charsum stats if needed
  if(this->ftIndexer->csfOpts != CSF_NONE) 
    this->merger->queryCharsumStats = new QueryCharsumStats(query.str, query.threshold, this->ftIndexer->charsumFilter);
    
  // Step 0: Check for panic
  if((signed)this->mergeThreshold <= 0) {    
    if(this->merger->singleFilterOpt) {
      searchAllLeafNodesPanic(leaves, query, resultStringIDs);
    }
    else {
      for(unsigned i = 0; i < leaves.size(); i++)
	this->searchLeafNodePanic(leaves.at(i), query, resultStringIDs);
    }  
    return;
  } 
  
#ifdef DEBUG_STAT
  this->queryStats->strLen = query.str.size();
  this->queryStats->mergeThresh = this->mergeThreshold;
  this->queryStats->simThresh = query.threshold;
#endif
  
  // set required info in merger if we have no filters
  if(this->ftIndexer->filterTypes.size() == 0)
    this->merger->numberStringsInLeaf.push_back(this->ftIndexer->strContainer->size());
  
  // prepare to pass information to the merger
  vector<vector<QueryGramList<InvList>* >* > allLists;  
  vector<unsigned> candidates;
    
  // Step 1: Preprocess
#ifdef DEBUG_STAT
  this->sutil.startTimeMeasurement();
#endif
  
  for(unsigned i = 0; i < leaves.size(); i++) {      
    vector<QueryGramList<InvList>* >* currentLists = new vector<QueryGramList<InvList>* >();
    leaves.at(i)->getQueryGramLists(queryGramCodes, currentLists);
    if(currentLists->size() >= this->mergeThreshold) {
      if(this->ftIndexer->filterTypes.size() > 0) {
	unsigned stringsInLeaf = 
	  dynamic_cast<GramListOnDisk<Array<unsigned> >*>(leaves.at(i)->distinctStringIDs)->listSize;
	this->merger->numberStringsInLeaf.push_back(stringsInLeaf);
      }
      allLists.push_back(currentLists);
    }
  }

#ifdef DEBUG_STAT
  this->sutil.stopTimeMeasurement();
  this->queryStats->preprocTime += this->sutil.getTimeMeasurement();
#endif
  
#ifdef DEBUG_STAT
  this->sutil.startTimeMeasurement();
#endif

  // Step 2.1: Merge
  this->merger->merge(query,
		      allLists, 
		      this->mergeThreshold, 
		      this->ftIndexer->invListsFile,
		      this->ftIndexer->filterTypes.size(),
		      candidates);
  
#ifdef DEBUG_STAT
  this->sutil.stopTimeMeasurement();
  this->queryStats->mergeTime += this->sutil.getTimeMeasurement();
  this->queryStats->candidates += candidates.size();
  dynamic_cast<QueryStatsType*>(this->queryStats)->numberSeeks += this->merger->numberSeeks;
#endif
  
  // sort candidates, first by whether their page is in the cache, second by stringId
  FtSearcherOnDisk<FtIndexer, Merger>::strRM = dynamic_cast<StringContainerRM*>(this->ftIndexer->strContainer);    
  sort(candidates.begin(), candidates.end(), FtSearcherOnDisk<FtIndexer, Merger>::cmpCandis);
  
#ifdef DEBUG_STAT
  // count the number of pages read
  set<unsigned> rids;
  for(unsigned i = 0; i < candidates.size(); i++) {
    RecordID* rid = this->ftIndexer->strContainer->getRecordID(candidates.at(i));
    rids.insert( rid->blockNum );
  } 
  dynamic_cast<QueryStatsType*>(this->queryStats)->postprocPages += rids.size();
#endif
  
  // Step 2.2: Postprocess  
  this->postProcess(query, candidates, resultStringIDs);
  
  // cleanup, delete temporary structures and inverted lists
  for(unsigned i = 0; i < allLists.size(); i++) {
    vector<QueryGramList<InvList>* >* currentLists = allLists.at(i);
    for(unsigned j = 0; j < currentLists->size(); j++) {
      dynamic_cast<GramListOnDisk<InvList>*>( currentLists->at(j)->gl )->clear();
      delete currentLists->at(j);
    }
    delete allLists.at(i);
  }
  
  if(this->ftIndexer->csfOpts != CSF_NONE)
    delete this->merger->queryCharsumStats;
}

#endif
