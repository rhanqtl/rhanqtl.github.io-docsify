LEX      := $(shell which flex)
YACC     := $(shell which bison)

CC       := $(shell which gcc)
CXXFLAGS := -Iinclude -lm

TEMP     := calc.tab.c calc.tab.h calc.lex.c

vpath %.c src
vpath %.l src
vpath %.y src
vpath %.h include

.PHONY: all run clean
.INTERMEDIATE: $(TEMP)

all: $(TEMP)
	$(CC) $(CXXFLAGS) -o calc $(filter %.c,$^)

calc.tab.c calc.tab.h: calc.y location.h
	$(YACC) -H $<

calc.lex.c: calc.l location.h
	$(LEX) -o $@ $<

test: all
	./calc tests/normal.txt

run: all
	./calc

clean:
	rm calc.tab.{c,h} calc.lex.c calc || :
