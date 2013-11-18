//
//  Tree.h
//  AmusementPark
//
//  Created by Philip Tseng on 11/13/13.
//  Copyright (c) 2013 AppOven. All rights reserved.
//

#ifndef AmusementPark_Tree_h
#define AmusementPark_Tree_h

#include <Fl/gl.h>
#include <OpenGL/glu.h>

class Tree {
private:
    GLubyte display_list;   // The display list that does all the work.
    GLuint  texture_obj;    // The object for the tree texture.
    bool    initialized;    // Whether or not we have been initialised.
    
    GLUquadricObj * tree;
    
public:
    // Constructor. Can't do initialization here because we are
    // created before the OpenGL context is set up.
    Tree(void) { display_list = 0; initialized = false; };
    
    // Destructor. Frees the display lists and texture object.
    ~Tree(void);
    
    // Initializer. Creates the display list.
    bool Initialize(double x, double y, double z, char * texture);
    
    // Does the drawing.
    void    Draw(void);
    
    
};

#endif
