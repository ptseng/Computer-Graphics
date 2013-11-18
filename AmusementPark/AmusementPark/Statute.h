//
//  Statute.h
//  AmusementPark
//
//  Created by Philip Tseng on 11/17/13.
//  Copyright (c) 2013 AppOven. All rights reserved.
//

#include <math.h>
#include "Geometry.h"

class Statute {
private:
    int    num_vertices;
    int    num_edges;
    int    num_faces;
    Vertex  	    *vertices;
    Edge    	    *edges;
    Triangle	    *faces;
    
public:
    Statute();
    ~Statute();
    
    void    Subdivide(int);
    void    Render(bool smooth);
};