/*
  $Id: simdist.cc 4143 2008-12-08 23:23:55Z abehm $

  Copyright (C) 2007 by The Regents of the University of California

  Redistribution of this file is permitted under the terms of the 
  BSD license

  Date: 01/14/2006
  Author: Rares Vernica <rares (at) ics.uci.edu>
*/

#include "simdist.h"

#if _simdist_h_ == 1

#include "util/simfuncs.h"

SimType (*SimDist)(const string&, const string&) = ed;

#elif _simdist_h_ == 2

#include "util/simfuncs.h"

inline float jdQ(const string &s1, const string &s2) { return jd(s1, s2, Q_GRAM); }
SimType (*SimDist)(const string&, const string&) = jdQ;

#endif
