#!/usr/bin/env bash

_output=output.txt

while bash maybe_fail.sh >> "${_output}" 2>&1; do
  :
done

cat "${_output}"