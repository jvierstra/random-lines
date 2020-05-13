SHELL=/bin/sh

PREFIX ?= $(HOME)/.local
BINDIR ?= $(PREFIX)/bin
INCLUDEDIR ?= $(PREFIX)/include
LIBDIR ?= $(PREFIX)/lib

.SUFFIXES:
.SUFFIXES: .cc .o .d .hh .a
.LIBPATTERNS = lib%.a lib%.so

CC=$(CXX)
CPPFLAGS += -Iinclude
CPPFLAGS += -g -Wno-deprecated -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -O2
CXXFLAGS += -fopenmp

LDFLAGS += -fopenmp
LDLIBS += -lz

vpath lib%.a src

E := $(SUFFIX)
O := $(SUFFIX).o
A := $(SUFFIX).a
D := .d 

#Override implicit rule
%: %.cc

#Dependency file creation rule
%$(D): %.cc
	@$(CXX) -MM -MT '$(*)$$(O) $@' $(CPPFLAGS) $< > $@

#Object creation rule
%$(O): %.cc
	$(COMPILE.cc) $(OUTPUT_OPTION) $<

#Library creation rule
%$(A):
	rm -f $@
	$(AR) qc $@ $^

DIRS:=
SRCS:=$(wildcard src/*.cc)
LIBS:=
OBJS=$(SRCS:.cc=$(O))

MAINS:=src/random-lines src/random-lines-pairs
MAINS_OBJS:=$(foreach bin,$(MAINS), $(bin)$(O))
SHARED_OBJS:=$(filter-out $(MAINS_OBJS),$(OBJS))

BINS:= $(foreach bin,$(MAINS),$(bin)$(E))


DEPENDS=$(SRCS:.cc=$(D))

#include other makefiles
include $(wildcard */include.mk)

ifeq (,$(findstring clean,$(MAKECMDGOALS)))
-include $(DEPENDS)
endif

.PHONY: all clean install depends $(DIRS)

$(BINS): $(SHARED_OBJS)

ALL_TARGETS = $(LIBS) $(BINS)

all: $(LIBS) $(BINS)

depends: $(DEPENDS)

clean:
	rm -f $(DEPENDS) $(OBJS) $(BINS) $(LIBS)

cleandeps:
	rm -f $(DEPENDS)

# $(DIRS):
# 	$(MAKE) $(filter $@/%, $(ALL_TARGETS))

$(BINDIR):
	mkdir -p $(BINDIR)

install: $(BINS) $(BINDIR)
	install -sp $(BINS) $(BINDIR)

