# $Id: makefile 4143 2008-12-08 23:23:55Z abehm $

# Copyright (C) 2007 by The Regents of the University of California

# Redistribution of this file is permitted under the terms of the 
# BSD license

# Date: 03/27/2007
# Author: Rares Vernica <rares (at) ics.uci.edu>

TRUNK_PATH = ../../
SEPIA_PATH = ../

include $(SEPIA_PATH)makefile.defs

all: update.o predicate.o record.o errorcorr.o freqest.o test.o

update.o: update.h

predicate.o: predicate.h $(SEPIA_PATH)simfunc/simdist.h

record.o: record.h predicate.h $(SEPIA_PATH)simfunc/simdist.h

errorcorr.o: errorcorr.h record.h predicate.h freqest.h \
	$(SEPIA_PATH)sample.h \
	$(SEPIA_PATH)simfunc/simdist.h $(SEPIA_PATH)simfunc/simvect.h \
	$(SEPIA_PATH)simfunc/editvect.h $(SEPIA_PATH)simfunc/jaccvect.h \
  $(SEPIA_PATH)cluster/cluster.h $(SEPIA_PATH)cluster/ftable.h \
	$(SEPIA_PATH)cluster/clusters.h \
  $(SEPIA_PATH)ppdtable/ppdtable.h $(SEPIA_PATH)ppdtable/ppdentry.h \
	$(TRUNK_PATH)util/simfuncs.h

freqest.o: freqest.h predicate.h \
	$(SEPIA_PATH)sample.h \
	$(SEPIA_PATH)simfunc/simdist.h $(SEPIA_PATH)simfunc/simvect.h \
	$(SEPIA_PATH)simfunc/editvect.h $(SEPIA_PATH)simfunc/jaccvect.h \
	$(SEPIA_PATH)cluster/cluster.h $(SEPIA_PATH)cluster/ftable.h \
	$(SEPIA_PATH)cluster/clusters.h \
  $(SEPIA_PATH)ppdtable/ppdtable.h $(SEPIA_PATH)ppdtable/ppdentry.h \
	$(TRUNK_PATH)util/simfuncs.h

test.o: test.h errorcorr.h record.h predicate.h freqest.h \
	$(SEPIA_PATH)sample.h \
	$(SEPIA_PATH)simfunc/simdist.h $(SEPIA_PATH)simfunc/simvect.h \
	$(SEPIA_PATH)simfunc/editvect.h $(SEPIA_PATH)simfunc/jaccvect.h \
  $(SEPIA_PATH)cluster/cluster.h $(SEPIA_PATH)cluster/ftable.h \
	$(SEPIA_PATH)cluster/clusters.h \
  $(SEPIA_PATH)ppdtable/ppdtable.h $(SEPIA_PATH)ppdtable/ppdentry.h \
	$(TRUNK_PATH)util/simfuncs.h 


.PHONY: clean
clean:
	-rm *.o *~