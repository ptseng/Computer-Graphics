//
//  Sphere.h
//  AmusementPark
//
//  Created by Philip Tseng on 11/16/13.
//  Copyright (c) 2013 AppOven. All rights reserved.
//

#ifndef __AmusementPark__Sphere__
#define __AmusementPark__Sphere__

#include <iostream>
#include <Fl/gl.h>

class Sphere {
    
private:
    GLubyte display_list;   // The display list that does all the work.
    GLuint texture_obj;    // The object for the grass texture.
    bool initialized;    // Whether or not we have been initialised.
    GLUquadricObj * qobj_ptr;
    
public:
    Sphere() { display_list = 0; initialized = false; };
    ~Sphere();
    
    bool Initialize(int scale, float x, float y, float z, float r, float g, float b, char * texture);
    void Draw();
};

#endif /* defined(__AmusementPark__Sphere__) */
