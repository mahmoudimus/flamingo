/*
  $Id: ftindexerabs.h 5229 2010-05-11 05:44:03Z abehm $

  Copyright (C) 2010 by The Regents of the University of California
	
  Redistribution of this file is permitted under
  the terms of the BSD license.
    
  Date: 09/17/2007
  Author: Alexander Behm <abehm (at) ics.uci.edu>
*/

#ifndef _ftindexerabs_h_
#define _ftindexerabs_h_

#include <cmath>

#include "filtertreenode.h"
#include "statsutil.h"
#include "gramlist.h"
#include "gramlistsimple.h"
#include "stringcontainer.h"
#include "common/filtertypes.h"
#include "common/gramgen.h"
#include "common/compressionargs.h"

enum FtIndexerType {
  UNCOMPRESSED,
  HOLES_GLOBAL_LLF,
  HOLES_GLOBAL_SLF,
  HOLES_GLOBAL_RANDOM,
  HOLES_GLOBAL_TIMECOST,
  HOLES_GLOBAL_PANICCOST,
  ONDISK
};

enum CharsumFilterOptions {
  CSF_NONE,
  CSF_REG,
  CSF_OPT
};

template<class FtIndexerConcrete>
struct FtIndexerTraits;

template<class FtIndexerConcrete> 
class FtIndexerAbs {
public:
  typedef FtIndexerTraits<FtIndexerConcrete> TraitsType;
  typedef typename TraitsType::StringContainer StringContainer;
  typedef typename TraitsType::InvList InvList;
  
protected:
#ifdef DEBUG_STAT
  IndexStats* ixStats;
  StatsUtil sutil;
#endif
  
  typedef typename tr1::unordered_map<unsigned, GramList<InvList>* > GramMap;

  FtIndexerType indexerType;
  GramGen* gramGen;
  CompressionArgs compressionArgs;  
  unsigned maxStrLen;
  unsigned ftFanout;  
  unsigned nextStringID;
  
  void buildHollowTreeRecursive(FilterTreeNode<InvList>* node, unsigned filterId);
  void deleteFilterTreeRec(FilterTreeNode<InvList>* node);
  
  void setCharsumFilter(CharsumFilterOptions csfOpts);
  
  // for saving/loading indexes
  void saveBasicInfo(ofstream& fpOut);
  void loadBasicInfo(ifstream& fpIn);
  
public:
  StringContainer* strContainer;
  vector<AbstractFilter*> filterTypes;
  FilterTreeNode<InvList>* filterTreeRoot;
  CharsumFilter* charsumFilter;
  CharsumFilterOptions csfOpts;
  StrCharsumStats* charsumStats;  
  
  FtIndexerAbs(StringContainer* strContainer, CharsumFilterOptions csfOpts = CSF_NONE);
  FtIndexerAbs(StringContainer* strContainer, 
	       GramGen* gramGenerator, 
	       unsigned maxStrLen = 150, 
	       unsigned ftFanout = 50,
	       CharsumFilterOptions csfOpts = CSF_NONE);
  
  void buildIndex(const string& dataFile, unsigned linesToRead = 0) {
    static_cast<FtIndexerConcrete*>(this)->buildIndex_Impl(dataFile, linesToRead);
  }
  
  void buildIndex() {
    static_cast<FtIndexerConcrete*>(this)->buildIndex_Impl();
  }
  
  void autoAddPartFilter();
  void addPartFilter(const AbstractFilter* filter);
  void clearPartFilters();
    
  GramGen* getGramGen() const { return gramGen; }
  void deleteFilterTree() {
    deleteFilterTreeRec(filterTreeRoot);
    filterTreeRoot = NULL;    
  } 

  // should be implemented by all indexers, too
  string getName() { 
    return "FtIndexerAbs"; 
  }
  
  // for compressed indexes only
  CompressionArgs* getCompressionArgs() { 
    return static_cast<FtIndexerConcrete*>(this)->getCompressionArgs_Impl(); 
  }

  // for disk-based indexes
  fstream* getInvListsFile() { 
    return static_cast<FtIndexerConcrete*>(this)->getInvListsFile_Impl();
  }
    
  FtIndexerType getType() { return indexerType; }
  
#ifdef DEBUG_STAT
  IndexStats* getIndexStats() { return ixStats; }
#endif

  ~FtIndexerAbs() {
    deleteFilterTree();
    clearPartFilters();
    if(charsumStats) delete[] charsumStats;
    if(charsumFilter) delete charsumFilter;
#ifdef DEBUG_STAT
    if(this->ixStats) delete this->ixStats;
#endif
  }  
};

template<class FtIndexerConcrete> 
void 
FtIndexerAbs<FtIndexerConcrete>::
buildHollowTreeRecursive(FilterTreeNode<InvList>* node, unsigned filterId) {

  // special case: no partitioning filters used, just create one child
  if(filterTypes.size() == 0) {
    FilterTreeNode<InvList>* newNode = new FilterTreeNode<InvList>(true);    
    node->children.push_back( KeyNodePair<InvList>(0, newNode) );
    return;
  }
  
  unsigned filterLbound, filterUbound;
  AbstractFilter* filter = filterTypes.at(filterId);
  filterLbound = filter->getFilterLbound();
  filterUbound = filter->getFilterUbound();
  if(filter->getType() == FT_CHARSUM) filterUbound = 15600; // google
  //if(filter->getType() == FT_CHARSUM) filterUbound = 24900; // pubmed
  double step = (filterUbound - filterLbound) / (double)ftFanout;  
  bool isLeaf = filterId >= filterTypes.size()-1;
  for(unsigned i = 0; i < ftFanout; i++) {
    FilterTreeNode<InvList>* newNode = new FilterTreeNode<InvList>(isLeaf);
    unsigned key = (unsigned)round((i+1)*step);
    node->children.push_back( KeyNodePair<InvList>(key, newNode) );
    if(!isLeaf)
      buildHollowTreeRecursive(newNode, filterId+1);
  }
}

// used for reading/writing index from/to file, 
// using logical separation into functions so compressed indexers can use these, too
template<class FtIndexerConcrete> 
void 
FtIndexerAbs<FtIndexerConcrete>::
saveBasicInfo(ofstream& fpOut) {
  unsigned u;
  
  // write primitive information
  fpOut.write((const char*)&this->maxStrLen, sizeof(unsigned));
  fpOut.write((const char*)&this->ftFanout, sizeof(unsigned));
  fpOut.write((const char*)&this->nextStringID, sizeof(unsigned));
  
  // write charsum info
  fpOut.write((const char*)&this->csfOpts, sizeof(CharsumFilterOptions));
  if(this->csfOpts != CSF_NONE) {
    unsigned contSize = strContainer->size();
    fpOut.write((const char*)&contSize, sizeof(unsigned));
    fpOut.write((const char*)this->charsumStats, sizeof(StrCharsumStats) * contSize);
  }
  
  // write gramgenerator
  this->gramGen->saveGramGenInstance(fpOut);
  
  // write filterTypes
  u = this->filterTypes.size();
  fpOut.write((const char*)&u, sizeof(unsigned));
  for(unsigned i = 0; i < this->filterTypes.size(); i++) {
    AbstractFilter* filter = this->filterTypes.at(i);
    filter->saveFilterInstance(fpOut);
  }  
}

template<class FtIndexerConcrete> 
void 
FtIndexerAbs<FtIndexerConcrete>::
loadBasicInfo(ifstream& fpIn) {
  unsigned nFilters;
  
  fpIn.read((char*)&this->maxStrLen, sizeof(unsigned));
  fpIn.read((char*)&this->ftFanout, sizeof(unsigned));
  fpIn.read((char*)&this->nextStringID, sizeof(unsigned));  
  
  // load charsum info
  fpIn.read((char*)&this->csfOpts, sizeof(CharsumFilterOptions));
  if(this->csfOpts != CSF_NONE) {
    unsigned contSize = 0;
    fpIn.read((char*)&contSize, sizeof(unsigned));
    if(this->charsumStats) delete[] this->charsumStats;
    this->charsumStats = new StrCharsumStats[contSize];
    fpIn.read((char*)this->charsumStats, sizeof(StrCharsumStats) * contSize);
  }
  //setCharsumFilter(csfOpts);
  
  // load gramgenerator
  this->gramGen = GramGen::loadGramGenInstance(fpIn);  
  
  // load filters
  fpIn.read((char*)&nFilters, sizeof(unsigned));
  for(unsigned i = 0; i < nFilters; i++) {
    AbstractFilter* newFilter = AbstractFilter::loadFilterInstance(fpIn);
    this->filterTypes.push_back(newFilter);
  }
}

template<class FtIndexerConcrete> 
void 
FtIndexerAbs<FtIndexerConcrete>::
setCharsumFilter(CharsumFilterOptions csfOpts) {
  this->csfOpts = csfOpts;
  if(strContainer->size() > 0) {
    switch(csfOpts) {
      
    case CSF_REG: {
      charsumFilter = new CharsumFilter((unsigned*)NULL);
      charsumStats = new StrCharsumStats[strContainer->size()];
    } break;
      
    case CSF_OPT: {
      if(strContainer->getStatsCollector()) {
	charsumFilter = new CharsumFilter(strContainer->getStatsCollector()->getCharFreqs());
	charsumStats = new StrCharsumStats[strContainer->size()];
      }
      else {
	cout << "WARNING: charsum filter disabled because stringcontainer has no stats" << endl;
	charsumFilter = new CharsumFilter((unsigned*)NULL);
	charsumStats = new StrCharsumStats[strContainer->size()];
      }
    } break;
      
    case CSF_NONE: {
      charsumFilter = NULL;
      charsumStats = NULL;
    } break;

    }
  }
  else {
    charsumFilter = NULL;
    charsumStats = NULL;
  }
}

template<class FtIndexerConcrete> 
void 
FtIndexerAbs<FtIndexerConcrete>::
deleteFilterTreeRec(FilterTreeNode<InvList>* node) {
  if(!node) return;
  if(!node->isLeaf) {
    vector<KeyNodePair<InvList> >& children = node->children;
    for(unsigned i = 0; i < children.size(); i++) {
      deleteFilterTreeRec(children.at(i).node);
    }
  }
  delete node;
}

template<class FtIndexerConcrete> 
FtIndexerAbs<FtIndexerConcrete>::
FtIndexerAbs(StringContainer* strContainer, CharsumFilterOptions csfOpts)
  : gramGen(NULL), maxStrLen(0), ftFanout(0), nextStringID(0), strContainer(strContainer),  
    filterTreeRoot(new FilterTreeNode<InvList>(false)) {
#ifdef DEBUG_STAT
  this->ixStats = NULL;
#endif
  setCharsumFilter(csfOpts);
}

template<class FtIndexerConcrete> 
FtIndexerAbs<FtIndexerConcrete>::
FtIndexerAbs(StringContainer* strContainer, GramGen* gramGen, unsigned maxStrLen, unsigned ftFanout, CharsumFilterOptions csfOpts) 
  : gramGen(gramGen), maxStrLen(maxStrLen), ftFanout(ftFanout), nextStringID(0), strContainer(strContainer),  
    filterTreeRoot(new FilterTreeNode<InvList>(false)) {
#ifdef DEBUG_STAT
  this->ixStats = NULL;
#endif
  setCharsumFilter(csfOpts);
}

template<class FtIndexerConcrete> 
void
FtIndexerAbs<FtIndexerConcrete>::
autoAddPartFilter() {
  
  clearPartFilters();
  
  StatsCollector* statsCollector = strContainer->getStatsCollector();
  if(statsCollector) {
    FilterStats* bestFilterStats = statsCollector->getBestPartFilter();
    AbstractFilter* bestFilter = NULL;

    switch(bestFilterStats->getFilterType()) {
      
    case FT_LENGTH: {
      bestFilter = new LengthFilter(maxStrLen);
    } break;
      
    case FT_CHARSUM: {
      bestFilter = new CharsumFilter(maxStrLen);
    } break;

    case FT_NONE: break;    
      
    }
    
    if(bestFilter) {
      bestFilter->adjustBounds(bestFilterStats->getMinKey(), bestFilterStats->getMaxKey(), ftFanout);
      addPartFilter(bestFilter);    
    }
  }
  else {
    cout << "WARNING: no partitioning filter set because container has no stats" << endl;    
  }
}

template<class FtIndexerConcrete> 
void 
FtIndexerAbs<FtIndexerConcrete>::
addPartFilter(const AbstractFilter* filter) {
  for(unsigned i = 0; i < filterTypes.size(); i++) {
    AbstractFilter* tmp = filterTypes.at(i);
    if(filter->getType() == tmp->getType()) return;
  }
  filterTypes.push_back((AbstractFilter*)filter);    
}

template<class FtIndexerConcrete> 
void 
FtIndexerAbs<FtIndexerConcrete>::
clearPartFilters() {
  for(unsigned i = 0; i < filterTypes.size(); i++) {
    AbstractFilter* tmp = filterTypes.at(i);
    if(tmp) delete tmp;
  }
  filterTypes.clear();
}

#endif
