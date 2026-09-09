# Proposal: harden embedding concurrency

## Why

Embedding correctness depends on state isolation under repeated mixed workloads,
not only a single happy-path parallel compile. Race detection should enforce
that contract continuously.

## What changes

- Stress many C contexts over repeated bounded batches and context reuse.
- Mix valid, parser-failing, cancelled, and virtual-include jobs in each batch.
- Inject resolver and output callback exceptions and verify failure containment.
- Verify callback counts and result ordering remain context-local.
- Add a Linux ThreadSanitizer CI gate and local `make check-tsan` entry point.
- Document concurrency ownership and the deterministic test strategy.
