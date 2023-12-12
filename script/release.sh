#!/bin/bash

if [[ -z "${1}" ]]; then
  echo "usage: ${0} <version>" >&2
  exit 1
fi

# root directory
root="$(cd "$(dirname "$(readlink -f "${BASH_SOURCE[0]}")")/.." && pwd)"

# parse version
version="${1}"
major="${version%%.*}"
minor="${version%.*}"; minor="${minor#*.}"
patch="${version##*.}"

# check configure.ac
configure_version="$(awk '/m4_define.*SQIDS_VERSION_STRING/ { gsub(/[\[\]()]/, "", $2); print($2) }' "${root}/configure.ac")"
if [[ "${configure_version}" != "${version}" ]]; then
  echo "version mismatch: check ${root}/configure.ac" >&2
  exit 1
fi

# check sqids.h
sqids_version="$(awk '/SQIDS_VERSION_STRING/ { gsub(/"/, "", $3); print($3) }' "${root}/src/sqids.h")"
sqids_major="$(awk '/SQIDS_VERSION_MAJOR/ { print($3); } ' "${root}/src/sqids.h")"
sqids_minor="$(awk '/SQIDS_VERSION_MINOR/ { print($3); } ' "${root}/src/sqids.h")"
sqids_patch="$(awk '/SQIDS_VERSION_PATCH/ { print($3); } ' "${root}/src/sqids.h")"
if [[ "${sqids_version}" != "${version}" ||
      "${sqids_major}" != "${major}" ||
      "${sqids_minor}" != "${minor}" ||
      "${sqids_patch}" != "${patch}" ]]; then
  echo "version mismatch: check ${root}/src/sqids.h" >&2
  exit 1
fi

# abort if repo is dirty
if [[ -n "$(git status --porcelain 2>/dev/null)" ]]; then
  echo "error: git repo is dirty" >&2
  exit 1
fi

# proceed to tagging
gh release create "v${version}" --title "v${version}"
