/* Test the Triangle library API. */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef CPU86
#include <float.h>
#endif
#ifdef LINUX
#include <fpu_control.h>
#endif

#include "triangle_release.h"
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

static double angle_at(const double *a, const double *b, const double *c)
{
  double ux = b[0] - a[0], uy = b[1] - a[1];
  double vx = c[0] - a[0], vy = c[1] - a[1];
  return acos((ux * vx + uy * vy) / (hypot(ux, uy) * hypot(vx, vy))) *
         180.0 / 3.14159265358979323846;
}

static double largest_angle(const double *a, const double *b, const double *c)
{
  return fmax(angle_at(a, b, c), fmax(angle_at(b, c, a), angle_at(c, a, b)));
}

static double mesh_largest_angle(struct triangulateio *mesh)
{
  double largest = 0.0;
  int i;
  for (i = 0; i < mesh->numberoftriangles; i++) {
    int *corner = &mesh->trianglelist[3 * i];
    largest = fmax(largest, largest_angle(&mesh->pointlist[2 * corner[0]],
                                          &mesh->pointlist[2 * corner[1]],
                                          &mesh->pointlist[2 * corner[2]]));
  }
  return largest;
}

/* u switch test: refine triangles with an angle over the limit in data */
static int angle_over_limit(const double *org, const double *dest,
                            const double *apex, double area, void *data)
{
  (void) area;
  return largest_angle(org, dest, apex) > *(double *) data;
}

static void free_output(struct triangulateio *mesh)
{
  triangle_free(mesh->pointlist);
  triangle_free(mesh->pointattributelist);
  triangle_free(mesh->pointmarkerlist);
  triangle_free(mesh->triangleattributelist);
  triangle_free(mesh->neighborlist);
  triangle_free(mesh->trianglelist);
  triangle_free(mesh->segmentlist);
  triangle_free(mesh->segmentmarkerlist);
  triangle_free(mesh->edgelist);
  triangle_free(mesh->edgemarkerlist);
  triangle_free(mesh->normlist);
}

int main(void)
{
  double square[8] = {0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0};
  int edges[8] = {0, 1, 1, 2, 2, 3, 3, 0};
  struct triangulateio in, mesh, refined, bad, out, direct, voronoi;
  double polygon[16] = {0.0, 0.0, 10.0, 0.0, 10.0, 3.0, 7.0, 3.0,
                        6.0, 8.0, 2.0, 9.0, 1.0, 4.0, 0.0, 2.0};
  int polygon_edges[16] = {0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 0};
  double angle_limit = 100.0;
  int plain_count;
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
#ifdef CPU86
  {
    unsigned int before = _control87(0, 0);
    status = triangulate_status("pzQa0.05", &in, &mesh, NULL);
    check((_control87(0, 0) & _MCW_PC) == (before & _MCW_PC),
          "floating-point precision is restored");
  }
#elif defined(LINUX)
  {
    fpu_control_t before, after;
    _FPU_GETCW(before);
    status = triangulate_status("pzQa0.05", &in, &mesh, NULL);
    _FPU_GETCW(after);
    check(after == before, "floating-point control word is restored");
  }
#else
  status = triangulate_status("pzQa0.05", &in, &mesh, NULL);
#endif
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
  memset(&out, 0, sizeof(out));
  status = triangulate_status("zQ", &bad, &out, NULL);
  check(status != 0, "a second error returns an error status");

  /* the library still works after an error */
  free_output(&refined);
  memset(&refined, 0, sizeof(refined));
  status = triangulate_status("pzQa0.05", &in, &refined, NULL);
  check(status == 0, "triangulate again after an error");
  check(refined.numberoftriangles == mesh.numberoftriangles,
        "same mesh after an error");

  /* triangulate() gives the same mesh */
  memset(&direct, 0, sizeof(direct));
  triangulate("pzQa0.05", &in, &direct, NULL);
  check(direct.numberoftriangles == mesh.numberoftriangles,
        "triangulate() gives the same mesh");

  /* the Voronoi diagram has a vertex for each triangle */
  memset(&out, 0, sizeof(out));
  memset(&voronoi, 0, sizeof(voronoi));
  status = triangulate_status("pzQeva0.05", &in, &out, &voronoi);
  check(status == 0, "write the Voronoi diagram");
  check(voronoi.numberofpoints == out.numberoftriangles,
        "one Voronoi vertex for each triangle");
  check(out.numberofedges > 0 && voronoi.numberofedges == out.numberofedges,
        "one Voronoi edge for each triangle edge");
  free_output(&out);
  free_output(&voronoi);

  free_output(&mesh);
  free_output(&refined);
  free_output(&direct);

  /* the u switch refines with the caller's test, and does nothing without */
  memset(&in, 0, sizeof(in));
  in.numberofpoints = 8;
  in.pointlist = polygon;
  in.numberofsegments = 8;
  in.segmentlist = polygon_edges;
  memset(&out, 0, sizeof(out));
  status = triangulate_status("pzQq30a0.5", &in, &out, NULL);
  check(status == 0 && mesh_largest_angle(&out) > angle_limit,
        "the polygon has an angle over the limit");
  plain_count = out.numberoftriangles;
  free_output(&out);
  memset(&out, 0, sizeof(out));
  status = triangulate_status("pzQq30a0.5u", &in, &out, NULL);
  check(status == 0 && out.numberoftriangles == plain_count,
        "the u switch without a test changes nothing");
  free_output(&out);
  triangle_set_unsuitable(angle_over_limit, &angle_limit);
  memset(&out, 0, sizeof(out));
  status = triangulate_status("pzQq30a0.5u", &in, &out, NULL);
  triangle_set_unsuitable(NULL, NULL);
  check(status == 0 && mesh_largest_angle(&out) <= angle_limit,
        "refining by largest angle meets the limit");
  check(fabs(mesh_area(&out) - 58.0) < 1.0e-9,
        "refining by largest angle keeps the area");
  free_output(&out);

  /* version and copyright */
  check(strcmp(triangle_version(), "1.6") == 0, "Triangle version is 1.6");
  check(strcmp(triangle_release_version(), TRIANGLE_RELEASE_VERSION) == 0,
        "release version is the version in meson.build");
  check(strstr(triangle_copyright(), "Jonathan Richard Shewchuk") != NULL,
        "copyright names the author");

  if (failures > 0) {
    printf("%d check(s) failed\n", failures);
    return 1;
  }
  printf("all checks passed\n");
  return 0;
}
