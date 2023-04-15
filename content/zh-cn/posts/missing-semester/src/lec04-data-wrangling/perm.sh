#!/usr/bin/false

_ALPHABET=({a..z})

function permute() {
  local n="${1}"
  if [[ $1 -eq 0 ]]; then
    echo "${2}"
    return
  fi
  for c in "${_ALPHABET[@]}"; do
    permute $(( $1 - 1 )) "${2}$c"
  done
}