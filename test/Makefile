
.PHONY: test clean

targets := require auto_fun parameter_pack newtype while struct static_control format match
CXXFLAGS += -g -O0
match.deps match: CXXFLAGS += -std=c++1z

test: $(targets)
	for i in $^; do ./$$i; done

$(targets): % : %.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $< -o $@

clean:
	rm -f $(targets) $(targets:=.deps) $(targets:=.o)

%.deps: %.cpp
	$(CXX) -MM -MT'$@ $*.o' $(CPPFLAGS) $(CXXFLAGS) $< >$@
	[[ -s $@ ]] || rm -f $@

ifneq ($(MAKECMDGOALS),clean)
include $(targets:=.deps)
endif
