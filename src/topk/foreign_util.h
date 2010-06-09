/*
  $Id: foreign_util.h 4285 2009-02-19 19:02:52Z rares $

  Copyright (C) 2007 by The Regents of the University of California
 
  Redistribution of this file is permitted under the terms of the BSD
  license

  Date: 06/25/2008
  Author: Rares Vernica <rares (at) ics.uci.edu>
*/

#define _foreign_util_h_
#include "util/array.h"

#ifndef _foreign_util_h_
#define _foreign_util_h_

#include "common/typedef.h"

template<class T> 
struct Array 
{
  Array(uint);
  void insert(T);
  void append(T);
  uint size();
  uint at(uint);
  uint binarySearch(uint, uint);
  uint operator[](uint);
};

#endif
