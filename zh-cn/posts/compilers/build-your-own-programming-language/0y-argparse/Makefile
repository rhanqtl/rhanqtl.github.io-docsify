FLEX := $(shell which flex)
BISON := $(shell which bison)
CC := gcc

.PHONY: clean

argparse: argparse.y argparse.l
	$(BISON) -vd argparse.y
	$(FLEX) argparse.l
	$(CC) lex.yy.c argparse.tab.c -o argparse

clean:
	rm argparse.tab.{c,h} argparse.output lex.yy.c argparse >/dev/null 2>&1 || :
