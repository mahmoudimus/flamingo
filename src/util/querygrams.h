/*
  $Id: querygrams.h 5149 2010-03-24 23:37:18Z abehm $

  Copyright (C) 2010 by The Regents of the University of California
 
  Redistribution of this file is permitted under the terms of the BSD
  license.

  Date: 11/07/2008
  Author: Rares Vernica <rares (at) ics.uci.edu>
*/

#ifndef _querygrams_h_
#define _querygrams_h_

#include "common/simmetric.h"

#include <vector>

class QueryGrams
{
public:
  const std::vector<uint> &str;
  const uint noGrams, len;
  const SimMetric &sim;  
  const uint k;

  QueryGrams(
    std::vector<uint> &grams, const SimMetric &sim, uint k):
    str(grams), 
    noGrams(grams.size()), 
    len(noGrams), 
    sim(sim), 
    k(k)
  {}
};

#endif
