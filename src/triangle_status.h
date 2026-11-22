/* Calling Triangle as a library with errors returned to the caller.         */
/* Not part of the Triangle distribution; see README.md.                     */

#ifndef TRIANGLE_STATUS_H
#define TRIANGLE_STATUS_H

#ifndef REAL
#define REAL double
#endif
#ifndef VOID
#define VOID int
#endif
#ifndef ANSI_DECLARATORS
#define ANSI_DECLARATORS
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include "triangle.h"

/* Call triangulate(); return 0 on success, or the nonzero status Triangle   */
/*   would have exited with.  After an error, the output structures are not  */
/*   valid and memory Triangle allocated during the call is not freed.       */
/*   Triangle uses global state, so calls must not run concurrently.         */
int triangulate_status(char *triswitches, struct triangulateio *in,
                       struct triangulateio *out,
                       struct triangulateio *vorout);

/* Triangle's version, "1.6".                                                */
const char *triangle_version(void);

/* The MODFLOW-ORG/triangle release this library was built from.            */
const char *triangle_release_version(void);

/* Triangle's copyright notice and terms of use, which a program that        */
/*   contains Triangle must make available to its users.                     */
const char *triangle_copyright(void);

#ifdef __cplusplus
}
#endif

#endif /* TRIANGLE_STATUS_H */
