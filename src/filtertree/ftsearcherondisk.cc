/*
  $Id: ftsearcherondisk.cc 5146 2010-03-24 23:05:57Z abehm $

  Copyright (C) 2010 by The Regents of the University of California
	
  Redistribution of this file is permitted under
  the terms of the BSD license.
    
  Date: 09/06/2008
  Author: Alexander Behm <abehm (at) ics.uci.edu>
*/

#include "ftsearcherondisk.h"

// special case for pos indexer
template<>
void
FtSearcherOnDisk<class FtIndexerOnDisk<StringContainerRM, Array<PosID> >, 
		 OnDiskMergerAdapt<Array<PosID> > >::
prepare_Impl() {
  // TODO EXPERIMENTS
  if(this->merger->estimationParamsNeeded()) 
    this->merger->setEstimationParams(0.000106651f, 13.9275f, 23.8322f);

  //if(this->merger->estimationParamsNeeded() ) setMergerEstimationParams(1000, 1000);
  this->merger->setGramGen(this->gramGen);
  
  // TODO EXPERIMENTS
  this->merger->gramcode2gram = &this->ftIndexer->gramcode2gram;
}
