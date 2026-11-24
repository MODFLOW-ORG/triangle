# triangle

[![triangle checks](https://github.com/MODFLOW-ORG/triangle/actions/workflows/ci.yml/badge.svg)](https://github.com/MODFLOW-ORG/triangle/actions/workflows/ci.yml)

A Two-Dimensional Quality Mesh Generator and Delaunay Triangulator.
Version 1.6

Copyright 1993, 1995, 1997, 1998, 2002, 2005  
Jonathan Richard Shewchuk  
2360 Woolsey #H  
Berkeley, California  94705-1927

Additional information on triangle can be found at [https://www.cs.cmu.edu/~quake/triangle.html](https://www.cs.cmu.edu/~quake/triangle.html).

-------------

Downloaded from https://www.netlib.org/voronoi/triangle.zip on July 5, 2023.

This repository was created to facilitate automatic builds of triangle used in GitHub Actions CI workflows. Only the `triangle.c`, `triangle.h`, and `README` files, which are available in [https://www.netlib.org/voronoi/triangle.zip](https://www.netlib.org/voronoi/triangle.zip), have been included in this repository, in `src/`.

## Older macOS

The macOS release requires macOS 15 or later. Building on an older macOS has not been tested, but since Triangle is standard C it should work from source with Meson (`pixi run setup` and `pixi run build`), which targets the macOS it is built on. Use the gcc of the pixi `gcc` environment (`pixi run -e gcc setup` and `pixi run -e gcc build`), not Apple's `gcc` command, which is clang; `meson.build` sets the same compiler options for gcc as the release build, so that the meshes match it. Setting `MACOSX_DEPLOYMENT_TARGET` (for example to `14.0`) when building on a newer Mac targets an older version.

## Modifications

`src/triangle.c` has been modified from the netlib distribution as listed below. Each change is marked `MODFLOW-ORG modification` in the file. `triangle.h` and `README` are unmodified. Triangle and these modifications remain under the copyright of the original author; see [LICENSE](LICENSE).

- **64-bit Windows (#2):** the pointer casts use `unsigned_long`, defined as `unsigned long long` on Windows, where `unsigned long` is 32 bits and cannot hold a pointer, and as `unsigned long` elsewhere.
- **ARM (#25):** `INEXACT` is defined as `volatile` on ARM so that the exact-arithmetic predicates are not compiled into fused multiply-add instructions.
- **Library errors (#37):** in the library build (`TRILIBRARY`), `triexit()` calls a handler before `exit()`, so that `triangulate_status()` can return the error to the caller instead of ending the program.
- **Library messages (#61):** in the library build, `printf` is defined as `triangle_printf()`, which prints Triangle's messages and keeps them for `triangle_messages()`.

## Library

Triangle is also built as a library, `libtriangle`, shared or static according to Meson's `default_library` option. `src/triangle_status.c` and `src/triangle_status.h` are not part of the Triangle distribution; they add:

- `triangulate_status()`, which calls `triangulate()` and returns 0, 1 on an error instead of ending the program, or 2 if the `u` switch's test stopped it, and restores the caller's floating-point control word, which Triangle changes on x86
- `triangle_free()`, which frees an array Triangle allocated in an output structure; use it rather than `free()`, which on Windows may belong to a different C runtime. Free every array pointer in the output structures, including `pointattributelist`, `triangleattributelist`, and `neighborlist`, which Triangle allocates even when they are empty, except `holelist` and `regionlist`, which point to the input's arrays
- `triangle_set_unsuitable()`, which sets the test the `u` switch uses to decide which triangles to refine, such as triangles with an angle over a limit. The library is built with `EXTERNAL_TEST`, so without a test the `u` switch adds no refinement of its own (it still turns on quality refinement, as `q` does). The test receives each corner as a pointer to x, y, and any attributes, valid only during the call, and the area, and returns a positive value to refine the triangle, 0 to accept it, or a negative value to stop: `triangulate_status()` then returns 2, with no valid output, as after an error. It must not call Triangle or let a C++ exception escape. It stays set, also after an error, until it is set again or cleared with NULL. Refinement driven by a test may never finish (a 90 degree angle limit does not), and once it has gone far it may not finish even after the test accepts every triangle, so a test that could keep rejecting should count its calls and stop the run with a negative value
- `triangle_messages()`, the text Triangle printed during the last call to `triangulate_status()`, such as the reason for an error, cleared at the start of each call; when there is more than its 4095 characters, it keeps the last whole lines that fit. The text is valid until the next call. `triangle_set_print()` turns printing to standard output off (0) or on (nonzero, the default); the messages are kept either way
- `triangle_version()`, Triangle's version (1.6); `triangle_release_version()`, the release of this repository; and `triangle_copyright()`, Triangle's copyright notice and terms, which a program that contains Triangle must make available to its users

Include `triangle_status.h`, not `triangle.h`, which has no include guard. Triangle prints the reason for an error on standard output, unless printing is off, and `triangle_messages()` returns it. After an error, the arrays in the output structures are not valid and must not be freed, `in` is unchanged, and memory Triangle allocated during the call is not freed, so a long-running program should not retry bad input indefinitely. Triangle keeps global state, so calls must not run concurrently. `triangulate()` is also exported, but it ends the program on an error, or when the `u` switch's test stops it, does not restore the control word, and does not clear the messages. `test/test_library.c` shows the calls. Another Meson project can use the library as a subproject through the `triangle` dependency; as a subproject, nothing is installed and the executable and tests are not built. The shared library exports only the API; a static library exposes all of Triangle's global names, including `triunsuitable` and `triangle_printf`, so a program that links it statically must not define the same names.
