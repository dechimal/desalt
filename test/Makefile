
.PHONY: test clean

targets := require auto_fun parameter_pack newtype while struct static_control format match record record-2a
CXXFLAGS += -g -O0
match.o record.o match.deps record.deps: CXXFLAGS += -std=c++1z
record-2a.deps record-2a.o: CPPFLAGS += -DDESALT_CXX20
record-2a.deps record-2a.o: CXXFLAGS += -std=c++2a

test: $(targets)
	for i in $^; do ./$$i; done

$(targets): % : %.o
	$(CXX) $(CPPFLAGS) $(LDFLAGS) $< -o $@

define loop
$o.source := $(o:.o=.cpp)
endef
$(foreach o,$(targets:=.o),$(eval $(value loop)))
record-2a.o.source := record.cpp

define loop
$o: $($o.source)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $< -o $@ -c
endef
$(foreach o,$(targets:=.o),$(eval $(value loop)))

clean:
	rm -f $(targets) $(targets:=.deps) $(targets:=.o)

%.deps: %.cpp
	$(CXX) -MM -MT'$@ $*.o' $(CPPFLAGS) $(CXXFLAGS) $< -MF $@
	test -s $@ || rm -f $@

record-2a.deps: record.cpp
	$(CXX) -MM -MT'$@ record-2a.o' $(CPPFLAGS) $(CXXFLAGS) $< -MF $@
	test -s $@ || rm -f $@

ifneq ($(MAKECMDGOALS),clean)
include $(targets:=.deps)
endif
