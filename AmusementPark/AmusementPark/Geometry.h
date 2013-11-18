//
//  Geometry.h
//  AmusementPark
//
//  Created by Philip Tseng on 11/17/13.
//  Copyright (c) 2013 AppOven. All rights reserved.
//

#ifndef AmusementPark_Geometry_h
#define AmusementPark_Geometry_h

struct Vertex {
    float   	    x[3];
};

struct Edge {
    unsigned int    vs;
    unsigned int    ve;
    unsigned int    v_new;
    unsigned int    s_child;
    unsigned int    e_child;
};

typedef struct _Triangle {
    bool forward[3];
    int edges[3];
    int unused[3];
} Triangle, * TrianglePtr;


#endif
