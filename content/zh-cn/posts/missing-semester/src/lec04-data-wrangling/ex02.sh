#!/usr/bin/env bash

cat "$1" | tr -s 'A-Z' 'a-z' | egrep '^.*a.*a.*a.*$' | grep -v "'s$" | grep -o '..$' | sort | uniq -c | sort -k1 -r
