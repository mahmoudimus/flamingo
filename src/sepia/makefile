# $Id: makefile 5029 2010-02-18 19:54:25Z rares $

# Copyright (C) 2007 by The Regents of the University of California

# Redistribution of this file is permitted under the terms of the 
# BSD license

# Date: 03/27/2007
# Author: Rares Vernica <rares (at) ics.uci.edu>

TRUNK_PATH = ../
SEPIA_PATH = ./

include makefile.defs

all: subdirs duplicate.o example unittest perftest

libsepia.a: 	libsepia.a( sepia.o \
			sample.o \
			cluster/cluster.o \
			cluster/clusters.o \
			cluster/ftable.o \
			cluster/lexic.o \
			cluster/medoids.o \
			freqest/errorcorr.o \
			freqest/freqest.o \
			freqest/predicate.o \
			freqest/record.o \
			freqest/test.o \
			freqest/update.o \
			ppdtable/allrand.o \
			ppdtable/closelex.o \
			ppdtable/closerand.o \
			ppdtable/closeunique.o \
			ppdtable/indexentry.o \
			ppdtable/ppdentry.o \
			ppdtable/ppdpair.o \
			ppdtable/ppdsample.o \
			ppdtable/ppdtable.o \
			ppdtable/ppdtriple.o \
			simfunc/simdist.o \
			simfunc/jaccvect.o \
			simfunc/simvect.o \
			simfunc/editvect.o)

sample.o: sample.h

duplicate.o: duplicate.h \
	$(TRUNK_PATH)util/simfuncs.h

sepia.o: sepia.h sample.h \
	simfunc/simdist.h simfunc/simvect.h \
	simfunc/editvect.h simfunc/jaccvect.h \
	cluster/cluster.h cluster/ftable.h \
	cluster/lexic.h cluster/medoids.h \
  cluster/clusters.h \
  ppdtable/ppdtable.h ppdtable/ppdentry.h \
	freqest/errorcorr.h freqest/record.h freqest/predicate.h freqest/freqest.h \
	$(TRUNK_PATH)util/simfuncs.h \
	$(TRUNK_PATH)util/input.h $(TRUNK_PATH)util/misc.h

example.o: sepia.h sample.h \
	simfunc/simdist.h simfunc/simvect.h \
	simfunc/editvect.h simfunc/jaccvect.h \
	cluster/cluster.h cluster/ftable.h \
	cluster/clusters.h \
  ppdtable/ppdtable.h ppdtable/ppdentry.h \
  freqest/errorcorr.h freqest/record.h freqest/predicate.h \
	$(TRUNK_PATH)util/simfuncs.h $(TRUNK_PATH)util/input.h

unittest.o: sepia.h sample.h \
	simfunc/simdist.h simfunc/simvect.h \
	simfunc/editvect.h simfunc/jaccvect.h \
	cluster/cluster.h cluster/ftable.h \
	cluster/clusters.h \
  ppdtable/ppdtable.h ppdtable/ppdentry.h \
  freqest/errorcorr.h freqest/record.h freqest/predicate.h \
	$(TRUNK_PATH)util/simfuncs.h $(TRUNK_PATH)util/input.h

perftest.o: sepia.h sample.h \
	simfunc/simdist.h simfunc/simvect.h \
	simfunc/editvect.h simfunc/jaccvect.h \
	cluster/cluster.h cluster/ftable.h \
	cluster/clusters.h \
  ppdtable/ppdtable.h ppdtable/ppdentry.h \
  freqest/errorcorr.h freqest/record.h freqest/predicate.h \
	$(TRUNK_PATH)util/simfuncs.h $(TRUNK_PATH)util/input.h


example: example.o sepia.o sample.o \
	simfunc/simvect.o simfunc/simdist.o \
	simfunc/editvect.o simfunc/jaccvect.o \
	cluster/clusters.o cluster/cluster.o cluster/ftable.o \
	cluster/lexic.o cluster/medoids.o \
	ppdtable/ppdtable.o ppdtable/ppdentry.o \
	ppdtable/ppdsample.o ppdtable/indexentry.o ppdtable/allrand.o \
	ppdtable/closelex.o ppdtable/closerand.o ppdtable/closeunique.o \
	freqest/errorcorr.o freqest/record.o freqest/predicate.o freqest/freqest.o \
	$(TRUNK_PATH)util/libutil.a

unittest: unittest.o sepia.o sample.o \
	simfunc/simvect.o simfunc/simdist.o \
	simfunc/editvect.o simfunc/jaccvect.o \
	cluster/clusters.o cluster/cluster.o cluster/ftable.o \
	cluster/lexic.o cluster/medoids.o \
	ppdtable/ppdtable.o ppdtable/ppdentry.o \
	ppdtable/ppdsample.o ppdtable/indexentry.o ppdtable/allrand.o \
	ppdtable/closelex.o ppdtable/closerand.o ppdtable/closeunique.o \
	freqest/errorcorr.o freqest/record.o freqest/predicate.o freqest/freqest.o \
	$(TRUNK_PATH)util/libutil.a

perftest: perftest.o sepia.o sample.o \
	simfunc/simvect.o simfunc/simdist.o \
	simfunc/editvect.o simfunc/jaccvect.o \
	cluster/clusters.o cluster/cluster.o cluster/ftable.o \
	cluster/lexic.o cluster/medoids.o \
	ppdtable/ppdtable.o ppdtable/ppdentry.o \
	ppdtable/ppdsample.o ppdtable/indexentry.o ppdtable/allrand.o \
	ppdtable/closelex.o ppdtable/closerand.o ppdtable/closeunique.o \
	freqest/errorcorr.o freqest/record.o freqest/predicate.o freqest/freqest.o \
	$(TRUNK_PATH)util/libutil.a


SUBDIRS = simfunc cluster ppdtable freqest

subdirs: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@ $(MAKECMDGOALS)

.PHONY: $(APPSTRINGROOT)/util/libutil.a
$(APPSTRINGROOT)/util/libutil.a: 
	$(MAKE) -C $(APPSTRINGROOT)/util libutil.a

cluster: simfunc

ppdtable: simfunc

freqest: cluster ppdtable

clean: subdirs
	-rm *.a *.o *~ example unittest perftest *.sep.*
	-$(MAKE) -C $(APPSTRINGROOT)/util clean

.PHONY: all clean subdirs $(SUBDIRS)
