
ROOT := $(shell pwd)
HEADERDIR := $(ROOT)/include

-include config.mk

CXXFLAGS += -std=c++1y -Werror -Wall -Wextra -pedantic-errors
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
