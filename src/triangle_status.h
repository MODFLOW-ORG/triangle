/* Triangle library API that returns errors; not part of Triangle. */
/* Include this header instead of triangle.h, which has no include guard. */

#ifndef TRIANGLE_STATUS_H
#define TRIANGLE_STATUS_H

/* triangle.h needs these; undefined after it, since VOID conflicts with
   windows.h */
#ifndef REAL
#define REAL double
#define TRIANGLE_STATUS_UNDEF_REAL
#endif
#ifndef VOID
#define VOID int
#define TRIANGLE_STATUS_UNDEF_VOID
#endif
#ifndef ANSI_DECLARATORS
#define ANSI_DECLARATORS
#define TRIANGLE_STATUS_UNDEF_ANSI_DECLARATORS
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include "triangle.h"

/* the library is built with REAL as double */
typedef char triangle_status_real_is_double[sizeof(REAL) == sizeof(double)
                                            ? 1 : -1];

/* Call triangulate(); return 0, 1 on an error, or 2 if the u switch's test
   stopped it. */
int triangulate_status(const char *triswitches, struct triangulateio *in,
                       struct triangulateio *out,
                       struct triangulateio *vorout);

/* Test for the u switch: return positive to refine the triangle with these
   corners (x, y, then attributes; valid only during the call) and area, 0 to
   accept it, or negative to stop triangulating. It must not call Triangle or
   throw. */
typedef int (*triangle_unsuitable_test)(const double *org, const double *dest,
                                        const double *apex, double area,
                                        void *data);

/* Set the u switch's test and the data passed to it; NULL clears it. */
void triangle_set_unsuitable(triangle_unsuitable_test test, void *data);

/* Free an output array Triangle allocated. */
void triangle_free(void *memptr);

/* Triangle's version, "1.6". */
const char *triangle_version(void);

/* MODFLOW-ORG/triangle release. */
const char *triangle_release_version(void);

/* Triangle's copyright notice and terms of use. */
const char *triangle_copyright(void);

#ifdef __cplusplus
}
#endif

#ifdef TRIANGLE_STATUS_UNDEF_REAL
#undef REAL
#undef TRIANGLE_STATUS_UNDEF_REAL
#endif
#ifdef TRIANGLE_STATUS_UNDEF_VOID
#undef VOID
#undef TRIANGLE_STATUS_UNDEF_VOID
#endif
#ifdef TRIANGLE_STATUS_UNDEF_ANSI_DECLARATORS
#undef ANSI_DECLARATORS
#undef TRIANGLE_STATUS_UNDEF_ANSI_DECLARATORS
#endif

#endif /* TRIANGLE_STATUS_H */
