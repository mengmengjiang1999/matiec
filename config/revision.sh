#!/bin/sh

set -eu

source_root=${1:-.}

if revision=$(git -C "$source_root" rev-parse --short=12 HEAD 2>/dev/null); then
  printf '%s\n' "$revision"
  exit 0
fi

archive_file="$source_root/.git_archival.txt"
if test -r "$archive_file"; then
  revision=$(sed -n 's/^revision: //p' "$archive_file" | sed -n '1p')
  case "$revision" in
    ''|'$Format:'*) ;;
    *)
      printf '%s\n' "$revision"
      exit 0
      ;;
  esac
fi

printf '%s\n' unknown
