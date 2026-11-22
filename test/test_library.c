/* Test calling Triangle through the library: triangulate a unit square, */
/* refine the result, and return an error without exiting.               */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "triangle_status.h"

static int failures = 0;

static void check(int condition, const char *message)
{
  if (!condition) {
    printf("FAIL: %s\n", message);
    failures++;
  }
}

static double mesh_area(struct triangulateio *mesh)
{
  double area = 0.0;
  int i;
  for (i = 0; i < mesh->numberoftriangles; i++) {
    int *corner = &mesh->trianglelist[3 * i];
    double *a = &mesh->pointlist[2 * corner[0]];
    double *b = &mesh->pointlist[2 * corner[1]];
    double *c = &mesh->pointlist[2 * corner[2]];
    area += 0.5 * ((b[0] - a[0]) * (c[1] - a[1]) -
                   (c[0] - a[0]) * (b[1] - a[1]));
  }
  return area;
}

static void free_output(struct triangulateio *mesh)
{
  trifree((VOID *) mesh->pointlist);
  trifree((VOID *) mesh->pointmarkerlist);
  trifree((VOID *) mesh->trianglelist);
  trifree((VOID *) mesh->segmentlist);
  trifree((VOID *) mesh->segmentmarkerlist);
}

int main(void)
{
  double square[8] = {0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0};
  int edges[8] = {0, 1, 1, 2, 2, 3, 3, 0};
  struct triangulateio in, mesh, refined, bad, out;
  double *area_limit;
  int status;
  int i;

  /* triangulate the unit square with a maximum triangle area */
  memset(&in, 0, sizeof(in));
  memset(&mesh, 0, sizeof(mesh));
  in.numberofpoints = 4;
  in.pointlist = square;
  in.numberofsegments = 4;
  in.segmentlist = edges;
  status = triangulate_status("pzQa0.05", &in, &mesh, NULL);
  check(status == 0, "triangulate the unit square");
  check(mesh.numberoftriangles >= 20, "area limit produces >= 20 triangles");
  check(fabs(mesh_area(&mesh) - 1.0) < 1.0e-12, "triangles cover the square");

  /* refine the mesh with a smaller area for every triangle */
  area_limit = (double *) malloc(mesh.numberoftriangles * sizeof(double));
  for (i = 0; i < mesh.numberoftriangles; i++) {
    area_limit[i] = 0.005;
  }
  mesh.trianglearealist = area_limit;
  memset(&refined, 0, sizeof(refined));
  status = triangulate_status("rpzQa", &mesh, &refined, NULL);
  check(status == 0, "refine the mesh");
  check(refined.numberoftriangles > 4 * mesh.numberoftriangles,
        "refinement increases the number of triangles");
  check(fabs(mesh_area(&refined) - 1.0) < 1.0e-12,
        "refined triangles cover the square");
  free(area_limit);

  /* an error returns a status instead of ending the program */
  memset(&bad, 0, sizeof(bad));
  memset(&out, 0, sizeof(out));
  bad.numberofpoints = 2;
  bad.pointlist = square;
  status = triangulate_status("zQ", &bad, &out, NULL);
  check(status != 0, "two vertices return an error status");

  /* the library still works after an error */
  free_output(&refined);
  memset(&refined, 0, sizeof(refined));
  status = triangulate_status("pzQa0.05", &in, &refined, NULL);
  check(status == 0, "triangulate again after an error");
  check(refined.numberoftriangles == mesh.numberoftriangles,
        "same mesh after an error");

  free_output(&mesh);
  free_output(&refined);

  /* version and copyright */
  check(strcmp(triangle_version(), "1.6") == 0, "Triangle version is 1.6");
  check(strlen(triangle_release_version()) > 0, "release version is set");
  check(strstr(triangle_copyright(), "Jonathan Richard Shewchuk") != NULL,
        "copyright names the author");

  if (failures > 0) {
    printf("%d check(s) failed\n", failures);
    return 1;
  }
  printf("all checks passed\n");
  return 0;
}
