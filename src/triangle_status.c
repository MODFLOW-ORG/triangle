/* Triangle library API that returns errors; not part of Triangle. */

#include <setjmp.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#ifdef CPU86
#include <float.h>
#endif
#ifdef LINUX
#include <fpu_control.h>
#endif

#include "triangle_release.h"
#include "triangle_status.h"

extern void (*triexithandler)(int);
void triexit(int status);

static triangle_unsuitable_test unsuitabletest = (triangle_unsuitable_test) 0;
static void *unsuitabledata = (void *) 0;

static jmp_buf triexitjump;
static int triexitstatus;

/* the end of the messages of the last call, with room for the null */
#define MESSAGES_SIZE 4096
static char messages[MESSAGES_SIZE];
static size_t messageslength = 0;
static int printmessages = 1;

/* Print a message, if printing is on, and keep it. */
int triangle_printf(const char *format, ...)
{
  char text[MESSAGES_SIZE];
  size_t length;
  size_t drop;
  char *cut;
  va_list args;
  int count;

  va_start(args, format);
  count = vsnprintf(text, sizeof(text), format, args);
  va_end(args);
  if (count < 0) {
    return count;
  }
  if (printmessages) {
    fputs(text, stdout);
  }
  length = strlen(text);
  /* drop whole lines from the start to make room */
  if (messageslength + length > MESSAGES_SIZE - 1) {
    drop = messageslength + length - (MESSAGES_SIZE - 1);
    cut = memchr(messages + drop - 1, '\n', messageslength - drop + 1);
    drop = cut == NULL ? messageslength : (size_t) (cut - messages) + 1;
    memmove(messages, messages + drop, messageslength - drop);
    messageslength -= drop;
  }
  memcpy(messages + messageslength, text, length);
  messageslength += length;
  messages[messageslength] = '\0';
  return count;
}

/* Return from triexit() to triangulate_status() instead of exiting. */
static void triexitreturn(int status)
{
  /* 2 is the u switch's test stopping; anything else is an error */
  triexitstatus = status == 2 ? 2 : 1;
  longjmp(triexitjump, 1);
}

int triangulate_status(const char *triswitches, struct triangulateio *in,
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
  messageslength = 0;
  messages[0] = '\0';
  if (setjmp(triexitjump) == 0) {
    triexithandler = triexitreturn;
    /* Triangle reads the switches without changing them */
    triangulate((char *) triswitches, in, out, vorout);
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

const char *triangle_messages(void)
{
  return messages;
}

void triangle_set_print(int print)
{
  printmessages = print;
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

void triangle_set_unsuitable(triangle_unsuitable_test test, void *data)
{
  unsuitabletest = test;
  unsuitabledata = data;
}

int triunsuitable(double *triorg, double *tridest, double *triapex,
                  double area);

/* Triangle calls this for the u switch, since the library is built with
   EXTERNAL_TEST; with no test set, it refines no triangle. */
int triunsuitable(double *triorg, double *tridest, double *triapex,
                  double area)
{
  int result;

  if (unsuitabletest == (triangle_unsuitable_test) 0) {
    return 0;
  }
  result = unsuitabletest(triorg, tridest, triapex, area, unsuitabledata);
  if (result < 0) {
    triexit(2);
  }
  return result > 0;
}
