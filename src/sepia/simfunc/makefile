# $Id: makefile 4143 2008-12-08 23:23:55Z abehm $

# Copyright (C) 2007 by The Regents of the University of California

# Redistribution of this file is permitted under the terms of the 
# BSD license

# Date: 03/25/2007
# Author: Rares Vernica <rares (at) ics.uci.edu>

TRUNK_PATH = ../../
SEPIA_PATH = ../

include $(SEPIA_PATH)makefile.defs

all: simdist.o editvect.o jaccvect.o simvect.o

simdist.o: simdist.h \
	$(TRUNK_PATH)util/simfuncs.h

editvect.o: editvect.h \
	$(TRUNK_PATH)util/simfuncs.h

jaccvect.o: jaccvect.h \
	$(TRUNK_PATH)util/simfuncs.h $(TRUNK_PATH)util/gram.h

simvect.o: simvect.h editvect.h \
	$(TRUNK_PATH)util/simfuncs.h

.PHONY: clean
clean:
	-rm *.o *~