# Design: replace namespace spelling bridge

## Two native parser passes

Experimental compilation always runs the existing lightweight parser prepass.
Namespace grammar actions record canonical namespace names, visibility, imports,
and member token classes in `ParserState`. This state survives scanner teardown
between prepass and definitive parse but is cleared when the context starts a new
compilation.

## Lexer lookup

Flex recognizes a dotted identifier as one lexical candidate. It resolves that
candidate only when its prefix names a registered namespace, preserving object
method syntax for the following change. Unqualified names resolve first in the
current namespace and then through visible USING directives. Ambiguous,
inaccessible, and unknown qualified names produce deterministic diagnostics.

## AST and includes

No source bytes are replaced. Namespace and USING nodes retain original source
ranges and include filenames. Because include streams enter the same scanner and
parser, their declarations participate in both the prepass registry and final AST.
