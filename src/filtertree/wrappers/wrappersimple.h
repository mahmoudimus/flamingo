/*
  $Id: wrappersimple.h 5146 2010-03-24 23:05:57Z abehm $

  Copyright (C) 2010 by The Regents of the University of California
	
  Redistribution of this file is permitted under
  the terms of the BSD license.
    
  Date: 09/06/2008
  Author: Alexander Behm <abehm (at) ics.uci.edu>
*/

#ifndef _wrappersimple_h_
#define _wrappersimple_h_

#include "wrapperabs.h"

template <class SimilarityMetric = SimMetricEd>
class WrapperSimple : public WrapperAbs<FtIndexerMem<>, FtSearcherMem<>, SimilarityMetric> {  
public:  
  WrapperSimple(StringContainerVector* strContainer, unsigned gramLength = 3,  bool usePartFilter = true, bool usePrePost = true) {
    this->gramGen = new GramGenFixedLen(gramLength, usePrePost);
    this->indexer = new FtIndexerMem<>(strContainer, this->gramGen);
    if(usePartFilter) this->indexer->autoAddPartFilter();
    this->searcher = new FtSearcherMem<>(&this->merger, this->indexer);
    this->simMetric = new SimilarityMetric(*(this->gramGen));
  }
};

#endif
