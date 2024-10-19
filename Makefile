
ROOT := $(shell pwd)
HEADERDIR := $(ROOT)/include

-include config.mk

CXXFLAGS += -Werror -Wall -Wextra -pedantic-errors -Wno-parentheses -Wno-unused-parameter $(CXX1YFLAGS)
CPPFLAGS += -I$(HEADERDIR)

CXXSTD ?= 14
ifeq ($(findstring clang,$(CXX)),clang)
  CXXFLAGS += -Wno-unused-local-typedef
endif

CXXSTD ?= 14
CXXFLAGS += -std=c++$(CXXSTD)

ifneq ($(BOOST_INCLUDE),)
  CPPFLAGS += -I$(BOOST_INCLUDE)
endif

export CPPFLAGS CXXFLAGS

.PHONY: test clean

test:
	$(MAKE) -C test test

clean:
	$(MAKE) -C test clean
