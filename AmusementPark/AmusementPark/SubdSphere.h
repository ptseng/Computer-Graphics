/*
 * Sphere.h: Code for sphere generation
 *
 * (c) 2001 Stephen Chenney, University of Wisconsin at Madison
 */

#include <math.h>
#include "Geometry.h"

class SubdSphere {
  private:
    int    num_vertices;
    int    num_edges;
    int    num_faces;
    Vertex  	    *vertices;
    Edge    	    *edges;
    Triangle	    *faces;

  public:
    SubdSphere();
    ~SubdSphere();

    void    Subdivide(int);
    void    Render(bool smooth);
};

