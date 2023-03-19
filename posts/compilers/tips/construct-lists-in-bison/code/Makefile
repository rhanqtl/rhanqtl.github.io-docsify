FLEX     := $(shell which flex)
BISON    := $(shell which bison)
CC       := $(shell which clang++)
CXXFLAGS := -std=c++14
TARGETS  := leftrec rightrec

.PHONY: all clean

all: $(TARGETS)

$(TARGETS): %: %.yy list.hpp
	$(BISON) --no-lines -o $@.cpp $<
	$(CC) $(CXXFLAGS) $@.cpp -o $@

clean:
	rm {right,left}rec.{cpp,hpp} {left,right}rec.output {left,right}rec >/dev/null 2>&1 || :
