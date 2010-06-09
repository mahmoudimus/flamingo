/*
  $Id: gramlistsimple.h 5146 2010-03-24 23:05:57Z abehm $

  Copyright (C) 2010 by The Regents of the University of California
	
  Redistribution of this file is permitted under
  the terms of the BSD license.
    
  Date: 04/04/2008
  Author: Alexander Behm <abehm (at) ics.uci.edu>
*/

#ifndef _gramlistsimple_h_
#define _gramlistsimple_h_

#include "gramlist.h"

template<typename InvList = Array<unsigned> >
class GramListSimple : public GramList<InvList> {
 protected:
  InvList invertedList;
  
 public:
  GramListSimple() {};
  InvList* getArray(fstream* invListsFile = NULL) { return &invertedList; }
  ~GramListSimple() {};
  void free() { delete this; }
  void clear() { }
};


// specialized for Array<unsigned> with a better reallocation policy
template<>
class GramListSimple<Array<unsigned> > : public GramList<Array<unsigned> > {
 protected:
  Array<unsigned>* invertedList;
  
 public:
  GramListSimple() 
    : invertedList(new Array<unsigned>(2, 2.0f, REALLOC_MULT)) {}
    
  Array<unsigned>* getArray(fstream* invListsFile = NULL) { return invertedList; }
  ~GramListSimple() { delete invertedList; }
  void free() { delete this; }
  void clear() { }
};

// specialized for Array<PosID> with a better reallocation policy
template<>
class GramListSimple<Array<PosID> > : public GramList<Array<PosID> > {
 protected:
  Array<PosID>* invertedList;
  
 public:
  GramListSimple()
    : invertedList(new Array<PosID>(2, 2.0f, REALLOC_MULT)) {}
  
  Array<PosID>* getArray(fstream* invListsFile = NULL) { return invertedList; }
  ~GramListSimple() { delete invertedList; }
  void free() { delete this; }
  void clear() { }
};


#endif
