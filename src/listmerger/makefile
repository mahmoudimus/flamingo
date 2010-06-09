
# $Id: makefile 5149 2010-03-24 23:37:18Z abehm $

# Copyright (C) 2010 by The Regents of the University of California

# Redistribution of this file is permitted under the terms of 
# the BSD license.

# Date: 05/14/2007
# Author: Chen Li, Jiaheng Lu

include ../makefile.inc

# FOR THE LOOPTIMER
LDFLAGS = -lrt

all: liblistmerger.a unittest example

liblistmerger.a: liblistmerger.a(heapmerger.o \
		 counttable.o utilities.o  heap.o \
	         showutil.o scancountmerger.o \
                 divideskipmerger.o mergeoptmerger.o \
                 mergeskipmerger.o mergeskipplusmerger.o \
                 mergeskiptest.o ondiskmergersimple.o \
		 ondiskmergeradapt.o \
		 ondiskmergerfixedlists.o)

counttable.o: counttable.h

heap.o: heap.h

heap.h: util/array.h
	touch $@

utilities.o: utilities.h heap.h showutil.h 

utilities.h: util/array.h
	touch $@

showutil.o: showutil.h heap.h

showutil.h: util/array.h  
	touch $@

listmerger.h: util/array.h  counttable.h utilities.h	
	touch $@

heapmerger.h: listmerger.h heap.h showutil.h 
	touch $@
heapmerger.o: heapmerger.h 

scancountmerger.h: listmerger.h 
	touch $@
scancountmerger.o: scancountmerger.h

divideskipmerger.h: listmerger.h  utilities.h 
	touch $@
divideskipmerger.o: divideskipmerger.h

mergeoptmerger.h: listmerger.h heap.h showutil.h \
                  utilities.h util/array.h
	touch $@
mergeoptmerger.o: mergeoptmerger.h

mergeskipmerger.h: listmerger.h  utilities.h   heap.h \
                   showutil.h
	touch $@
mergeskipmerger.o: mergeskipmerger.h

mergeskipplusmerger.h: listmerger.h  utilities.h   heap.h \
	      showutil.h
	touch $@
mergeskipplusmerger.o: mergeskipplusmerger.h

#only for test, please delete 
mergeskiptest.h: listmerger.h  utilities.h   heap.h \
                   showutil.h
	touch $@
mergeskiptest.o: mergeskiptest.h

ondiskmergerabs.h:	filtertree/gramlist.h \
			filtertree/gramlistondisk.h \
			filtertree/statsutil.h
			touch $@

ondiskmergersimple.h:	ondiskmergerabs.h \
			divideskipmerger.h
			touch $@	
ondiskmergersimple.o:	ondiskmergersimple.h

ondiskmergeradapt.h:	ondiskmergerabs.h
				touch $@	
ondiskmergeradapt.o:	ondiskmergeradapt.h


ondiskmergerfixedlists.h:	ondiskmergerabs.h
				touch $@	
ondiskmergerfixedlists.o:	ondiskmergerfixedlists.h


heapmerger-pq.o: listmerger.h util/array.h

unittest.o: listmerger.h showutil.h util/array.h 

unittest:  liblistmerger.a \
           $(APPSTRINGROOT)/util/libutil.a


example.o: listmerger.h showutil.h util/array.h 

example:  liblistmerger.a \
           $(APPSTRINGROOT)/util/libutil.a


.PHONY: $(APPSTRINGROOT)/util/libutil.a
$(APPSTRINGROOT)/util/libutil.a:
	$(MAKE) -C $(APPSTRINGROOT)/util libutil.a 


.PHONY: clean
clean:
	-rm unittest example *.o *.a *~ *.ix *.rm *.tmp
	-$(MAKE) -C $(APPSTRINGROOT)/util clean

