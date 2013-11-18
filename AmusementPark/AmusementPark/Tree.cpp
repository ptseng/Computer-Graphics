//
//  Tree.cpp
//  AmusementPark
//
//  Created by Philip Tseng on 11/13/13.
//  Copyright (c) 2013 AppOven. All rights reserved.
//

#include <iostream>
#include "Tree.h"
#include "libtarga.h"
#include <stdio.h>
#include <stdlib.h>
#include <Fl/gl.h>
#include <OpenGL/glu.h>
#include <math.h>

// Destructor
Tree::~Tree(void)
{
    if ( initialized )
    {
        glDeleteLists(display_list, 1);
        glDeleteTextures(1, &texture_obj);
        gluDeleteQuadric(tree);
    }
}

bool Tree::Initialize(double x, double y, double z, char * texture)
{
    double scale = 1.0f;
    
    double height = 10.0f * scale;
    double stalkheightratio = 0.15f;
    double stalkheight = height * stalkheightratio;
    
    double radius = 2.0f * scale;
    double stalkradiusratio = 0.30f;
    double stalkradius = radius * stalkradiusratio;
    
    double sliceheight = 1.0f;
    double totalsliceheight = height*(1-stalkheightratio);
    int slices = totalsliceheight/sliceheight;
    
    //Texture
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
    

    // End Texture
    
    // Now do the geometry. Create the display list.
    display_list = glGenLists(1);
    glNewList(display_list, GL_COMPILE);
    
	// Use white, because the texture supplies the color.
	glColor3f(89/255.0f, 33/255.0f, 25/255.0f);
    
	// The surface normal is up for the ground.
	glNormal3f(0.0, 0.0, 1.0);
    
	// Turn on texturing and bind the grass texture.

    
    
    // TREE //
    
    tree = gluNewQuadric();
    gluQuadricNormals(tree, GLU_SMOOTH);
    gluQuadricTexture(tree, GL_TRUE);
    
    // Draw at this location
    glTranslatef(x, y, z);
    
    // Stalk
    gluCylinder(tree, stalkradius, stalkradius, stalkheight, 100, 100);  // RadiusBottom, RadiusTop, Height, slice, stack
    
    // Top Disk of Stalk (optional)
    glTranslatef(0,0,stalkheight);
    gluDisk(tree, 0, stalkradius, 100, 1);  // Inner, Outer, Slices, Loops
    
    // Change to Green
    glColor3f(29/255.0f, 74/255.0f, 9/255.0f);
    
    // Bottom Facing Base Disk
    glRotated(180,1,0,0);
    gluDisk(tree, 0, radius, 100, 100);
    
    // Top Facing Base Disk
    glRotated(180,1,0,0);
    gluDisk(tree, 0, radius, 100, 100);
    
    // Slices
    for (int i = 0; i < slices*5; i++)
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texture_obj);
        gluCylinder(tree, radius-(1.0f*i/slices), 0, sliceheight, 100, 100);
        glTranslatef(0, 0, sliceheight/5);
    }
    
	// Turn texturing off again, because we don't want everything else to
	// be textured.
	glDisable(GL_TEXTURE_2D);
    glEndList();
    
    // We only do all this stuff once, when the GL context is first set up.
    initialized = true;
    
    return true;
}


// Draw just calls the display list we set up earlier.
void
Tree::Draw(void)
{
    glPushMatrix();
    glCallList(display_list);
    glPopMatrix();
}
