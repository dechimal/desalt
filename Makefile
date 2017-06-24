
ROOT := $(shell pwd)
HEADERDIR := $(ROOT)/include

-include config.mk

CXXFLAGS += -Werror -Wall -Wextra -pedantic-errors -Wno-parentheses -Wno-unused-parameter $(CXX1YFLAGS)
CPPFLAGS += -I$(HEADERDIR) -I.
ifeq ($(findstring clang,$(CXX)),clang)
CXX1YFLAGS := -std=c++14
CXXFLAGS += -Wno-unused-local-typedef
else
ifeq ($(findstring g++,$(CXX)),g++)
CXX1YFLAGS := -std=gnu++14
endif
endif
ifneq ($(BOOST_INCLUDE),)
CPPFLAGS += -I$(BOOST_INCLUDE)
endif

export

.PHONY: test clean

test:
	$(MAKE) -C test test

clean:
	$(MAKE) -C test clean
