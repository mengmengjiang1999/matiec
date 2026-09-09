## Current state

The installable embedding package contains a flattened static `libmatiec.a`.
Component archives are built without an explicit PIC/visibility policy, and the
package smoke test only verifies static consumption. The public header already
marks C ABI functions with `MATIEC_API` and publishes API version 1.3.0.

## Options explored

1. Re-list every source in one top-level libtool target. This would duplicate the
   recursive Automake source graph and generated Flex/Bison rules.
2. Convert every component archive into a libtool convenience library. This is
   architecturally clean but unnecessarily rewrites all internal build targets.
3. Compile existing component objects as PIC with hidden visibility, then link
   their archives into one platform-native shared library. This preserves the
   current graph and lets the existing `MATIEC_API` annotations define exports.

## Conclusion

Use option 3. Produce an ABI-major shared artifact and unversioned/development
links on ELF and Darwin, retain `libmatiec.a`, test exported symbols, and execute
installed consumers against both link modes.
