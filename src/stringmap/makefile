#
# $Id: makefile 4143 2008-12-08 23:23:55Z abehm $
#
#  Copyright (C) 2003 - 2007 by The Regents of the University of
#  California
#
# Redistribution of this file is permitted under the terms of the 
# BSD license
#
# Date: March 2002
#
# Authors: Michael Ortega-Binderberger (miki (at) ics.uci.edu)
#          Liang Jin (liangj (at) ics.uci.edu)
#          Chen Li (chenli (at) ics.uci.edu)
#

SPLIT_TYPE= l

# .SUFFIXES: .obj .cpp .c .rsp .exp

CC = g++ 
LINK = $(CC)
#LINK = /opt/rational/releases/PurifyPlus.2003a.06.15/sun4_solaris2/bin/purify -demangle-program=`which c++filt` -cache-dir=/var/preserve/chenli/purify-cache  $(CC)

LINKFLAGS = -O3 -o
#LINKFLAGS = -g -o

LIB_DIRS        = -L.
LIBS            = -lrtree
#CFLAGS = -c -g
CFLAGS = -c

EXT = o
LIB= librtree.a
OBJS= editdistance.$(EXT) index.$(EXT) node.$(EXT) rect.$(EXT) split_$(SPLIT_TYPE).$(EXT) stringmap_unittest.$(EXT) stringmap.$(EXT) element.$(EXT) mapper.$(EXT)
AR = ar -rv $@
STRINGMAP_UNITTEST = stringmap_unittest

.c.$(EXT):
	$(CC) $(CFLAGS)  $<

.cpp.$(EXT):
	$(CC) $(CFLAGS)  $<

$(STRINGMAP_UNITTEST): $(OBJS)
	$(LINK) $(LINKFLAGS) $(STRINGMAP_UNITTEST) $(OBJS) -lm

clean:
	rm -f *.$(EXT) *.bin stringmap_unittest
