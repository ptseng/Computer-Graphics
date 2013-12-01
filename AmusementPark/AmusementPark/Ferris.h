//
//  Ferris.h
//  AmusementPark
//
//  Created by Philip Tseng on 12/1/13.
//  Copyright (c) 2013 AppOven. All rights reserved.
//

#ifndef __AmusementPark__Ferris__
#define __AmusementPark__Ferris__

#include <iostream>
#include <Fl/gl.h>

class Ferris
{
private:
    GLubyte display_list;   // The display list that does all the work.
    GLuint  texture_obj;    // The object for the tree texture.
    bool    initialized;    // Whether or not we have been initialised.
    
    GLfloat gDegreesRotated = 359.0f;
    
    
public:
    // Constructor. Can't do initialization here because we are
    // created before the OpenGL context is set up.
    Ferris() { display_list = 0; initialized = false; };
    ~Ferris();
    
    // Initializer. Creates the display list.
    bool Initialize(double x, double y, double z);
    
    // Does the drawing.
    void Draw(void);
    
    void Update(float dt);
};

#endif /* defined(__AmusementPark__Ferris__) */
