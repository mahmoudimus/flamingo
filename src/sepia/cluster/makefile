# $Id: makefile 4143 2008-12-08 23:23:55Z abehm $

# Copyright (C) 2007 by The Regents of the University of California

# Redistribution of this file is permitted under the terms of the 
# BSD license

# Date: 03/26/2007
# Author: Rares Vernica <rares (at) ics.uci.edu>

TRUNK_PATH = ../../
SEPIA_PATH = ../

include $(SEPIA_PATH)makefile.defs

all: ftable.o cluster.o clusters.o lexic.o medoids.o

ftable.o: ftable.h \
	$(SEPIA_PATH)simfunc/simvect.h $(SEPIA_PATH)simfunc/editvect.h \
  $(TRUNK_PATH)util/simfuncs.h

cluster.o: cluster.h ftable.h \
	$(SEPIA_PATH)simfunc/simdist.h $(SEPIA_PATH)simfunc/simvect.h \
	$(SEPIA_PATH)simfunc/editvect.h $(SEPIA_PATH)simfunc/jaccvect.h \
	$(TRUNK_PATH)util/simfuncs.h

clusters.o: clusters.h ftable.h \
	$(SEPIA_PATH)sample.h \
	$(SEPIA_PATH)simfunc/simdist.h $(SEPIA_PATH)simfunc/simvect.h \
  $(SEPIA_PATH)simfunc/editvect.h $(SEPIA_PATH)simfunc/jaccvect.h \
	$(SEPIA_PATH)ppdtable/ppdtable.h $(SEPIA_PATH)ppdtable/ppdentry.h \
	$(SEPIA_PATH)ppdtable/ppdsample.h $(SEPIA_PATH)ppdtable/ppdtriple.h \
	$(SEPIA_PATH)ppdtable/allrand.h $(SEPIA_PATH)ppdtable/indexentry.h \
  $(SEPIA_PATH)ppdtable/closelex.h $(SEPIA_PATH)ppdtable/closerand.h \
	$(SEPIA_PATH)ppdtable/closeunique.h \
	$(TRUNK_PATH)util/simfuncs.h cluster.h 	

lexic.o: lexic.h cluster.h clusters.h ftable.h \
	$(SEPIA_PATH)sample.h \
	$(SEPIA_PATH)ppdtable/ppdtable.h $(SEPIA_PATH)ppdtable/ppdentry.h \
	$(SEPIA_PATH)simfunc/simdist.h $(SEPIA_PATH)simfunc/simvect.h \
  $(SEPIA_PATH)simfunc/editvect.h $(SEPIA_PATH)simfunc/jaccvect.h \
	$(TRUNK_PATH)util/simfuncs.h

medoids.o: medoids.h clusters.h cluster.h ftable.h \
	$(SEPIA_PATH)sample.h \
  $(SEPIA_PATH)ppdtable/ppdtable.h $(SEPIA_PATH)ppdtable/ppdentry.h \
	$(SEPIA_PATH)simfunc/simdist.h $(SEPIA_PATH)simfunc/simvect.h \
	$(SEPIA_PATH)simfunc/editvect.h $(SEPIA_PATH)simfunc/jaccvect.h \
	$(TRUNK_PATH)util/simfuncs.h \

.PHONY: clean
clean:
	-rm *.o *~