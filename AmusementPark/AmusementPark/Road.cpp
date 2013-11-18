//
//  Road.cpp
//  AmusementPark
//
//  Created by Philip Tseng on 11/15/13.
//  Copyright (c) 2013 AppOven. All rights reserved.
//

#include "Road.h"
#include <iostream>
#include "libtarga.h"
#include <stdio.h>
#include <Fl/gl.h>
#include <OpenGL/glu.h>

// Destructor
Road::~Road(void)
{
    if ( initialized )
    {
        glDeleteLists(display_list, 1);
        glDeleteTextures(1, &texture_obj);
    }
}

// Initializer. Returns false if something went wrong, like not being able to
// load the texture.
bool Road::Initialize(float angle, float x, float y, float z, float length, float width, char * texture)
{
    
    ubyte   *image_data;
    int	    image_height, image_width;
    
    // Load the image for the texture. The texture file has to be in
    // a place where it will be found.
    if ( ! ( image_data = (ubyte*)tga_load(texture, &image_width,
                                           &image_height, TGA_TRUECOLOR_24) ) )
    {
        fprintf(stderr, "Ground::Initialize: Couldn't load road.tga\n");
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
	// Use white, because the texture supplies the color.
	glColor3f(1.0, 1.0, 1.0);
    
	// The surface normal is up for the ground.
	glNormal3f(0.0, 0.0, 1.0);
    
    glTranslatef(x, y, z);
    glRotated(angle, 0, 0, 1);
    
	// Turn on texturing and bind the grass texture.
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture_obj);
    
    glDisable(GL_CULL_FACE);
    
    float height = 0.1f;
    //float length = 100;
    //float width = 8;
    
    glBegin(GL_TRIANGLE_STRIP);
    
    glVertex3f(length/2, width/2, 0);
    glVertex3f(length/2, -width/2, 0);
    glVertex3f(length/2, width/2, height);
    glVertex3f(length/2, -width/2, height);
    
    glVertex3f(-length/2, width/2, height);
    glVertex3f(-length/2, -width/2, height);
    glVertex3f(-length/2, width/2, 0);
    glVertex3f(-length/2, -width/2, 0);
    
    glVertex3f(length/2, -width/2, 0);
    glVertex3f(length/2, width/2, 0);
    glVertex3f(-length/2, width/2, 0);
    
    glEnd();
    
    glBegin(GL_TRIANGLE_STRIP);
    
    glVertex3f(length/2, width/2, height);
    glVertex3f(length/2, width/2, 0);
    glVertex3f(-length/2, width/2, height);
    glVertex3f(-length/2, width/2, 0);
    
    glEnd();
    
    glBegin(GL_TRIANGLE_STRIP);
    
    glVertex3f(length/2, -width/2, height);
    glVertex3f(length/2, -width/2, 0);
    glVertex3f(-length/2, -width/2, height);
    glVertex3f(-length/2, -width/2, 0);
    
    glEnd();
    
    glTranslatef(-length/2, -width/2, 0.1f);
    
    glBegin(GL_QUADS);
    
    glTexCoord2f(0, 0);
    glVertex3f(0, 0, height);
    
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(0, width, height);
    
    glTexCoord2f(16.0f, 1.0f);
    glVertex3f(length, width, height);
    
    glTexCoord2f(16.0f, 0.0f);
    glVertex3f(length, 0, height);
    
    glEnd();
    
    glEnable(GL_CULL_FACE);
    
	// Turn texturing off again, because we don't want everything else to
	// be textured.
	glDisable(GL_TEXTURE_2D);
    glEndList();
    
    // We only do all this stuff once, when the GL context is first set up.
    initialized = true;
    
    return true;
}


// Draw just calls the display list we set up earlier.
void Road::Draw(void)
{
    glPushMatrix();
    glCallList(display_list);
    glPopMatrix();
}