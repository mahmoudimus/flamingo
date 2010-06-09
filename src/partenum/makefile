# $Id: makefile 4143 2008-12-08 23:23:55Z abehm $

# Copyright (C) 2007 by The Regents of the University of California

# The implementation of the PartEnum algorithm invented by Microsoft
# researchers is limited to non commercial use, which would be covered
# under the royalty free covenant that Microsoft made public.

# Date: 01/30/2007
# Author: Rares Vernica <rares (at) ics.uci.edu>

include ../makefile.inc

all: libpartenum.a unittest example


libpartenum.a: libpartenum.a(partenum.o)


partenum.o: partenum.h \
	util/simfuncs.h \
	util/input.h \
	util/misc.h

partenum.h: util/appsearch.h \
	util/gram.h


example.o: partenum.h \
	util/input.h util/misc.h

example: example.o libpartenum.a $(APPSTRINGROOT)/util/libutil.a


unittest.o: partenum.h \
	util/output.h

unittest: unittest.o libpartenum.a $(APPSTRINGROOT)/util/libutil.a


.PHONY: util/libutil.a
$(APPSTRINGROOT)/util/libutil.a:
	$(MAKE) -C $(APPSTRINGROOT)/util libutil.a


.PHONY: clean
clean:
	-rm example unittest *.a *.o *~ *.bin
	-$(MAKE) -C $(APPSTRINGROOT)/util clean