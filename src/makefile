# $Id: makefile 5149 2010-03-24 23:37:18Z abehm $
#
# Copyright (C) 2007 by The Regents of the University of California
#
# Redistribution of this file is permitted under the terms of the
# BSD license
#
# Date: 09/10/2008
#
# Author: Rares Vernica <rares (at) ics.uci.edu>
#

all: common util listmerger filtertree filtertree/wrappers partenum sepia stringmap topk

.PHONY: common util listmerger filtertree filtertree/wrappers partenum sepia stringmap topk

common:
	$(MAKE) -C common

util:
	$(MAKE) -C util

listmerger:
	$(MAKE) -C listmerger

filtertree:
	$(MAKE) -C filtertree

filtertree/wrappers:
	$(MAKE) -C filtertree/wrappers

partenum:
	$(MAKE) -C partenum

sepia:
	$(MAKE) -C sepia

stringmap:
	$(MAKE) -C stringmap

topk:
	$(MAKE) -C topk

.PHONY: clean
clean:
	$(MAKE) -C common clean
	$(MAKE) -C util clean
	$(MAKE) -C listmerger clean
	$(MAKE) -C filtertree clean
	$(MAKE) -C filtertree/wrappers clean
	$(MAKE) -C partenum clean
	$(MAKE) -C sepia clean
	$(MAKE) -C stringmap clean
	$(MAKE) -C topk clean
