# $Id: makefile 3169 2008-04-17 01:05:28Z abehm $
#
# Copyright (C) 2010 by The Regents of the University of California
#
# Redistribution of this file is permitted under the terms of the
# BSD license
#
# Date: 01/30/2007
#
# Author: Chen Li <chenli (at) ics.uci.edu>
#         Rares Vernica <rares (at) ics.uci.edu>
#         Jiaheng Lu
#

include ../makefile.inc

all: libcommon.a \
	typedef.o compressionargs.o

libcommon.a: libcommon.a(gramgen.o simmetric.o query.o filtertypes.o)


typedef.o: typedef.h


gramge.h: typedef.h
	touch $@

gramgen.o: gramgen.h typedef.h


compressionargs.h: typedef.h
	touch $@

compressionargs.o: compressionargs.h


simmetric.h: typedef.h gramgen.h compressionargs.h
	touch $@

simmetric.o: simmetric.h util/misc.h


query.h: typedef.h simmetric.h
	touch $@

query.o: query.h


filtertypes.cc: filtertypes.h util/misc.h listmerger/heap.h


unittest: unittest.o libcommon.a $(APPSTRINGROOT)/util/libutil.a

unittest.o: simmetric.h

.PHONY: $(APPSTRINGROOT)/util/libutil.a
$(APPSTRINGROOT)/util/libutil.a:
	$(MAKE) -C $(APPSTRINGROOT)/util libutil.a 

.PHONY: clean
clean: clean-local

.PHONY: clean-local
clean-local:
	-rm *.a *.o *~ unittest
