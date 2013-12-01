//
//  TeaPot.cpp
//  AmusementPark
//
//  Created by Philip Tseng on 11/15/13.
//  Copyright (c) 2013 AppOven. All rights reserved.
//

#include "TeaPot.h"
#include <iostream>
#include "libtarga.h"
#include <stdio.h>
#include <stdlib.h>
#include <Fl/gl.h>
#include <OpenGL/glu.h>
#include <FL/glut.H>
#include <math.h>

// Destructor
TeaPot::~TeaPot()
{
    if ( initialized )
    {
        glDeleteLists(display_list, 1);
        glDeleteTextures(1, &texture_obj);
    }
}

bool TeaPot::Initialize(double x, double y, double z)
{
    ubyte   *image_data;
    int	    image_height, image_width;
    
    // Load the image for the texture. The texture file has to be in
    // a place where it will be found.
    if ( ! ( image_data = (ubyte*)tga_load("Teapot.tga", &image_width,
                                           &image_height, TGA_TRUECOLOR_24) ) )
    {
        fprintf(stderr, "Ground::Initialize: Couldn't load grass.tga\n");
        return false;
    }
    
    // This creates a texture object and binds it, so the next few operations
    // apply to this texture.
    glGenTextures(1, &texture_obj);
    glBindTexture(GL_TEXTURE_2D, texture_obj);
    
    // This sets a parameter for how the texture is loaded and interpreted.
    // basically, it says that the data is packed tightly in the image array.
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    
    // This sets up the texture with high quality filtering. First it builds
    // mipmaps from the image data, then it sets the filtering parameters
    // and the wrapping parameters. We want the grass to be repeated over the
    // ground.
    gluBuild2DMipmaps(GL_TEXTURE_2D,3, image_width, image_height, GL_RGB, GL_UNSIGNED_BYTE, image_data);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    
    float maximumAnistropy;
    //get the value
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maximumAnistropy);
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maximumAnistropy);
    
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    
    // This says what to do with the texture. Modulate will multiply the
    // texture by the underlying color.
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    
    // Now do the geometry. Create the display list.
    display_list = glGenLists(1);
    glNewList(display_list, GL_COMPILE);
    
    //glColor3f(119/255.0f,11/255.0f,0);
    
    glDisable(GL_CULL_FACE);
    
    // The surface normal is up for the ground.
	glNormal3f(0.0, 0.0, 1.0);
    
    glRotated(90, 1, 0, 0);
    
    glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture_obj);
    glutSolidTeapot(3);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_CULL_FACE);
    
    glEndList();
    
    // We only do all this stuff once, when the GL context is first set up.
    initialized = true;
    
    return true;
}


// Draw just calls the display list we set up earlier.
void TeaPot::Draw(void)
{
    glPushMatrix();
    glTranslatef(-45, 0, 2.25f);
    glRotatef(gDegreesRotated, 0, 0, 1);
    glCallList(display_list);
    glPopMatrix();
}

void TeaPot::Update(float dt)
{
    gDegreesRotated -= 1.0f;
    while(gDegreesRotated > 360.0f)
        gDegreesRotated -= 360.0f;
}
