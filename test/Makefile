
.PHONY: test clean

targets := require auto_fun parameter_pack
LDFLAGS += -lstdc++

test: $(targets)

clean:
	rm -f $(targets) $(targets:=.deps) $(targets:=.o)

%.deps: %.cpp
	$(CXX) -MM -MT'$@ $*.o' $(CPPFLAGS) $(CXXFLAGS) $< >$@
	[[ -s $@ ]] || rm -f $@

include $(targets:=.deps)
