
ROOT := $(shell pwd)
HEADERDIR := $(ROOT)/include

-include config.mk

ifeq ($(findstring clang,$(CXX)),clang)
CXX1YFLAGS := -std=c++1z
else
ifeq ($(findstring g++,$(CXX)),g++)
CXX1YFLAGS := -std=gnu++1z
endif
endif
CXXFLAGS += -Werror -Wall -Wextra -pedantic-errors -Wno-parentheses -Wno-unused-parameter -Wno-unused-local-typedef $(CXX1YFLAGS)
CPPFLAGS += -I$(HEADERDIR) -I.
ifneq ($(BOOST_INCLUDE),)
CPPFLAGS += -I$(BOOST_INCLUDE)
endif

export

.PHONY: test clean

test:
	$(MAKE) -C test test

clean:
	$(MAKE) -C test clean
