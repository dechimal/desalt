
.PHONY: test clean

targets := require auto_fun parameter_pack newtype while struct static_control format
CXXFLAGS += -g -O0

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
