/*
  $Id: filtertreenode.cc 5146 2010-03-24 23:05:57Z abehm $

  Copyright (C) 2010 by The Regents of the University of California
	
  Redistribution of this file is permitted under
  the terms of the BSD license.
    
  Date: 04/04/2008
  Author: Alexander Behm <abehm (at) ics.uci.edu>
*/

#include "filtertreenode.h"

template<>
void
FilterTreeNode<Array<PosID> >::
getQueryGramLists(const vector<unsigned>& gramCodes, vector<QueryGramList<Array<PosID> >* >* queryGramLists) {
  for(unsigned i = 0; i < gramCodes.size(); i++) {
    unsigned gramCode = gramCodes.at(i);
    if(gramMap.find(gramCode) != gramMap.end()) {      
      queryGramLists->push_back(new QueryGramList<Array<PosID> >(gramCode, gramMap[gramCode], (unsigned char)i));
    }
  }
}

