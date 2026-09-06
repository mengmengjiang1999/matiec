## Context

The compiler currently has boolean language-extension switches but no standard-edition model. Its default grammar is rooted in the IEC 61131-3 second-edition final draft, selected third-edition constructs are optional, and MATIEC-specific extensions coexist with both. IEC 61131-3:2025 is copyrighted and is not available to this experimental project as a complete normative source; only public IEC metadata, public national-standard planning records, implementation evidence, vendor documentation, and cross-compiler observations may be used.

The immediate need is therefore not to guess new grammar. It is to create a trustworthy boundary between preserved behavior, publicly confirmed evolution, provisional compatibility behavior, and unknown requirements. The existing compiler context, structured options, semantic pass manager, and regression suite provide suitable integration points.

## Goals / Non-Goals

**Goals:**

- Preserve the current language as an explicitly named legacy profile.
- Establish an experimental 2025-oriented profile whose name and diagnostics cannot be mistaken for a conformance claim.
- Give every language feature a stable identifier, implementation state, evidence classification, owning profile, and verification links.
- Make future lexer, parser, semantic, library, and generator changes opt in through a profile or separately named extension.
- Keep profile behavior testable through the existing regression entry point.
- Split the large modernization route into independent OpenSpec changes.

**Non-Goals:**

- Claim complete or certified IEC 61131-3:2025 conformance.
- Reproduce, redistribute, or derive repository documentation by copying the IEC publication.
- Guess the contents of IEC 61131-3:2025 Annex B.
- Add UTF-8 strings, object-oriented syntax, namespaces, graphical LD/FBD input, or other new language constructs in this foundation change.
- Remove IL or change legacy generated C behavior.

## Decisions

### 1. Model profiles separately from extensions

`CompilerOptions` will gain a language-profile value with at least `legacy` and `iec61131-3:2025-experimental`. Existing switches such as references, safe types, relaxed type equivalence, and non-literal array bounds remain explicit extensions until a verified profile rule supersedes them.

This avoids treating a historical collection of booleans as if it were one standards edition. A single collection of implicit defaults was rejected because it would make compatibility regressions and standards claims impossible to audit.

### 2. Keep legacy as the migration default

The initial default remains the existing MATIEC behavior. Selecting the experimental profile must be explicit, and its first implementation may intentionally accept exactly the same language while profile-specific features are developed.

Changing the default immediately was rejected because no complete public fourth-edition requirements set is available and existing users require stable parsing and C output.

### 3. Use evidence levels, not guessed conformance labels

Each catalogue entry will use one of these evidence levels:

- `implementation-verified`: proven from this repository and its tests; says nothing about standard status.
- `official-public`: supported by a public page from IEC or a national standards body.
- `cross-vendor-provisional`: observed consistently in multiple independent implementations or vendor documents but not verified against normative text.
- `unverified`: known question requiring authoritative material.

Evidence and implementation status are separate fields. For example, current IL support is `implementation-verified`, while its precise fourth-edition classification remains `unverified` until an authoritative public source establishes the Annex B result.

### 4. Maintain one human-readable catalogue before generating it

The first source of truth will be `docs/standards/iec61131-3-evolution.zh-CN.md`. It records current implementation coverage and decisions without duplicating the detailed user syntax manual. A later implementation task may move structured entries into code or data and generate the table, but only after the required fields and identifiers stabilize.

Starting with generated documentation was rejected because the project does not yet have stable feature identifiers or profile semantics.

### 5. Require a dedicated follow-up change for each language family

UTF-8 source/string support, references, namespace resolution, object-oriented elements, access variables, standard-library revisions, and legacy IL policy will each receive a focused OpenSpec change. Every proposal must identify its evidence, compatibility behavior, and tests.

One all-edition rewrite was rejected because parser, AST, semantic, runtime, and backend regressions would be difficult to isolate or revert.

### 6. Keep copyrighted standards outside the repository

The repository may contain public URLs, clause identifiers supplied by an authorized contributor, original summaries, implementation notes, and tests. It must not contain an IEC PDF, substantial copied tables, copied grammar, or a reconstructed substitute for the publication.

If authoritative text becomes available later, its license determines who may inspect it. The implementation and tests remain independently written and reviewable.

## Risks / Trade-offs

- **The experimental profile is mistaken for compliance** → Include `experimental` in the CLI value and user-visible diagnostics, and repeat the disclaimer in documentation.
- **Cross-vendor behavior is incorrectly attributed to IEC** → Keep `cross-vendor-provisional` distinct from `official-public` and store source links.
- **The feature catalogue becomes stale** → Require catalogue and profile-test updates in every language-change task and review checklist.
- **Profile plumbing creates behavior changes before features exist** → Initially map both profiles to the existing feature set and characterize output equality.
- **Later authoritative text contradicts an implementation** → Treat experimental behavior as revisable, document migration, and preserve legacy mode.
- **The work expands into a full compiler rewrite** → Deliver each language family as a separate OpenSpec change with an explicit non-goal list.

## Migration Plan

1. Land the decision record, baseline feature matrix, and OpenSpec requirements.
2. Add the profile enum, CLI parsing, help text, and embedding option without changing default language behavior.
3. Add stable feature identifiers and evidence/status metadata.
4. Characterize equality between legacy and the initially empty experimental profile.
5. Start the UTF-8 source and string-model change as the first publicly confirmed 2025 feature.
6. Add later features only through their dedicated changes.

Rollback consists of removing the new explicit profile selector while retaining the documentation and legacy behavior. No source-language migration is required by this foundation change.

## Open Questions

- Whether a future public Chinese adoption draft will permit clause-level traceability before its final publication.
- Whether the feature catalogue should ultimately be compiled C++ data, YAML consumed by tests, or generated from OpenSpec requirements.
- Which exact IL diagnostic belongs in the experimental profile after authoritative public evidence is available.
- Which public multi-vendor corpus is suitable for provisional behavior comparisons.
