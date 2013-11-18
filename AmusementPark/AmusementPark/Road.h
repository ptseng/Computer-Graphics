//
//  Road.h
//  AmusementPark
//
//  Created by Philip Tseng on 11/15/13.
//  Copyright (c) 2013 AppOven. All rights reserved.
//

#ifndef __AmusementPark__Road__
#define __AmusementPark__Road__

#include <iostream>
#include <Fl/gl.h>

class Road {
private:
    GLubyte display_list;   // The display list that does all the work.
    GLuint  texture_obj;    // The object for the grass texture.
    bool    initialized;    // Whether or not we have been initialised.
    
public:
    // Constructor. Can't do initialization here because we are
    // created before the OpenGL context is set up.
    Road(void) { display_list = 0; initialized = false; };
    
    // Destructor. Frees the display lists and texture object.
    ~Road(void);
    
    // Initializer. Creates the display list.
    bool    Initialize(float angle, float x, float y, float z, float length, float width, char * texture);
    
    // Does the drawing.
    void    Draw(void);
};


#endif /* defined(__AmusementPark__Road__) */
