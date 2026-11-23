# Changelog

Changes in each release of MODFLOW-ORG/triangle, from the release note items in
`changes/`. Releases before 1.6.1 are listed on the GitHub releases page.

## 1.6.2 (2026-11-23)

### Build and distribution

- Each release archive holds the executable and the library in bin/, with the headers in include/ and, on Windows, the import library in lib/ (#55). flopy's get-modflow (get-modflow --repo triangle) installs only what is in bin/, so it now installs the library beside the executable instead of in a lib/ folder inside the bindir.

## 1.6.1 (2026-11-23)

### New features

- Triangle is also built as a library, libtriangle, which a program can call in memory instead of running the executable with files, and which another Meson project can build as a subproject (#39). triangulate_status() returns an error status instead of ending the program and restores the caller's floating-point control word; triangle_free() frees the arrays it returns; and triangle_version(), triangle_release_version(), and triangle_copyright() report Triangle's version, this repository's release, and Triangle's copyright notice. Include triangle_status.h.
- A program using the library can decide which triangles Triangle refines with the u switch, by setting a test with triangle_set_unsuitable(), for example to refine triangles with an angle over a limit (#44). The test can also stop the run, and triangulate_status() then returns 2, since refinement toward some limits, such as 90 degrees, never finishes. The executable's u switch is unchanged.

### Fixes

- The ARM (Apple silicon) macOS executable could hang on some inputs, so the Intel executable had to be run through Rosetta (#13). On every ARM build, the intermediate values of Triangle's exact arithmetic are now declared volatile, so the processor cannot fuse its operations; this stopped the hangs reported in #13, at some cost in speed (#25).
- The macOS executable needed Homebrew's gcc libraries to run. They are now linked into it, so it runs without Homebrew (#19, and commits 39b8948 and 18e58fc).

### Build and distribution

- gcc and the Intel compilers are given options that turn off extended precision and fused multiply-add, and Triangle is optimized at level 1 with every compiler, so that the meshes are as consistent as possible across platforms and compilers; a mesh, or the run time, may differ slightly from an executable built before (#23).
- The macOS release is built with gcc 15 from conda-forge instead of gcc 13 from Homebrew (#52). It still links only the system library, and the meshes are unchanged.
- An executable for Intel Macs (mac.zip) is no longer built; the macOS executable (macarm.zip) is for Apple silicon only (#23).
- The macOS executable and library require macOS 15 or later; the 1.6 macarm.zip ran on macOS 14 (#35). The minimum is now set explicitly rather than by the build machine's macOS version, so a newer build machine does not raise it (#48). Building on an older macOS has not been tested, but since Triangle is standard C it should work from source with Meson, which targets the macOS it is built on; use the gcc of the pixi gcc environment (pixi run -e gcc setup and pixi run -e gcc build), not Apple's gcc command, which is clang, so that meson.build sets the same compiler options as the release.
- The 1.6 release files were rebuilt in place on 2026-05-26, so linux.zip, macarm.zip, win64.zip, and triangle_source.zip downloaded from the 1.6 release since then already include #19, #23, and #25; its mac.zip, built on 2026-01-30, includes #19 only. From 1.6.1 on, a change is released under a new version rather than by rebuilding an existing release.
- Each platform's release archive now also holds the library in lib/ (on Windows, the DLL beside the executable, and its import library in lib/) and the headers in include/, with the executable still at the top level (#42).

### Documentation

- Each change to Triangle's source is marked in triangle.c and listed in README.md, LICENSE holds Triangle's full terms of use, and the source archive includes LICENSE and README.md (#36).

