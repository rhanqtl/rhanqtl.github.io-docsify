FLEX := $(shell which flex)
BISON := $(shell which bison)
CC := gcc

.PHONY: clean

yatl: yatl.y yatl.l
	$(BISON) -vd yatl.y
	$(FLEX) yatl.l
	$(CC) lex.yy.c yatl.tab.c -o yatl

clean:
	rm yatl.tab.{c,h} yatl.output lex.yy.c yatl >/dev/null 2>&1 || :
