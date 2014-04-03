
ROOT := $(shell pwd)
HEADERDIR := $(ROOT)/include

-include config.mk

ifeq ($(findstring clang,$(CXX)),clang)
CXX1YFLAGS := -std=c++1y
else
ifeq ($(findstring g++,$(CXX)),g++)
CXX1YFLAGS := -std=gnu++1y
endif
endif
CXXFLAGS += -Werror -Wall -Wextra -pedantic-errors $(CXX1YFLAGS)
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
