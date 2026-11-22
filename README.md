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

## Modifications

`src/triangle.c` has been modified from the netlib distribution as listed below. Each change is marked `MODFLOW-ORG modification` in the file. `triangle.h` and `README` are unmodified. Triangle and these modifications remain under the copyright of the original author; see [LICENSE](LICENSE).

- **64-bit Windows (#2):** the pointer casts use `unsigned_long`, defined as `unsigned long long` on Windows, where `unsigned long` is 32 bits and cannot hold a pointer, and as `unsigned long` elsewhere.
- **ARM (#25):** `INEXACT` is defined as `volatile` on ARM so that the exact-arithmetic predicates are not compiled into fused multiply-add instructions.
- **Library errors (#37):** in the library build (`TRILIBRARY`), `triexit()` calls a handler before `exit()`, so that `triangulate_status()` can return the error to the caller instead of ending the program.

## Library

Triangle is also built as a library, `libtriangle`, shared or static according to Meson's `default_library` option. `src/triangle_status.c` and `src/triangle_status.h` are not part of the Triangle distribution; they add:

- `triangulate_status()`, which calls `triangulate()` and returns 0, or 1 on an error, instead of ending the program, and restores the caller's floating-point control word, which Triangle changes on x86
- `triangle_free()`, which frees an array Triangle allocated in an output structure; use it rather than `free()`, which on Windows may belong to a different C runtime. Free every array pointer in the output structures, including `pointattributelist`, `triangleattributelist`, and `neighborlist`, which Triangle allocates even when they are empty, except `holelist` and `regionlist`, which point to the input's arrays
- `triangle_set_unsuitable()`, which sets the test the `u` switch uses to decide which triangles to refine, such as triangles with an angle over a limit. The library is built with `EXTERNAL_TEST`, so without a test the `u` switch adds no refinement of its own (it still turns on quality refinement, as `q` does). The test receives each corner as a pointer to x, y, and any attributes, valid only during the call, and the area; it must not call Triangle or let a C++ exception escape. It stays set, also after an error, until it is set again or cleared with NULL. Triangle refines until the test accepts every triangle and has no limit of its own, so a test that can keep rejecting new triangles must stop itself, for example after a number of rejections; a 90 degree angle limit, for one, does not finish
- `triangle_version()`, Triangle's version (1.6); `triangle_release_version()`, the release of this repository; and `triangle_copyright()`, Triangle's copyright notice and terms, which a program that contains Triangle must make available to its users

Include `triangle_status.h`, not `triangle.h`, which has no include guard. Triangle prints the reason for an error on standard output. After an error, the arrays in the output structures are not valid and must not be freed, `in` is unchanged, and memory Triangle allocated during the call is not freed, so a long-running program should not retry bad input indefinitely. Triangle keeps global state, so calls must not run concurrently. `triangulate()` is also exported, but it ends the program on an error and does not restore the control word. `test/test_library.c` shows the calls. Another Meson project can use the library as a subproject through the `triangle` dependency; as a subproject, nothing is installed and the executable and tests are not built. The shared library exports only the API; a static library exposes all of Triangle's global names, including `triunsuitable`, so a program that links it statically must not define the same names.
