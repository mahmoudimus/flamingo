/*
  $Id: display.h 5026 2010-02-17 20:25:03Z rares $

  Copyright (C) 2007 by The Regents of the University of California
 
  Redistribution of this file is permitted under the terms of the BSD
  license

  Date: 08/19/2008
  Author: Rares Vernica <rares (at) ics.uci.edu>
*/

#ifndef _display_h_
#define _display_h_

#include "common/query.h"
#include "util/range.h"

#include <list>

struct DisplayListsInfo 
{
  range<uint*> r;
  uint *i, *p, *c;
  bool e, u;

  DisplayListsInfo(range<uint*> r):
    r(r), i(r._begin), p(r._begin), c(r._begin), u(false)
    {}
};

struct DisplayLists
{
  std::vector<std::string> gs;
  std::list<DisplayListsInfo> lists;
  uint g, gn, longest;

  void init(
    const std::list<range<uint *> > &initialLists, 
    const Query &query);

  void show(const std::list<range<uint *> > &crt);
};

#endif
