# -*- makefile -*-
#
# $Id: makefile.inc 5162 2010-03-25 23:34:39Z abehm $
#
# Copyright (C) 2007 by The Regents of the University of California
#
# Redistribution of this file is permitted under the terms of the BSD
# license
#
# This is a global makefile to be included by each local makefile.
#
# Date: 10/05/2007
#
# Author: Chen Li <chenli (at) ics.uci.edu>
#         Rares Vernica <rares (at) ics.uci.edu>
#

# set CODEBASEROOT to the absolute path of the source code directory
#
# e.g., if you put the code in /home/joe/flamingo/src do:
#       CODEBASEROOT = /home/joe/flamingo-3.0/src

# e.g., CODEBASEROOT = /home/joe/flamingo-3.0/src
CODEBASEROOT =
APPSTRINGROOT = $(CODEBASEROOT)

VPATH = $(APPSTRINGROOT)

CC = g++

### Cygwin:
# CC = g++-4
# CXX = g++-4

CPPFLAGS = -Wall -I$(APPSTRINGROOT) -O3 -DNDEBUG
# CPPFLAGS = -Wall -I$(APPSTRINGROOT) -O3
# CPPFLAGS = -Wall -I$(APPSTRINGROOT) -g -pg

ifndef CODEBASEROOT
$(error -- - EDIT MAKEFILE.INI - -- Please edit makefile.ini and set the CODEBASEROOT variable to the absolute path of the source code directory. e.g., if you put the code in /home/joe/flamingo-3.0/src do: CODEBASEROOT = /home/joe/flamingo-3.0/src)
 endif
