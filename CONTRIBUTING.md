# Contributing

Thanks for your interest in contributing to NOVERA Legacy. Bug fixes,
protocol work, tooling improvements, and documentation are all welcome.

## Getting started

1. Build the tree — see [docs/BUILDING.md](docs/BUILDING.md).
2. Set up the databases — see [docs/DATABASE.md](docs/DATABASE.md).

## Ground rules

- **Win32 / x86 only.** The wire protocol depends on 32-bit layout;
  64-bit porting PRs will not be accepted.
- **Keep diffs minimal and targeted.** This is a legacy codebase — do not
  mass-modernize or reformat untouched code.
- **Wire protocol changes need justification.** If a pull request changes
  packet structure (field order, sizes, framing), describe the compatibility
  impact in detail.
- **No client binaries or assets.** Contributions must contain source or
  tooling only — never game client files or copyrighted media.

## Commits

Follow [Conventional Commits](https://www.conventionalcommits.org/), in
lowercase English: `feat:`, `fix:`, `docs:`, `chore:`.

## Pull requests

- Keep pull requests small and focused — one logical change per PR.
- Describe **what** changed and **why**.
- Make sure **both Debug and Release** configurations build before
  submitting.

## Issues

When reporting bugs, include a description of the problem, steps to
reproduce, and relevant logs.

## License

By contributing, you agree that your contributions will be licensed under
the **GNU GPL v3**, the same license as the project.
