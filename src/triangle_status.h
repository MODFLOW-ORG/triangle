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

/* Call triangulate(); return 0, or 1 on an error. */
int triangulate_status(char *triswitches, struct triangulateio *in,
                       struct triangulateio *out,
                       struct triangulateio *vorout);

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
