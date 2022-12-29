#!/usr/bin/env bash

source perm.sh

permute 2 | [[ $(wc -l) -eq $(( 26 * 26 )) ]] && echo "PASS" || echo "FAIL"