#!/usr/bin/env bash

_last_dir=$(pwd)

function macro() {
  _last_dir=$(pwd)
}

function polo() {
  cd "${_last_dir}"
}