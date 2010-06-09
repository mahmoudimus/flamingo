/*
  $Id: query.h 5149 2010-03-24 23:37:18Z abehm $

  Copyright (C) 2010 by The Regents of the University of California
 
  Redistribution of this file is permitted under the terms of the 
  BSD license.
  
  Date: 04/22/2008
  Author: Rares Vernica <rares (at) ics.uci.edu>
*/

#ifndef _query_h_
#define _query_h_

#include "typedef.h"
#include "simmetric.h"

typedef enum
{
  QueryNotDefined = 0,
  QueryRange = 1, 
  QueryRanking = 2
} QueryType ;

class Query 
{
public:
  const std::string str;
  const uint len;
  uint noGrams;                 // temporary not const, see setNoGrams
  const SimMetric &sim;
  
  const float threshold;
  const uint k;
  const QueryType type;

  Query(std::string str, const SimMetric &sim, float threshold):
    str(str), 
    len(str.length()), 
    noGrams(sim.gramGen.getNumGrams(str)), 
    sim(sim),
    threshold(threshold), 
    k(0), 
    type(QueryRange) 
    {
      setNoGrams();
    }

  Query(std::string str, const SimMetric &sim, uint k):
    str(str), 
    len(str.length()), 
    noGrams(sim.gramGen.getNumGrams(str)), 
    sim(sim),
    threshold(0), 
    k(k), 
    type(QueryRanking) 
    {
      setNoGrams();
    }

  Query(
    std::string str, 
    const SimMetric &sim, 
    float threshold, 
    uint k, 
    QueryType type):
    str(str), 
    len(str.length()), 
    noGrams(sim.gramGen.getNumGrams(str)), 
    sim(sim), 
    threshold(threshold), 
    k(k), 
    type(type) 
    {
      setNoGrams();
    }

private:
  // temporary, until Alex implements the set & bag semantics in GramGen
  // set semantics !
  void setNoGrams() 
    {
      std::set<uint> g;
      sim.gramGen.decompose(str, g);
      noGrams = g.size();
    }
};

#endif
