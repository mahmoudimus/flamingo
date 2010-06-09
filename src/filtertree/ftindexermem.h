/*
  $Id: ftindexermem.h 5146 2010-03-24 23:05:57Z abehm $

  Copyright (C) 2010 by The Regents of the University of California
	
  Redistribution of this file is permitted under
  the terms of the BSD license.
    
  Date: 04/04/2008
  Author: Alexander Behm <abehm (at) ics.uci.edu>
*/

#ifndef _ftindexeruncompressed_h_
#define _ftindexeruncompressed_h_

#include "ftindexermemabs.h"

template <class StringContainer, class InvList>
class FtIndexerMem;

template<class TStringContainer, class TInvList>
struct FtIndexerTraits<FtIndexerMem<TStringContainer, TInvList> > {
  typedef TStringContainer StringContainer;
  typedef TInvList InvList;
};

template <class StringContainer = StringContainerVector, class InvList = Array<unsigned> >
class FtIndexerMem 
  : public FtIndexerMemAbs<FtIndexerMem<StringContainer, InvList> > { 

private:
  typedef 
  FtIndexerMemAbs<FtIndexerMem<StringContainer, InvList> > 
  SuperClass;
  
public:
  typedef typename SuperClass::GramMap GramMap;

#ifdef DEBUG_STAT
  typedef IndexStats IxStatsType;
#endif

  FtIndexerMem(StringContainer* strContainer) : SuperClass(strContainer) {
#ifdef DEBUG_STAT
    this->ixStats = new IndexStats();
#endif
  }
  
  FtIndexerMem(StringContainer* strContainer,
	       GramGen* gramGenerator, 
	       unsigned maxStrLen = 150, 
	       unsigned maxChildNodes = 50) 
    : SuperClass(strContainer, gramGenerator, maxStrLen, maxChildNodes) {
#ifdef DEBUG_STAT
    this->ixStats = new IndexStats();
#endif    
  }

  void prepare_Impl() {};    
  void addStringId_Impl(FilterTreeNode<InvList>* leaf, const vector<unsigned>& gramCodes, unsigned stringId);
  CompressionArgs* getCompressionArgs_Impl() { return &(this->compressionArgs); }

  void saveIndex_Impl(const char* indexFileName);
  void loadIndex_Impl(const char* indexFileName);

  string getName() {
    return "FtIndexerMem";
  }
  
  void insertString(const string& s);
  void clearInvertedLists();
};

template<class StringContainer, class InvList> 
void 
FtIndexerMem<StringContainer, InvList>::
insertString(const string& s) {
  if(s.empty()) {
    this->nextStringID++; // this is extremely important! caller might rely on increment
    return;
  }
  
  // traverse filtertree if there is one
  if(this->filterTypes.size() > 0 && this->ftFanout > 0) {    
    FilterTreeNode<InvList>* currentNode = this->filterTreeRoot;
    // position currentNode to appropriate leaf node in filter tree
    for(unsigned filterId = 0; filterId < this->filterTypes.size(); filterId++) {
      unsigned key = this->filterTypes.at(filterId)->getKey(s);
      KeyNodePair<InvList> knp(key, NULL);	
      typename vector<KeyNodePair<InvList> >::iterator iter = 
	lower_bound(currentNode->children.begin(), currentNode->children.end(), knp);
      if(iter != currentNode->children.end()) currentNode = iter->node;      
    }
    this->addStringToNode(currentNode, s, this->nextStringID);    
  }
  else {
    // no filters selected, so get the one leaf node
    FilterTreeNode<InvList>* currentNode = this->filterTreeRoot->children.at(0).node;
    this->addStringToNode(currentNode, s, this->nextStringID); 
  }

  this->nextStringID++;
} 

template<class StringContainer, class InvList> 
void 
FtIndexerMem<StringContainer, InvList>::
clearInvertedLists() {
  vector<FilterTreeNode<InvList>*> leaves;
  FilterTreeNode<InvList>::getSubTreeLeaves(this->filterTreeRoot, leaves);
  for(unsigned i = 0; i < leaves.size(); i++) {
    GramMap& gramMap = leaves.at(i)->gramMap;
    typename GramMap::iterator iter;
    for(iter = gramMap.begin(); iter != gramMap.end(); iter++) delete iter->second;
    gramMap.clear();
  }
}

template<class StringContainer, class InvList>
void 
FtIndexerMem<StringContainer, InvList>::
addStringId_Impl(FilterTreeNode<InvList>* leaf, const vector<unsigned>& gramCodes, unsigned stringId) {
  typename SuperClass::GramMap& gramMap = leaf->gramMap;
  
  for(unsigned i = 0; i < gramCodes.size(); i++) {
    unsigned gramCode = gramCodes.at(i);

    if (gramMap.find(gramCode) == gramMap.end()) {
      // a new gram
      GramListSimple<InvList>* newGramList = new GramListSimple<InvList>();
      gramMap[gramCode] = newGramList;    
      InvList* array = newGramList->getArray();
      array->append(stringId);
    }
    else {
      // an existing gram
      GramList<InvList>* gramList = gramMap[gramCode];
      InvList* array = gramList->getArray();
      //avoid adding duplicate elements
      if(array->at(array->size()-1) != stringId)
	array->append(stringId);
    }	        
  }
}

template<class StringContainer, class InvList> 
void 
FtIndexerMem<StringContainer, InvList>::
saveIndex_Impl(const char* indexFileName) {
  ofstream fpOut;
  fpOut.open(indexFileName, ios::out);
  if(fpOut.is_open()) {
    this->saveBasicInfo(fpOut);
    this->saveLeavesRec(this->filterTreeRoot, fpOut);    
    fpOut.close();
  }
  else cout << "ERROR: could not open file " << indexFileName << endl;
}

template<class StringContainer, class InvList> 
void 
FtIndexerMem<StringContainer, InvList>::
loadIndex_Impl(const char* indexFileName) {
  ifstream fpIn;
  fpIn.open(indexFileName, ios::in);
  if(fpIn.is_open()) {
    this->filterTypes.clear();
    this->loadBasicInfo(fpIn);  
    this->buildHollowTreeRecursive(this->filterTreeRoot, 0);  
    this->loadLeavesRec(this->filterTreeRoot, fpIn);  
    fpIn.close();
  }
  else cout << "ERROR: could not open file " << indexFileName << endl;
}



// partial specialization for inverted lists containing positional information, i.e. Array<PosID>
// PosID is defined in gramlist.h

template <class StringContainer>
class FtIndexerMem<StringContainer, Array<PosID> > 
  : public FtIndexerMemAbs<FtIndexerMem<StringContainer, Array<PosID> > > { 

 private:
 typedef 
 FtIndexerMemAbs<FtIndexerMem<StringContainer, Array<PosID> > > 
 SuperClass;

 public:
  typedef typename SuperClass::GramMap GramMap;

  FtIndexerMem(StringContainer* strContainer) : SuperClass(strContainer) {}
  FtIndexerMem(StringContainer* strContainer,
			GramGen* gramGenerator, 
			const unsigned maxStrLen = 150, 
			const unsigned maxChildNodes = 50) 
    : SuperClass(strContainer, gramGenerator, maxStrLen, maxChildNodes) {}
  
  void prepare_Impl() {};    
  void addStringId_Impl(FilterTreeNode<Array<PosID> >* leaf, const vector<unsigned>& gramCodes, unsigned stringId);
  CompressionArgs* getCompressionArgs_Impl() { return &(this->compressionArgs); }
  
  void saveIndex_Impl(const char* indexFileName);
  void loadIndex_Impl(const char* indexFileName);  
  
  void insertString(const string& s);
  void clearInvertedLists();
};

template<class StringContainer> 
void 
FtIndexerMem<StringContainer, Array<PosID> >::
insertString(const string& s) {
  if(s.empty()) {
    this->nextStringID++; // this is extremely important! caller might rely on increment
    return;            
  }
  
  // traverse filtertree if there is one
  if(this->filterTypes.size() > 0 && this->ftFanout > 0) {    
    FilterTreeNode<Array<PosID> >* currentNode = this->filterTreeRoot;
    // position currentNode to appropriate leaf node in filter tree
    for(unsigned filterId = 0; filterId < this->filterTypes.size(); filterId++) {
      unsigned key = this->filterTypes.at(filterId)->getKey(s);
      KeyNodePair<Array<PosID> > knp(key, NULL);	
      typename vector<KeyNodePair<Array<PosID> > >::iterator iter = 
	lower_bound(currentNode->children.begin(), currentNode->children.end(), knp);
      if(iter != currentNode->children.end()) currentNode = iter->node;
    }
    this->addStringToNode(currentNode, s, this->nextStringID);
  }
  else {
    // no filters selected, so get the one leaf node
    FilterTreeNode<Array<PosID> >* currentNode = this->filterTreeRoot->children.at(0).node;
    this->addStringToNode(currentNode, s, this->nextStringID); 
  }

  this->nextStringID++;
} 

template<class StringContainer> 
void 
FtIndexerMem<StringContainer, Array<PosID> >::
clearInvertedLists() {
  vector<FilterTreeNode<Array<PosID> >*> leaves;
  FilterTreeNode<Array<PosID> >::getSubTreeLeaves(this->filterTreeRoot, leaves);
  for(unsigned i = 0; i < leaves.size(); i++) {
    GramMap& gramMap = leaves.at(i)->gramMap;
    typename GramMap::iterator iter;
    for(iter = gramMap.begin(); iter != gramMap.end(); iter++) delete iter->second;
    gramMap.clear();
  }
}

template<class StringContainer>
void 
FtIndexerMem<StringContainer, Array<PosID> >::
addStringId_Impl(FilterTreeNode<Array<PosID> >* leaf, const vector<unsigned>& gramCodes, unsigned stringId) {
  typename SuperClass::GramMap& gramMap = leaf->gramMap;
  
  for(unsigned i = 0; i < gramCodes.size(); i++) {
    unsigned gramCode = gramCodes.at(i);
    
    if (gramMap.find(gramCode) == gramMap.end()) {
      // a new gram
      GramListSimple<Array<PosID> >* newGramList = new GramListSimple<Array<PosID> >();
      gramMap[gramCode] = newGramList;    
      Array<PosID>* array = newGramList->getArray();
      PosID newPosID(stringId, i);
      array->append(newPosID);
    }
    else {
      // an existing gram
      GramList<Array<PosID> >* gramList = gramMap[gramCode];
      Array<PosID>* array = gramList->getArray();
      PosID newPosID(stringId, i);
      array->append(newPosID);
    }	        
  }
}

template<class StringContainer> 
void 
FtIndexerMem<StringContainer, Array<PosID> >::
saveIndex_Impl(const char* indexFileName) {
  /*
  ofstream fpOut;
  fpOut.open(indexFileName, ios::out);
  if(fpOut.is_open()) {
    this->saveBasicInfo(fpOut);
    this->saveLeavesRec(this->filterTreeRoot, fpOut);    
    fpOut.close();
  }
  else cout << "ERROR: could not open file " << indexFileName << endl;
  */
  cout << "ERROR: saveIndex_Impl not implemented" << endl;
}

template<class StringContainer> 
void 
FtIndexerMem<StringContainer, Array<PosID> >::
loadIndex_Impl(const char* indexFileName) {
  /*
  ifstream fpIn;
  fpIn.open(indexFileName, ios::in);
  if(fpIn.is_open()) {
    this->filterTypes.clear();  
    this->loadBasicInfo(fpIn);  
    this->buildHollowTreeRecursive(this->filterTreeRoot, 0);  
    this->loadLeavesRec(this->filterTreeRoot, fpIn);  
    fpIn.close();
  }
  else cout << "ERROR: could not open file " << indexFileName << endl;    
  */
  cout << "ERROR: loadIndex_Impl not implemented" << endl;
}




#endif
