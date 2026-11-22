# Release note items

Each pull request that changes what a user runs, gets back, reads, or compiles
(the executable, the library and its API, the meshes they produce, the release
archives, or the README and license) adds one TOML file here. A `refactor`,
`ci`, `test`, `style`, or `chore` pull request that changes none of these adds
none. Name the file after the change (for example `library.toml`).

```toml
section = "fixes"
description = "The ARM macOS executable could hang on some inputs (#13). ..."
```

- `section` is `features`, `fixes`, `build`, or `documentation`.
- `description` is one to four sentences for someone using Triangle: what was
  wrong or what is new, and what it means for them, with the issue or pull
  request number. Detail belongs in the pull request.

`pixi run check-release-notes` checks the items, and `pixi run release-notes`
prints them as they will appear. The release pull request runs
`python scripts/release_notes.py release --version <v>`, which moves the items
into a `CHANGELOG.md` entry; the release workflow uses that entry as the
release description.
