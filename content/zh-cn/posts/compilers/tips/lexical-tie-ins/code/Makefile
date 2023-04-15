FLEX := $(shell which flex)
BISON := $(shell which bison)
CC := gcc

.PHONY: clean run

tie_in: tie_in.y tie_in.l
	$(BISON) -vd tie_in.y
	$(FLEX) tie_in.l
	$(CC) lex.yy.c tie_in.tab.c -o $@

run: tie_in
	./$<

clean:
	rm tie_in.tab.{c,h} tie_in.output lex.yy.c tie_in >/dev/null 2>&1 || :
