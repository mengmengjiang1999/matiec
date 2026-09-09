#!/bin/sh
set -eu

if test "$#" -lt 3; then
  echo "usage: $0 AR OUTPUT ARCHIVE..." >&2
  exit 2
fi

archive_tool=$1
output=$2
shift 2

case "$output" in
  libmatiec.a|*/libmatiec.a) ;;
  *) echo "refusing unexpected output path: $output" >&2; exit 2 ;;
esac

work_directory=$(mktemp -d "${TMPDIR:-/tmp}/matiec-archive.XXXXXX")
trap 'rm -rf "$work_directory"' EXIT HUP INT TERM

archive_index=0
for archive in "$@"; do
  archive_index=$((archive_index + 1))
  "$archive_tool" t "$archive" | while IFS= read -r member; do
    test -n "$member" || continue
    "$archive_tool" p "$archive" "$member" > \
      "$work_directory/$archive_index-$member"
  done
done

rm -f "$output"
"$archive_tool" cr "$output" "$work_directory"/*.o
