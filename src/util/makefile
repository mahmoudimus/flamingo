# $Id: makefile 5151 2010-03-24 23:58:00Z abehm $
#
# Copyright (C) 2010 by The Regents of the University of California
#
# Redistribution of this file is permitted under the terms of the GNU
# BSD license.
#
# Date: 01/30/2007
# Author: Chen Li <chenli (at) ics.uci.edu>
#         Rares Vernica <rares (at) ics.uci.edu>
#         Jiaheng Lu


include ../makefile.inc

LDFLAGS = -lrt

all: libutil.a range.o querygroup.o querygrams.o unittest #speedtest

libutil.a: libutil.a(simfuncs.o misc.o \
	input.o output.o io.o \
	array.o stringshuffler.o gram.o \
	looptimer.o time.o)


simfuncs.o: simfuncs.h misc.h gram.h

gram.o: gram.h misc.h input.h

gram.h: array.h
	touch $@


misc.o: misc.h input.h

output.o: output.h


input.o: input.h


stringshuffler.o: stringshuffler.h input.h


appsearch.o: appsearch.h simfuncs.h input.h misc.h


scan.o: scan.h simfuncs.h

scan.h: appsearch.h
	touch $@


array.o: array.h

### === Range ===
range.o: range.h

### === Query Group ===
querygroup.o: querygroup.h

### === Query Grams ===
querygrams.o: querygrams.h

### === IO ===
io.o: io.h


looptimer.o: looptimer.h

### === Time ===
time.o: time.h

time.h: util/looptimer.h


unittest.o: input.h gram.h misc.h output.h array.h stringshuffler.h simfuncs.h

unittest: unittest.o libutil.a


#speedtest.o: appsearch.h input.h scan.h \
# 	$(APPSTRINGROOT)/partenum/partenum.h \
#  	$(APPSTRINGROOT)/filtertree_old/filtertree.h \

#speedtest: speedtest.o appsearch.o scan.o \
# 	$(APPSTRINGROOT)/partenum/libpartenum.a \
#	libutil.a \
#  	$(APPSTRINGROOT)/filtertree_old/buckethead.o \
#  	$(APPSTRINGROOT)/filtertree_old/filtertree.o \
#  	$(APPSTRINGROOT)/filtertree_old/gram.o


#.PHONY: $(APPSTRINGROOT)/partenum/libpartenum.a
#$(APPSTRINGROOT)/partenum/libpartenum.a:
#	$(MAKE) -C $(APPSTRINGROOT)/partenum libpartenum.a


.PHONY: clean
clean:
	-rm speedtest unittest *.a *.o *~ *.bin dataset.*.txt
