/*
  $Id: ftindexermemabs.h 5146 2010-03-24 23:05:57Z abehm $

  Copyright (C) 2010 by The Regents of the University of California
	
  Redistribution of this file is permitted under
  the terms of the BSD license.
    
  Date: 04/04/2008
  Author: Alexander Behm <abehm (at) ics.uci.edu>
*/

#ifndef _ftindexermemabs_h_
#define _ftindexermemabs_h_

#include "ftindexerabs.h"
#include "gramlistsimple.h"
#include "util/debug.h"

template<class FtIndexerConcrete>
class FtIndexerMemAbs;

template<class FtIndexerConcrete>
struct FtIndexerTraits<FtIndexerMemAbs<FtIndexerConcrete> > {
  typedef FtIndexerTraits<FtIndexerConcrete> TraitsType;
  typedef typename TraitsType::StringContainer StringContainer;
  typedef typename TraitsType::InvList InvList;
};

template<class FtIndexerConcrete>
class FtIndexerMemAbs 
  : public FtIndexerAbs<FtIndexerMemAbs<FtIndexerConcrete> > {
  
public:
  typedef FtIndexerTraits<FtIndexerConcrete> TraitsType;
  typedef typename TraitsType::StringContainer StringContainer;
  typedef typename TraitsType::InvList InvList;
  
protected:  
  void addStringToNode(FilterTreeNode<InvList>* node, const string& s, const unsigned stringId);

  typedef 
  FtIndexerAbs<FtIndexerMemAbs<FtIndexerConcrete> >
  SuperClass;  
  
  // used for reading/writing index from/to file, 
  // using logical separation into functions so compressed indexers can use these, too
  void saveLeavesRec(FilterTreeNode<InvList>* node, ofstream& fpOut);
  void loadLeavesRec(FilterTreeNode<InvList>* node, ifstream& fpIn);
    
public:
  typedef typename SuperClass::GramMap GramMap;

  FtIndexerMemAbs(StringContainer* strContainer) : SuperClass(strContainer) {}
  FtIndexerMemAbs(StringContainer* strContainer, 
		  GramGen* gramGenerator, 
		  const unsigned maxStrLen = 150, 
		  const unsigned ftFanout = 50) 
    : SuperClass(strContainer, gramGenerator, maxStrLen, ftFanout) {}

  // here are the statically resolved interfaces that a concrete indexer MUST implement
  // they use the CRTP design idiom
  void prepare() {
    static_cast<FtIndexerConcrete*>(this)->prepare_Impl();
  }

  void addStringId(FilterTreeNode<InvList>* leaf, const vector<unsigned>& gramCodes, unsigned stringId) {
    static_cast<FtIndexerConcrete*>(this)->addStringId_Impl(leaf, gramCodes, stringId);
  }

  void saveIndex(const char* indexFileName) {
    static_cast<FtIndexerConcrete*>(this)->saveIndex_Impl(indexFileName);
  }
  
  void loadIndex(const char* indexFileName) {
    static_cast<FtIndexerConcrete*>(this)->loadIndex_Impl(indexFileName);
  }
  
  CompressionArgs* getCompressionArgs() { 
    return static_cast<FtIndexerConcrete*>(this)->getCompressionArgs_Impl(); 
  }
  
  string getName() { 
    return "FtIndexerMemAbs"; 
  }
  
  fstream* getInvListsFile_Impl() { 
    return NULL;
  }

  void buildIndex_Impl(const string& dataFile, const unsigned linesToRead = 0);
  void buildIndex_Impl();

  void printFilterTree(FilterTreeNode<InvList>* node = NULL);
};

template<class FtIndexerConcrete> 
void 
FtIndexerMemAbs<FtIndexerConcrete>::
addStringToNode(FilterTreeNode<InvList>* node, const string& s, const unsigned stringId) {
  // add stringId to distinctStringIDs and create the GramListSimple instance if neccessary
  if(!node->distinctStringIDs) node->distinctStringIDs = new GramListSimple<Array<unsigned> >();
  node->distinctStringIDs->getArray()->append(stringId);
  
  // add stringId to gramMap of leaf
  vector<unsigned> gramCodes;
  this->gramGen->decompose(s, gramCodes);    
  addStringId(node, gramCodes, stringId);
}

template<class FtIndexerConcrete> 
void
FtIndexerMemAbs<FtIndexerConcrete>:: 
buildIndex_Impl(const string& dataFile, const unsigned linesToRead) {  
  this->strContainer->fillContainer(dataFile.c_str(), linesToRead, this->maxStrLen);
  this->buildIndex();
}

template<class FtIndexerConcrete> 
void 
FtIndexerMemAbs<FtIndexerConcrete>::
buildIndex_Impl() {

#ifdef DEBUG_STAT  
  this->ixStats->dictSize = this->strContainer->size();
  this->ixStats->gramLen = this->gramGen->getGramLength();
  this->ixStats->maxStrLen = this->maxStrLen;
  this->ixStats->ftFanout = this->ftFanout;
  this->ixStats->partFilters = this->filterTypes.size();
  this->sutil.startTimeMeasurement();
#endif
  
  // prepare compressor for building index
  prepare();
  
  // build hollow tree, i.e. create nodes and leaves without filling inverted lists  
  if(this->filterTypes.size() > 0 && this->ftFanout > 0) {
    this->buildHollowTreeRecursive(this->filterTreeRoot, 0);
    
    // add stringIds to tree
    TIMER_START("INSERTING INTO INDEX", this->strContainer->size());
    for(unsigned stringId = 0; stringId < this->strContainer->size(); stringId++) {
      FilterTreeNode<InvList>* currentNode = this->filterTreeRoot;
      string currentString;
      this->strContainer->retrieveString(currentString, stringId);
      if(currentString.empty()) continue;
      
      // position currentNode to appropriate leaf node in filter tree      
      for(unsigned filterId = 0; filterId < this->filterTypes.size(); filterId++) {
	unsigned key = this->filterTypes.at(filterId)->getKey(currentString);	
	KeyNodePair<InvList> knp(key, NULL);	
	typename vector<KeyNodePair<InvList> >::iterator iter = 
	  lower_bound(currentNode->children.begin(), currentNode->children.end(), knp);
	if(iter != currentNode->children.end()) currentNode = iter->node;	
      }      
      addStringToNode(currentNode, currentString, stringId);
      TIMER_STEP();
    }  
    TIMER_STOP();
  }
  else {
    // no filters selected, so just create one leaf node to store all inverted lists
    FilterTreeNode<InvList>* n = new FilterTreeNode<InvList>(true);
    this->filterTreeRoot->children.push_back( KeyNodePair<InvList>(0, n) );
    
    TIMER_START("INSERTING INTO INDEX", this->strContainer->size());
    for(unsigned stringId = 0; stringId < this->strContainer->size(); stringId++) {
      string currentString;
      this->strContainer->retrieveString(currentString, stringId);
      if(currentString.empty()) continue;      
      addStringToNode(n, currentString, stringId);
      TIMER_STEP();
    }
    TIMER_STOP();
  }
  
  // set next stringID
  this->nextStringID = this->strContainer->size();
  
#ifdef DEBUG_STAT
  this->sutil.stopTimeMeasurement();
  this->ixStats->buildTime = this->sutil.getTimeMeasurement();
#endif
}

template<class FtIndexerConcrete> 
void 
FtIndexerMemAbs<FtIndexerConcrete>::
saveLeavesRec(FilterTreeNode<InvList>* node, ofstream& fpOut) {
  unsigned u;
  
  if(node->isLeaf) {
    if(node->distinctStringIDs) {
      InvList* distinctArr = node->distinctStringIDs->getArray();
      // write distinct string ids
      u = distinctArr->size();           
      fpOut.write((const char*)&u, sizeof(unsigned));
      for(unsigned i = 0; i < distinctArr->size(); i++) {
	u = distinctArr->at(i);
	fpOut.write((const char*)&u, sizeof(unsigned));      
      }      
    }
    else {
      u = 0;
      fpOut.write((const char*)&u, sizeof(unsigned));
    }
    
    // write gram map
    GramMap& gramMap = node->gramMap;
    u = gramMap.size();
    fpOut.write((const char*)&u, sizeof(unsigned));
    for(typename GramMap::iterator iter = gramMap.begin();     
	iter != gramMap.end();
	iter++) {
      
      u = iter->first;
      fpOut.write((const char*)&u, sizeof(unsigned));
      
      InvList* arr = iter->second->getArray();
      if(arr) {
	u = arr->size();
	fpOut.write((const char*)&u, sizeof(unsigned));
	for(unsigned i = 0; i < arr->size(); i++) {
	  u = arr->at(i);
	  fpOut.write((const char*)&u, sizeof(unsigned));
	}
      }
      else {
	u = 0;
	fpOut.write((const char*)&u, sizeof(unsigned));
      }      
    }    
  }
  else {
    vector<KeyNodePair<InvList> >& children = node->children;
    for(unsigned i = 0; i < children.size(); i++)
      saveLeavesRec(children.at(i).node, fpOut);
  }
}

template<class FtIndexerConcrete> 
void 
FtIndexerMemAbs<FtIndexerConcrete>::
loadLeavesRec(FilterTreeNode<InvList>* node, ifstream& fpIn) {
  if(node->isLeaf) {
    if(node->distinctStringIDs) delete node->distinctStringIDs;
    unsigned distinctStrings;
    fpIn.read((char*)&distinctStrings, sizeof(unsigned));   
    
    if(distinctStrings > 0) node->distinctStringIDs = new GramListSimple<InvList>();
    for(unsigned i = 0; i < distinctStrings; i++) {
      unsigned tmp;
      fpIn.read((char*)&tmp, sizeof(unsigned));
      node->distinctStringIDs->getArray()->append(tmp);
    }    
    
    GramMap& gramMap = node->gramMap;
    unsigned gramMapSize;
    unsigned gramCode;
    unsigned arrSize;
    fpIn.read((char*)&gramMapSize, sizeof(unsigned));
    for(unsigned j = 0; j < gramMapSize; j++) {
      fpIn.read((char*)&gramCode, sizeof(unsigned));
      fpIn.read((char*)&arrSize, sizeof(unsigned));
      
      if(arrSize != 0) {	
	GramListSimple<InvList>* gramList = new GramListSimple<InvList>();
	gramMap[gramCode] = gramList;
	InvList* newArr = gramList->getArray();
	for(unsigned i = 0; i < arrSize; i++) {
	  unsigned tmp;
	  fpIn.read((char*)&tmp, sizeof(unsigned));
	  newArr->append(tmp);		
	}
      }
    }
  }
  else {
    vector<KeyNodePair<InvList> >& children = node->children;
    for(unsigned i = 0; i < children.size(); i++)
      loadLeavesRec(children.at(i).node, fpIn);
  } 
}

template<class FtIndexerConcrete> 
void
FtIndexerMemAbs<FtIndexerConcrete>::
printFilterTree(FilterTreeNode<InvList>* node) {
  if(!node) {
    printFilterTree(this->filterTreeRoot);
    return;
  }
  
  if(node->isLeaf()) {
    GramMap& gramMap = node->gramMap;
    
    for(typename GramMap::iterator iter = gramMap.begin();
	iter != gramMap.end();
	iter++) {
      
      Array<unsigned>* arr = iter->second->getArray();
      cout << iter->first << " ";
      if(arr) {
	for(unsigned i = 0; i < arr->size(); i++)
	  cout << arr->at(i) << " ";
      }
      cout << endl;
    }
  }
  else {
    vector<pair<unsigned, FilterTreeNode<InvList>*> >& children = node->children;
    for(unsigned i = 0; i < children.size(); i++) {
      cout << children.at(i).first << endl;
      printFilterTree(children.at(i).second);
    }
  }
}

#endif
