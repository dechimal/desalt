
.PHONY: test clean

targets := require auto_fun parameter_pack newtype
LDFLAGS += -lstdc++
CXXFLAGS += -g -O0

test: $(targets)
	for i in $^; do ./$$i; done

clean:
	rm -f $(targets) $(targets:=.deps) $(targets:=.o)

%.deps: %.cpp
	$(CXX) -MM -MT'$@ $*.o' $(CPPFLAGS) $(CXXFLAGS) $< >$@
	[[ -s $@ ]] || rm -f $@

include $(targets:=.deps)
