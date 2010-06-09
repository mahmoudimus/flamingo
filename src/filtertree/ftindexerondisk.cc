/*
  $Id: ftindexerondisk.cc 5146 2010-03-24 23:05:57Z abehm $

  Copyright (C) 2010 by The Regents of the University of California
	
  Redistribution of this file is permitted under
  the terms of the BSD license.
    
  Date: 09/06/2008
  Author: Alexander Behm <abehm (at) ics.uci.edu>
*/

#include "ftindexerondisk.h"

vector<unsigned>* InvListSpecializer<Array<PosID> >::stringid2leafid;
