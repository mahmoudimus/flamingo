# $Id: makefile 5026 2010-02-17 20:25:03Z rares $
#
# Copyright (C) 2007 by The Regents of the University of California
#
# Redistribution of this file is permitted under the terms of the BSD
# license
#
# Date: 04/22/2008
#
# Author: Rares Vernica <rares (at) ics.uci.edu>
#

include ../makefile.inc

all: libtopk.a topkiterative.o topktwophase.o \
	misc.o display.o perftest unittest example

libtopk.a: libtopk.a(topkindex.o topkindex.v1.o \
	topkroundrobin.o topkheap.v1.o topkheap.o topk.o topksearch.o)

### === Index ===
topkindex.h: util/range.h common/query.h
	touch $@

topkindex.o: topkindex.h util/io.h

### === Index v1 ===
topkindex.v1.h: foreign_util.h common/gramgen.h
	touch $@

topkindex.v1.o: topkindex.v1.h util/io.h

### ==== Iterative ===
topkiterative.h: topkindex.h topkindex.v1.h foreign_listmerger.h
	touch $@

topkiterative.o: topkiterative.h

### === RoundRobin ===
topkroundrobin.h: topkindex.h
	touch $@

topkroundrobin.o: topkroundrobin.h 

## === Heap ===
topkheap.h: topkindex.h
	touch $@

topkheap.o: topkheap.h

## === Heap v1 ===
topkheap.v1.h: topkindex.h topkindex.v1.h topkheap.h foreign_listmerger.h
	touch $@

topkheap.v1.o: topkheap.v1.h

### === TwoPhase ===
topktwophase.h: topkiterative.h topkheap.v1.h
	touch $@

topktwophase.o: topktwophase.h

## === Scan ===
topkscan.h: topkindex.h
	touch $@

topkscan.o: topkscan.h

### === Topk -- main ===
topk.h: topkiterative.h topkroundrobin.h topkheap.h topkheap.v1.h topktwophase.h
	touch $@

topk.o: topk.h

### === Search ===
topksearch.h: topk.h

topksearch.o: topksearch.h

### === Misc ===
misc.h: common/typedef.h
	touch $@

misc.o: misc.h

### === Display ===

display.h: util/range.h common/query.h

display.o: display.h

### === UnitTest ===
unittest.o: topk.h topksearch.h

unittest: unittest.o \
	libtopk.a \
	$(APPSTRINGROOT)/common/libcommon.a \
	$(APPSTRINGROOT)/util/libutil.a \
	$(APPSTRINGROOT)/listmerger/liblistmerger.a


### === PerfTest Util ===
perftest_util.h: topkindex.h topkindex.v1.h
	touch $@

perftest_util.o: perftest_util.h util/io.h misc.h \
	util/debug.h util/input.h util/misc.h

### === PerfTest Exps ===
perftest_exps.h: topk.h perftest_util.h
	touch $@

perftest_util.o: perftest_exps.h util/io.h \
	util/debug.h util/misc.h

### === PerfTest ===
perftest.o: topk.h util/io.h util/debug.h util/misc.h perftest_util.h \
	perftest_exps.h

perftest: perftest.o perftest_util.o display.o \
	libtopk.a \
	$(APPSTRINGROOT)/common/libcommon.a \
	$(APPSTRINGROOT)/util/libutil.a \
	$(APPSTRINGROOT)/listmerger/liblistmerger.a \
	perftest_exps.o

### === Example ===
example.o: topk.h topksearch.h

example: example.o \
	libtopk.a \
	$(APPSTRINGROOT)/common/libcommon.a \
	$(APPSTRINGROOT)/util/libutil.a \
	$(APPSTRINGROOT)/listmerger/liblistmerger.a

### === libraries ===
.PHONY:	$(APPSTRINGROOT)/common/libcommon.a
$(APPSTRINGROOT)/common/libcommon.a:
	$(MAKE) -C $(APPSTRINGROOT)/common libcommon.a

.PHONY:	$(APPSTRINGROOT)/util/libutil.a
$(APPSTRINGROOT)/util/libutil.a:
	$(MAKE) -C $(APPSTRINGROOT)/util libutil.a

.PHONY:	$(APPSTRINGROOT)/listmerger/liblistmerger.a
$(APPSTRINGROOT)/listmerger/liblistmerger.a:
	$(MAKE) -C $(APPSTRINGROOT)/listmerger liblistmerger.a


### === clean ===
.PHONY: clean-local
clean-local:
	-rm *.a *.o *~ unittest perftest example

.PHONY: clean
clean: clean-local
	$(MAKE) -C $(APPSTRINGROOT)/common clean
	$(MAKE) -C $(APPSTRINGROOT)/util clean
	$(MAKE) -C $(APPSTRINGROOT)/listmerger clean

