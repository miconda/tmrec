# $Id: Makefile,v 1.1.1.1 2004-03-31 09:41:56 dcm Exp $
#
#

include ../Makefile.defs

#set some vars from the environment (and not make builtins)
CC := $(shell echo "$${CC}")

OS = $(shell uname -s)
ARCH=$(shell uname -m |sed -e s/i.86/i386/ -e s/sun4u/sparc64/)

#defines
ALLDEP=Makefile
CFLAGS=
DEFS=-DNAME='"$(NAME)"' -DVERSION='"$(RELEASE)"'

LIBS=
LDFLAGS=


#checks
ifeq ($(CC),)
	CC=gcc
endif

DEFS+= -DARCH='"$(ARCH)"' -DOS='"$(OS)"' -DCOMPILER='"$(CC)"' \
	 #-DEXTRA_DEBUG \

#more defines
LD= $(CC)
MKDEP=$(CC) -MM

#sources
sources=$(filter-out $(auto_gen), $(wildcard *.c) ) $(auto_gen)
#headers
headers=$(filter-out $(auto_gen), $(wildcard *.h) ) $(auto_gen)

objs=$(sources:.c=.o)
depends=$(sources:.c=.d)


#compiling mode
mode = debug
ifeq ($(mode),)
	mode = release
endif


#CFLAGS and LDFLAGS
ifeq ($(mode), release)
	CFLAGS=-g -O9 -funroll-loops  -Wcast-align $(PROFILE) -Wall
	LDFLAGS=-Wl,-O2 -Wl,-E
else
	CFLAGS=-g -Wcast-align -Wall
	LDFLAGS+=-g -Wl,-E
endif


#******************************** RULES ***************************************


#implicit rules
%.o:%.c  $(ALLDEP)  $(headers)
	$(CC) $(CFLAGS) $(DEFS) -c $< -o $@

%.d: %.c $(ALLDEP)
	@set -e; $(MKDEP) $(DEFS) $< \
	|  sed 's#\($*\)\.o[ :]*#\1.o $@ : #g' > $@; \
	[ -s $@ ] || rm -f $@


#common rules

.PHONY: all
all: $(BIN)

.PHONY: dep
dep: $(depends)

.PHONY: bin
$(BIN):  $(objs) $(ALLDEP)
	$(LD) $(LDFLAGS) $(objs) $(LIBS) -o $(BIN)


.PHONY: TAGS
TAGS:
	ctags -R .


.PHONY: dist
dist: clean
	cd ..; tar --exclude .*.swp --exclude ~* --exclude ~*.* --exclude tags\
			-zcf $(NAME)-$(RELEASE).tar.gz src/


.PHONY: clean
clean:
	@rm -fr $(BIN)
	@rm -f $(objs) $(NAME) $(objs:.o=.il) 2>/dev/null


.PHONY: proper
proper: clean
	-@rm -f $(depends) $(auto_gen) 2>/dev/null


ifeq (,$(MAKECMDGOALS))
include $(depends)
endif

