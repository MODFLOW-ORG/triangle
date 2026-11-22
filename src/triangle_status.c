/* Triangle library API that returns errors; not part of Triangle. */

#include <setjmp.h>
#ifdef CPU86
#include <float.h>
#endif
#ifdef LINUX
#include <fpu_control.h>
#endif

#include "triangle_release.h"
#include "triangle_status.h"

extern void (*triexithandler)(int);

static jmp_buf triexitjump;
static int triexitstatus;

/* Return from triexit() to triangulate_status() instead of exiting. */
static void triexitreturn(int status)
{
  triexitstatus = status != 0 ? status : 1;
  longjmp(triexitjump, 1);
}

int triangulate_status(char *triswitches, struct triangulateio *in,
                       struct triangulateio *out,
                       struct triangulateio *vorout)
{
  /* exactinit() changes the control word; restore the caller's */
#ifdef CPU86
  unsigned int control = _control87(0, 0);
#endif
#ifdef LINUX
  fpu_control_t control;
  _FPU_GETCW(control);
#endif

  triexitstatus = 0;
  if (setjmp(triexitjump) == 0) {
    triexithandler = triexitreturn;
    triangulate(triswitches, in, out, vorout);
  }
  triexithandler = (void (*)(int)) 0;

#ifdef CPU86
  _control87(control, _MCW_PC);
#endif
#ifdef LINUX
  _FPU_SETCW(control);
#endif
  return triexitstatus;
}

void triangle_free(void *memptr)
{
  trifree(memptr);
}

const char *triangle_version(void)
{
  return "1.6";
}

const char *triangle_release_version(void)
{
  return TRIANGLE_RELEASE_VERSION;
}

const char *triangle_copyright(void)
{
  return
    "Triangle\n"
    "A Two-Dimensional Quality Mesh Generator and Delaunay Triangulator.\n"
    "Version 1.6\n"
    "\n"
    "Copyright 1993, 1995, 1997, 1998, 2002, 2005 Jonathan Richard Shewchuk\n"
    "2360 Woolsey #H / Berkeley, California 94705-1927\n"
    "Bugs/comments to jrs@cs.berkeley.edu\n"
    "Created as part of the Quake project (tools for earthquake simulation).\n"
    "Supported in part by NSF Grant CMS-9318163 and an NSERC 1967 Scholarship.\n"
    "There is no warranty whatsoever.  Use at your own risk.\n"
    "\n"
    "These programs may be freely redistributed under the condition that the\n"
    "copyright notices (including the copy of this notice in the code comments\n"
    "and the copyright notice printed when the `-h' switch is selected) are\n"
    "not removed, and no compensation is received.  Private, research, and\n"
    "institutional use is free.  You may distribute modified versions of this\n"
    "code UNDER THE CONDITION THAT THIS CODE AND ANY MODIFICATIONS MADE TO IT\n"
    "IN THE SAME FILE REMAIN UNDER COPYRIGHT OF THE ORIGINAL AUTHOR, BOTH\n"
    "SOURCE AND OBJECT CODE ARE MADE FREELY AVAILABLE WITHOUT CHARGE, AND\n"
    "CLEAR NOTICE IS GIVEN OF THE MODIFICATIONS.  Distribution of this code as\n"
    "part of a commercial system is permissible ONLY BY DIRECT ARRANGEMENT\n"
    "WITH THE AUTHOR.  (If you are not directly supplying this code to a\n"
    "customer, and you are instead telling them how they can obtain it for\n"
    "free, then you are not required to make any arrangement with me.)\n"
    "\n"
    "Modified by MODFLOW-ORG; see https://github.com/MODFLOW-ORG/triangle.\n";
}
