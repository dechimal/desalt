
ROOT := $(shell pwd)
HEADERDIR := $(ROOT)/include

-include config.mk

ifeq ($(findstring clang,$(CXX)),clang)
CXX11FLAGS := -std=c++11
else
ifeq ($(findstring g++,$(CXX)),g++)
CXX11FLAGS := -std=gnu++11
endif
endif
CXXFLAGS += -Werror -Wall -Wextra -pedantic-errors -std=gnu++11
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
