#!/usr/bin/false

function pidwait() {
  local pid=$1
  while kill -0 $pid &> /dev/null; do
    sleep 1
  done
}