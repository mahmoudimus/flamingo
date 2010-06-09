# $Id: makefile 4143 2008-12-08 23:23:55Z abehm $

# Copyright (C) 2007 by The Regents of the University of California

# Redistribution of this file is permitted under the terms of the 
# BSD license

# Date: 03/25/2007
# Author: Rares Vernica <rares (at) ics.uci.edu>

TRUNK_PATH = ../../
SEPIA_PATH = ../

include $(SEPIA_PATH)makefile.defs

all: ppdentry.o ppdpair.o ppdtable.o ppdtriple.o ppdsample.o indexentry.o \
	allrand.o closelex.o closerand.o closeunique.o

ppdentry.o: ppdentry.h \
	$(SEPIA_PATH)simfunc/simdist.h $(SEPIA_PATH)simfunc/simvect.h \
	$(SEPIA_PATH)simfunc/editvect.h $(SEPIA_PATH)simfunc/jaccvect.h \
	$(TRUNK_PATH)util/simfuncs.h

ppdpair.o: ppdpair.h \
	$(SEPIA_PATH)simfunc/simdist.h $(SEPIA_PATH)simfunc/simvect.h \
  $(SEPIA_PATH)simfunc/editvect.h $(SEPIA_PATH)simfunc/jaccvect.h \
	$(TRUNK_PATH)util/simfuncs.h

ppdtable.o: ppdtable.h ppdentry.h \
	$(SEPIA_PATH)simfunc/simdist.h $(SEPIA_PATH)simfunc/simvect.h \
	$(SEPIA_PATH)simfunc/editvect.h $(SEPIA_PATH)simfunc/jaccvect.h \
	$(TRUNK_PATH)util/simfuncs.h

ppdtriple.o: ppdtriple.h \
	$(SEPIA_PATH)simfunc/simdist.h $(SEPIA_PATH)simfunc/simvect.h \
	$(SEPIA_PATH)simfunc/editvect.h $(SEPIA_PATH)simfunc/jaccvect.h \
	$(TRUNK_PATH)util/simfuncs.h

ppdsample.o: ppdsample.h ppdtable.h ppdentry.h ppdtriple.h \
	$(SEPIA_PATH)sample.h \
	$(SEPIA_PATH)simfunc/simdist.h $(SEPIA_PATH)simfunc/simvect.h \
	$(SEPIA_PATH)simfunc/editvect.h $(SEPIA_PATH)simfunc/jaccvect.h \
  $(SEPIA_PATH)cluster/cluster.h $(SEPIA_PATH)cluster/ftable.h \
	$(SEPIA_PATH)cluster/clusters.h \
	$(TRUNK_PATH)util/simfuncs.h

indexentry.o: indexentry.h \
	$(SEPIA_PATH)simfunc/simdist.h $(SEPIA_PATH)simfunc/simvect.h \
	$(SEPIA_PATH)simfunc/editvect.h $(SEPIA_PATH)simfunc/jaccvect.h \
	$(TRUNK_PATH)util/simfuncs.h

allrand.o: allrand.h ppdsample.h ppdtable.h ppdentry.h ppdtriple.h \
  $(SEPIA_PATH)sample.h \
  $(SEPIA_PATH)simfunc/simdist.h $(SEPIA_PATH)simfunc/simvect.h \
	$(SEPIA_PATH)simfunc/editvect.h $(SEPIA_PATH)simfunc/jaccvect.h \
	$(SEPIA_PATH)cluster/cluster.h $(SEPIA_PATH)cluster/ftable.h \
	$(SEPIA_PATH)cluster/clusters.h \
  $(TRUNK_PATH)util/simfuncs.h

closelex.o: closelex.h ppdsample.h ppdtable.h ppdentry.h indexentry.h ppdtriple.h \
  $(SEPIA_PATH)sample.h \
  $(SEPIA_PATH)simfunc/simdist.h $(SEPIA_PATH)simfunc/simvect.h \
	$(SEPIA_PATH)simfunc/editvect.h $(SEPIA_PATH)simfunc/jaccvect.h \
	$(SEPIA_PATH)cluster/cluster.h $(SEPIA_PATH)cluster/ftable.h \
	$(SEPIA_PATH)cluster/clusters.h \
  $(TRUNK_PATH)util/simfuncs.h

closerand.o: closerand.h ppdsample.h ppdtable.h ppdentry.h indexentry.h \
	ppdtriple.h \
  $(SEPIA_PATH)sample.h \
  $(SEPIA_PATH)simfunc/simdist.h $(SEPIA_PATH)simfunc/simvect.h \
	$(SEPIA_PATH)simfunc/editvect.h $(SEPIA_PATH)simfunc/jaccvect.h \
	$(SEPIA_PATH)cluster/cluster.h $(SEPIA_PATH)cluster/ftable.h \
	$(SEPIA_PATH)cluster/clusters.h \
	$(TRUNK_PATH)util/simfuncs.h

closeunique.o: closeunique.h ppdsample.h ppdtable.h ppdentry.h indexentry.h \
	ppdtriple.h \
  $(SEPIA_PATH)sample.h \
  $(SEPIA_PATH)simfunc/simdist.h $(SEPIA_PATH)simfunc/simvect.h \
	$(SEPIA_PATH)simfunc/editvect.h $(SEPIA_PATH)simfunc/jaccvect.h \
	$(SEPIA_PATH)cluster/cluster.h $(SEPIA_PATH)cluster/ftable.h \
	$(SEPIA_PATH)cluster/clusters.h \
  $(TRUNK_PATH)util/simfuncs.h


.PHONY: clean
clean:
	-rm *.o *~