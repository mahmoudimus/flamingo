/*
  $Id: gramlist.h 5146 2010-03-24 23:05:57Z abehm $

  Copyright (C) 2010 by The Regents of the University of California
	
  Redistribution of this file is permitted under
  the terms of the BSD license.
    
  Date: 04/04/2008
  Author: Alexander Behm <abehm (at) ics.uci.edu>
          Shengyue Ji <shengyuj (at) ics.uci.edu>
*/

#ifndef _gramlist_h_
#define _gramlist_h_

#include "util/array.h"

#include <fstream>

#pragma pack(push,1) 
class PosID {
public:
  unsigned id;
  unsigned char pos;
  
  PosID(unsigned id, unsigned char pos)
    : id(id), pos(pos) {}

  PosID()
    : id(0), pos(0) {}
  
  // considered equal if the ids match
  // WARNING: the implementation of this is critical to the functioning
  // of the ignorelist merger if Array<PosID> is used as inverted list type
  inline bool operator==(const PosID& p) const {
    return id == p.id;
    /*
    if(id == p.id) {
      if(pos == p.pos) return true;
      else return false;
    }
    else return false;    
    */
  }    
  
  inline bool operator<(const PosID* p) const {
    if(id == p->id) return pos < p->pos;
    else return id < p->id;
  }
  
  inline bool operator<(const PosID& p) const {
    if(id == p.id) return pos < p.pos;
    else return id < p.id;
  }
  
  friend ostream& operator<<(ostream &stream, PosID& p) {
    stream << p.id << " " << (unsigned)p.pos;
    return stream; 
  }
  
  friend bool operator!=(const PosID& a, const PosID& b) {
    if(a.id != b.id || a.pos != b.pos) return true;
    return false;
  }
};
#pragma pack(pop)

namespace std {
  namespace tr1 {
    template<> struct hash<PosID> {
      hash<unsigned> h;
      inline bool operator() (const PosID& g1) const {
	return h(g1.id);
      }
    };      
  }
}

template <typename InvList = Array<unsigned> >
class GramList {
 public:  
  virtual InvList* getArray(fstream* invListsFile = NULL) = 0;
  virtual ~GramList() {};
  virtual void free() = 0;
  virtual void clear() = 0;
};

template<class InvList = Array<unsigned> >
class QueryGramList {
public:
  unsigned gramCode;
  GramList<InvList>* gl;  

  QueryGramList(unsigned gramCode, GramList<InvList>* gl)
    : gramCode(gramCode), gl(gl) {}
};

#pragma pack(push,1) 
template<>
class QueryGramList<Array<PosID> > {
public:
  unsigned gramCode;
  GramList<Array<PosID> >* gl;
  unsigned char pos;
  
  QueryGramList(unsigned gramCode, GramList<Array<PosID> >* gl, unsigned char pos) 
    : gramCode(gramCode), gl(gl), pos(pos) {}
};
#pragma pack(pop)

#endif
