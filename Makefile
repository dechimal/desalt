
ROOT := $(shell pwd)
HEADERDIR := $(ROOT)/include

-include config.mk

CXXFLAGS += -Werror -Wall -Wextra -pedantic-errors -Wno-parentheses -Wno-unused-parameter $(CXX1YFLAGS)
CPPFLAGS += -I$(HEADERDIR) -I.

CXXSTD ?= 14
ifeq ($(findstring clang,$(CXX)),clang)
  CXXFLAGS += -Wno-unused-local-typedef
endif

ifeq ($(CXXSTD),17)
  CXXFLAGS += -std=c++1z
else
ifeq ($(CXXSTD),1z)
  CXXFLAGS += -std=c++1z
else
  CXXFLAGS += -std=c++14
endif
endif

ifneq ($(BOOST_INCLUDE),)
  CPPFLAGS += -I$(BOOST_INCLUDE)
endif

export CPPFLAGS CXXFLAGS

.PHONY: test clean

test:
	$(MAKE) -C test test

clean:
	$(MAKE) -C test clean
