# $Id $

# Copyright (C) 2007 by The Regents of the University of California
# Redistribution of this file is permitted under
# the terms of the BSD License.
# Date: 09/06/2008
# Author: Alexander Behm <abehm (at) ics.uci.edu>

include ../../makefile.inc
#include ../../../../codebase/appstring/trunk/makefile.inc

# FOR THE LOOPTIMER
LDFLAGS = -lrt

all: example libfiltertreewrappers.a

wrappersimple.h:	filtertree/ftindexermem.h \
			filtertree/ftsearchermem.h \
			common/simmetric.h \
			common/gramgen.h \
			common/query.h
			touch $@

wrapperdiscardlists.h:	wrapperabs.h \
			filtertree/ftindexerdiscardlists.h
			touch $@

wrappercombinelists.h:	wrapperabs.h \
			filtertree/ftindexercombinelists.h
			touch $@

wrappers.h:		wrappersimple.h \
			wrapperdiscardlists.h \
			wrappercombinelists.h
			touch $@

wrappers.o:		wrappers.h

libfiltertreewrappers.a:	libfiltertreewrappers.a(\
				wrappers.o)

example.o:			wrappers.h

example: 			libfiltertreewrappers.a \
				$(APPSTRINGROOT)/filtertree/libfiltertree.a \
				$(APPSTRINGROOT)/sepia/libsepia.a \
				$(APPSTRINGROOT)/common/libcommon.a \
				$(APPSTRINGROOT)/util/libutil.a \
				$(APPSTRINGROOT)/listmerger/liblistmerger.a

.PHONY:	$(APPSTRINGROOT)/sepia/libsepia.a
$(APPSTRINGROOT)/sepia/libsepia.a: 
	$(MAKE) -C $(APPSTRINGROOT)/sepia libsepia.a

.PHONY: $(APPSTRINGROOT)/common/libcommon.a
$(APPSTRINGROOT)/common/libcommon.a: 
	$(MAKE) -C $(APPSTRINGROOT)/common libcommon.a

.PHONY: $(APPSTRINGROOT)/util/libutil.a
$(APPSTRINGROOT)/util/libutil.a: 
	$(MAKE) -C $(APPSTRINGROOT)/util libutil.a

.PHONY:	$(APPSTRINGROOT)/listmerger/liblistmerger.a
$(APPSTRINGROOT)/listmerger/liblistmerger.a: 
	$(MAKE) -C $(APPSTRINGROOT)/listmerger liblistmerger.a

.PHONY:	$(APPSTRINGROOT)/filtertree/libfiltertree.a
$(APPSTRINGROOT)/filtertree/libfiltertree.a: 
	$(MAKE) -C $(APPSTRINGROOT)/filtertree libfiltertree.a

.PHONY:	clean
clean:
	-rm perftest example *.o *.a *~ *.ix *.rm
	-$(MAKE) -C $(APPSTRINGROOT)/util clean
	-$(MAKE) -C $(APPSTRINGROOT)/listmerger clean
	-$(MAKE) -C $(APPSTRINGROOT)/core clean
	-$(MAKE) -C $(APPSTRINGROOT)/filtertree clean
