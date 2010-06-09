# $Id: makefile 5146 2010-03-24 23:05:57Z abehm $

# Copyright (C) 2010 by The Regents of the University of California	
# Redistribution of this file is permitted under
# the terms of the BSD license.

# Date: 04/04/2008
# Author: Chen Li, Alexander Behm

include ../makefile.inc

#override the CPPFLAGS in makefile.inc
CPPFLAGS = -Wall -I$(APPSTRINGROOT) -O3 -DDEBUG_TIMER_FANCY -DDEBUG_STAT

# FOR THE LOOPTIMER
LDFLAGS = -lrt

#all: libfiltertree.a example perftest unittest
all: libfiltertree.a example perftest

gramlist.h:		util/array.h
			touch $@

gramlistsimple.h:	gramlist.h
			touch $@
gramlistsimple.o:	gramlistsimple.h


gramlistunion.h:	gramlist.h
			touch $@
gramlistunion.o:	gramlistunion.h


gramlistondisk.h:	gramlist.h
			touch $@
gramlistondisk.o:	gramlistondisk.h


filtertreenode.h:	gramlistsimple.h \
			util/array.h \
			common/filtertypes.h \
			common/gramgen.h
			touch $@
filtertreenode.o:	filtertreenode.h


ftindexerabs.h:		filtertreenode.h \
			statsutil.h \
			gramlistsimple.h \
			stringcontainer.h \
			common/filtertypes.h \
			common/gramgen.h \
			common/compressionargs.h
			touch $@
ftindexerabs.o:		ftindexerabs.h


ftindexermemabs.h:	ftindexerabs.h \
			gramlistsimple.h
			touch $@
ftindexermemabs.o:	ftindexermemabs.h


ftindexermem.h:	ftindexermemabs.h
				touch $@		
ftindexermem.o:	ftindexermem.h


ftindexerdiscardlists.h:		ftindexermemabs.h \
				ftindexermem.h \
				listmerger/divideskipmerger.h
				touch $@						
ftindexerdiscardlists.o:		ftindexerdiscardlists.h


ftindexercombinelists.h:		ftindexermemabs.h \
				gramlistunion.h \
				listmerger/divideskipmerger.h
				touch $@
ftindexercombinelists.o:		ftindexercombinelists.h


ftindexerondisk.h:		ftindexerabs.h \
				ftindexermem.h \
				gramlistondisk.h
				touch $@		
ftindexerondisk.o:		ftindexerondisk.h


ftsearcherabs.h:	statsutil.h \
			filtertreenode.h \
			common/filtertypes.h \
			common/query.h
			touch $@
ftsearcherabs.o:	ftsearcherabs.h


ftsearchermem.h:	ftsearcherabs.h \
			ftindexermem.h \
			common/query.h \
			listmerger/divideskipmerger.h
			touch $@
ftsearchermem.o:	ftsearchermem.h


ftsearcherondisk.h:	ftsearcherabs.h \
			ftindexerondisk.h \
			listmerger/ondiskmergersimple.h \
			util/misc.h
			touch $@
ftsearcherondisk.o:	ftsearcherondisk.h


statsutil.h: 	util/array.h
		touch $@
statsutil.o: 	statsutil.h


statsgen.h: 	statsutil.h \
		ftsearchermem.h \
		ftsearcherondisk.h \
		ftindexermem.h \
		ftindexerdiscardlists.h \
		ftindexercombinelists.h \
		ftindexerondisk.h \
		common/simmetric.h \
		common/gramgen.h \
		util/looptimer.h
		touch $@
statsgen.o: 	statsgen.h ftindexerdiscardlists.h util/input.h util/misc.h


stringrm.o:	stringrm.h


stringcontainer.h:	stringrm.h \
			util/misc.h \
			common/filtertypes.h
			touch $@
stringcontainer.o:	stringcontainer.h \
			common/gramgen.h \
			common/simmetric.h \
			sepia/sepia.h \
			util/misc.h


libfiltertree.a: 	libfiltertree.a( \
			gramlistsimple.o \
			gramlistunion.o \
			gramlistondisk.o \
			filtertreenode.o \
			ftindexerabs.o \
			ftindexermemabs.o \
			ftindexermem.o \
			ftindexerdiscardlists.o \
			ftindexercombinelists.o \
			ftindexerondisk.o \
			ftsearcherabs.o \
			ftsearchermem.o \
			ftsearcherondisk.o \
			stringrm.o \
			stringcontainer.o \
			statsutil.o)		

perftest.o: 	ftindexermem.h \
		ftsearchermem.h \
		statsutil.h \
		statsgen.h \
		listmerger/listmerger.h \
		common/gramgen.h \
		util/input.h \
		util/simfuncs.h \
		util/array.h

perftest: 	statsgen.o \
		libfiltertree.a \
		$(APPSTRINGROOT)/sepia/libsepia.a \
		$(APPSTRINGROOT)/listmerger/liblistmerger.a \
		$(APPSTRINGROOT)/common/libcommon.a \
	  	$(APPSTRINGROOT)/util/libutil.a

example.o: 	ftindexerdiscardlists.h \
		ftindexercombinelists.h \
		ftsearchermem.h \
		common/query.h \
		common/simmetric.h \
		listmerger/divideskipmerger.h \
		listmerger/scancountmerger.h \
		ftindexerondisk.h \
		listmerger/ondiskmergersimple.h \
		listmerger/ondiskmergerabs.h

example: 	libfiltertree.a \
		$(APPSTRINGROOT)/sepia/libsepia.a \
		$(APPSTRINGROOT)/common/libcommon.a \
	  	$(APPSTRINGROOT)/listmerger/liblistmerger.a \
		$(APPSTRINGROOT)/util/libutil.a

unittest.o: 	ftindexerdiscardlists.h \
		ftindexercombinelists.h \
		ftsearchermem.h \
		common/query.h \
		common/simmetric.h \
		listmerger/divideskipmerger.h \
		listmerger/scancountmerger.h

unittest: 	libfiltertree.a \
		$(APPSTRINGROOT)/sepia/libsepia.a \
		$(APPSTRINGROOT)/common/libcommon.a \
	  	$(APPSTRINGROOT)/listmerger/liblistmerger.a \
		$(APPSTRINGROOT)/util/libutil.a \
		unittest.o


.PHONY: $(APPSTRINGROOT)/common/libcommon.a
$(APPSTRINGROOT)/common/libcommon.a: 
	$(MAKE) -C $(APPSTRINGROOT)/common libcommon.a

.PHONY: $(APPSTRINGROOT)/util/libutil.a
$(APPSTRINGROOT)/util/libutil.a: 
	$(MAKE) -C $(APPSTRINGROOT)/util libutil.a

.PHONY:	$(APPSTRINGROOT)/listmerger/liblistmerger.a
$(APPSTRINGROOT)/listmerger/liblistmerger.a: 
	$(MAKE) -C $(APPSTRINGROOT)/listmerger liblistmerger.a

.PHONY:	$(APPSTRINGROOT)/sepia/libsepia.a
$(APPSTRINGROOT)/sepia/libsepia.a: 
	$(MAKE) -C $(APPSTRINGROOT)/sepia libsepia.a

.PHONY:	clean
clean:
	-rm perftest unittest example *.o *.a *~ *.ix *.rm *.tmp *.out
	-$(MAKE) -C $(APPSTRINGROOT)/util clean
	-$(MAKE) -C $(APPSTRINGROOT)/listmerger clean
	-$(MAKE) -C $(APPSTRINGROOT)/common clean
	-$(MAKE) -C $(APPSTRINGROOT)/sepia clean
