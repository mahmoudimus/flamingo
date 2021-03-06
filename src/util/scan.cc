/*
  $Id: scan.cc 5149 2010-03-24 23:37:18Z abehm $

  Copyright (C) 2010 by The Regents of the University of California

  Redistribution of this file is permitted under the terms of the 
  BSD license.

  Date: 03/19/2007
  Author: Rares Vernica <rares (at) ics.uci.edu>
*/

#include "scan.h"
#include "simfuncs.h"

void Scan::search(const string &query, const unsigned editdist, 
                  vector<unsigned> &results)
{
  for (unsigned i = 0; i < data->size(); i++)
    if (ed(query, (*data)[i], editdist))
      results.push_back(i);
}

