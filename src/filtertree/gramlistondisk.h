/*
  $Id: gramlistondisk.h 5146 2010-03-24 23:05:57Z abehm $

  Copyright (C) 2010 by The Regents of the University of California
	
  Redistribution of this file is permitted under
  the terms of the BSD license.
    
  Date: 09/06/2008
  Author: Alexander Behm <abehm (at) ics.uci.edu>
*/

#ifndef _gramlistondisk_h_
#define _gramlistondisk_h_

#include "gramlist.h"

#include <iostream>
#include <cstring>

template <typename InvList = Array<unsigned> >
class GramListOnDisk : public GramList<InvList> {
private:
  InvList* invList;

public:
  unsigned listSize;
  streampos startOffset;
  
  GramListOnDisk()
    : invList(NULL), listSize(0), startOffset(0) {}
  
  GramListOnDisk(unsigned listSize, streampos& startOffset)
    : invList(NULL), listSize(listSize), startOffset(startOffset) {}
  
  InvList* getArray(fstream* invListsFile = NULL) {     
    if(invList) return invList;
    //cout << "NOT RETURNING IMMEDIATELY?!" << endl;
    if(!invListsFile) return NULL;    
    invList = new InvList(listSize);
    invList->setSize(listSize);
    unsigned bytes = listSize * InvList::elementSize();
    invListsFile->seekg(startOffset);
    invListsFile->read((char*)invList->begin(), bytes);
    return invList;
  }
  
  // fill the array from the given get position of the file stream
  // old invList will be deleted
  // no disk seek is performed
  void fillArray(fstream* invListsFile) {
    if(invList) clear();
    invList = new InvList(listSize);
    invList->setSize(listSize);
    unsigned bytes = listSize * InvList::elementSize();
    if(invListsFile->tellg() != startOffset) 
      cout << "THERE IS A PROBLEM HERE" << endl;
    invListsFile->read((char*)invList->begin(), bytes);    
  }
  
  void fillArray(char* data) {
    if(invList) clear();
    invList = new InvList(listSize);
    invList->setSize(listSize);
    unsigned bytes = listSize * InvList::elementSize();
    memcpy((void*)invList->begin(), (const void*)data, bytes);    
  }
  
  void clear() {
    if(invList) delete invList;
    invList = NULL;
  }

  void free() {
    clear();
    delete this;
  }

  ~GramListOnDisk() {
    clear();
  }
};

#endif
