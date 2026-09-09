#!/bin/sh

set -eu

script_dir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
project_root=$(CDPATH= cd -- "$script_dir/.." && pwd)

package_version=$(sed -n \
  's/^AC_INIT(\[matiec\], \[\([^]]*\)\].*/\1/p' \
  "$project_root/configure.ac")
api_major=$(sed -n \
  's/^#define MATIEC_API_VERSION_MAJOR \([0-9][0-9]*\)u$/\1/p' \
  "$project_root/include/matiec/api.h")
api_minor=$(sed -n \
  's/^#define MATIEC_API_VERSION_MINOR \([0-9][0-9]*\)u$/\1/p' \
  "$project_root/include/matiec/api.h")
api_patch=$(sed -n \
  's/^#define MATIEC_API_VERSION_PATCH \([0-9][0-9]*\)u$/\1/p' \
  "$project_root/include/matiec/api.h")
api_version=$api_major.$api_minor.$api_patch
runtime_version=$(sed -n \
  's/.*matiec_api_version_string(void) { return "\([^"]*\)"; }.*/\1/p' \
  "$project_root/compiler/c_api_version.cc")

fail() {
  echo "release version check failed: $*" >&2
  exit 1
}

test -n "$package_version" || fail "cannot read package version"
test "$package_version" = "$api_version" ||
  fail "package $package_version != public API $api_version"
test "$package_version" = "$runtime_version" ||
  fail "package $package_version != runtime API $runtime_version"
grep -q "MATIEC_SHARED_REAL=libmatiec\.so\.$package_version" \
  "$project_root/configure.ac" || fail "ELF shared-library version differs"
grep -q "MATIEC_SHARED_REAL=libmatiec\.$package_version\.dylib" \
  "$project_root/configure.ac" || fail "Darwin shared-library version differs"

if test -n "${RELEASE_TAG:-}"; then
  test "$RELEASE_TAG" = "v$package_version" ||
    fail "tag $RELEASE_TAG != v$package_version"
fi

echo "release version $package_version is consistent"
