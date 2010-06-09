/*
  $Id: unittest.cc 4143 2008-12-08 23:23:55Z abehm $

  Copyright (C) 2007 by The Regents of the University of California

  Redistribution of this file is permitted under the terms of the 
  BSD license

  Date: 04/03/2007
  Author: Rares Vernica <rares (at) ics.uci.edu>
*/

#include <cassert>
#include <iostream>

#include "sepia.h"

using namespace std;

void testSepia() 
{
  unsigned t = 0;

  vector<string> data;
  data.push_back("abc");
  data.push_back("ac");
  data.push_back("xyz");

  Sepia sSave(data, 1, 2, 1, 1, MEDOIDS_IMP, CLOSE_RAND, 1, 1, 1, 1, 1, 1);
  sSave.build();
  sSave.saveData("sepia"); // save
  Sepia sLoad(data, "sepia"); // load
  assert(sSave == sLoad); t++;

  cout << "sepia (" << t << ")" << endl;
}

int main() 
{
  cout << "test..." << endl;

  testSepia();

  cout << "OK" << endl;
}
