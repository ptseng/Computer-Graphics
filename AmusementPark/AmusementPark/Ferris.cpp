//
//  Ferris.cpp
//  AmusementPark
//
//  Created by Philip Tseng on 12/1/13.
//  Copyright (c) 2013 AppOven. All rights reserved.
//

#include "Ferris.h"
#include <iostream>
#include "libtarga.h"
#include <stdio.h>
#include <stdlib.h>
#include <Fl/gl.h>
#include <OpenGL/glu.h>
#include <FL/glut.H>
#include <math.h>

// Destructor
Ferris::~Ferris()
{
    if ( initialized )
    {
        glDeleteLists(display_list, 1);
        glDeleteTextures(1, &texture_obj);
    }
}



//Start Ferris Wheel

int axes=0;       //  Display axes
int th=0;         //  Azimuth of view angle
int ph=0;         //  Elevation of view angle
int fov=55;       //  Field of view (for perspective)
int num_lights=1;
int spokes = 6;
double asp=1;     //  Aspect ratio
double dim=7.0;   //  Size of world
double rotation = 0;
double vel_division = 900000.0;
int earthquake = 0;
//Lighting
int lamp      =   1;
int one       =   1;  // Unit value
int distance  =   5;  // Light distance
int inc       =  10;  // Ball increment
int smooth    =   1;  // Smooth/Flat shading
int local     =   0;  // Local Viewer Model
int emission  =   0;  // Emission intensity (%)
int ambient   =  30;  // Ambient intensity (%)
int diffuse   = 100;  // Diffuse intensity (%)
int specular  =   0;  // Specular intensity (%)
int shininess =   0;  // Shininess (power of two)
float shinyvec[1];    // Shininess (value)
int zh        =  90;  // Light azimuth
float ylight  =   0;  // Elevation of light

// Textures
unsigned int texture[3];

static void outer_frame(double center_x, double center_y,
                        double center_z, double r,
                        double segments, double step)
{
    int i;
    glBegin(GL_LINE_STRIP);
    glColor3f(1.0, 0.7, 0.5);
    for (i=0;i<segments;i++)
    {
        double x = r * cos(i*step+rotation);
        double y = r * sin(i*step+rotation);
        double x2 = r * cos((i+1)*step+rotation);
        double y2 = r * sin((i+1)*step+rotation);
        glVertex3f(x, y, center_z);
        glVertex3f(x2, y2, center_z); //close the circle
    }
    glEnd();
}

static void beam(double x,double y,double z,
                 double dx,double dy,double dz,
                 double th1, double th2, double ph)
{
    float white[] = {1,1,1,1};
    float Emission[]  = {0.0,0.0,0.01*emission,1.0};
    glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,shinyvec);
    glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
    glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,Emission);
    
    //  Save transformation
    glPushMatrix();
    //  Offset
    glTranslated(x,y,z);
    glRotated(th1,ph,1,th2);
    glScaled(dx,dy,dz);
    
    /*
    glEnable(GL_TEXTURE_2D);
    glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
    glColor3f(1,1,1);
    glBindTexture(GL_TEXTURE_2D,texture[1]);
    */
    
    //  passenger_box
    glBegin(GL_QUADS);
    //  Front
    glNormal3f( 0, 0, 1);
    glTexCoord2f(0,0); glVertex3f(-1,-1, 1);
    glTexCoord2f(1,0); glVertex3f(+1,-1, 1);
    glTexCoord2f(1,1); glVertex3f(+1,+1, 1);
    glTexCoord2f(0,1); glVertex3f(-1,+1, 1);
    //  Back
    glNormal3f( 0, 0,-1);
    glTexCoord2f(0,0); glVertex3f(+1,-1,-1);
    glTexCoord2f(1,0); glVertex3f(-1,-1,-1);
    glTexCoord2f(1,1); glVertex3f(-1,+1,-1);
    glTexCoord2f(0,1); glVertex3f(+1,+1,-1);
    //  Right
    glNormal3f(+1, 0, 0);
    glTexCoord2f(0,0); glVertex3f(+1,-1,+1);
    glTexCoord2f(1,0); glVertex3f(+1,-1,-1);
    glTexCoord2f(1,1); glVertex3f(+1,+1,-1);
    glTexCoord2f(0,1); glVertex3f(+1,+1,+1);
    //  Left
    glNormal3f(-1, 0, 0);
    glTexCoord2f(0,0); glVertex3f(-1,-1,-1);
    glTexCoord2f(1,0); glVertex3f(-1,-1,+1);
    glTexCoord2f(1,1); glVertex3f(-1,+1,+1);
    glTexCoord2f(0,1); glVertex3f(-1,+1,-1);
    //  Top
    glNormal3f( 0,+1, 0);
    glTexCoord2f(0,0); glVertex3f(-1,+1,+1);
    glTexCoord2f(1,0); glVertex3f(+1,+1,+1);
    glTexCoord2f(1,1); glVertex3f(+1,+1,-1);
    glTexCoord2f(0,1); glVertex3f(-1,+1,-1);
    //  Bottom
    glNormal3f( 0,-one, 0);
    glTexCoord2f(0,0); glVertex3f(-1,-1,-1);
    glTexCoord2f(1,0); glVertex3f(+1,-1,-1);
    glTexCoord2f(1,1); glVertex3f(+1,-1,+1);
    glTexCoord2f(0,1); glVertex3f(-1,-1,+1);
    //  End
    glEnd();
    //  Undo transofrmations
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}


static void Vertex(double th,double ph)
{
    // glVertex3d(Sin(th)*Cos(ph) , Sin(ph) , Cos(th)*Cos(ph));
    double x = sin(th)*cos(ph);
    double y = cos(th)*cos(ph);
    double z =         sin(ph);
    //  For a sphere at the origin, the position
    //  and normal vectors are the same
    glNormal3d(x,y,z);
    glTexCoord2d((90-th)/360.0+90,ph/360.0+0.5+rotation);
    glVertex3d(x,y,z);
}


static void ball(double x,double y,double z,double r)
{
    int th,ph;
    float yellow[] = {1.0,1.0,0.0,1.0};
    float Emission[]  = {0.0,0.0,0.01*emission,1.0};
    //  Save transformation
    glPushMatrix();
    //  Offset, scale and rotate
    glTranslated(x,y,z);
    glScaled(r,r,r);
    //  White ball
    glMaterialfv(GL_FRONT,GL_SHININESS,shinyvec);
    glMaterialfv(GL_FRONT,GL_SPECULAR,yellow);
    glMaterialfv(GL_FRONT,GL_EMISSION,Emission);
    
    //glEnable(GL_TEXTURE_2D);
    //glBindTexture(GL_TEXTURE_2D, texture[2]);
    //  Bands of latitude
    for (ph=-90;ph<90;ph+=inc)
    {
        glBegin(GL_QUAD_STRIP);
        for (th=0;th<=360;th+=2*inc)
        {
            Vertex(th,ph);
            Vertex(th,ph+inc);
        }
        glEnd();
    }
    //  Undo transofrmations
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

static void person(double x, double y , double z, double r, double height)
{
    ball(z, y, z, r);
    height += 1; // no unused variable warning
    // int h,ph;
    // float yellow[] = {1.0,1.0,0.0,1.0};
    // float Emission[]  = {0.0,0.0,0.01*emission,1.0};
    // //  Save transformation
    // glPushMatrix();
    // //  Offset, scale and rotate
    // glTranslated(x,y,z);
    // glScaled(r,r,r);
    // //  White ball
    // glMaterialfv(GL_FRONT,GL_SHININESS,shinyvec);
    // glMaterialfv(GL_FRONT,GL_SPECULAR,yellow);
    // glMaterialfv(GL_FRONT,GL_EMISSION,Emission);
    // //  Bands of latitude
    // for (ph=-90;ph<90;ph+=inc)
    // {
    //    glBegin(GL_QUAD_STRIP);
    //    for (h = -height/2; h < height/2; h+=inc)
    //    {
    //       glNormal3d(Cos(ph),Sin(ph),h);
    //       glVertex3d(Cos(ph),Sin(ph),h);
    //       glNormal3d(Cos(ph+inc),Sin(ph+inc),h+inc);
    //       glVertex3d(Cos(ph+inc),Sin(ph+inc),h+inc);
    //       // Vertex(th,ph);
    //       // Vertex(th,ph+inc);
    //    }
    //    glEnd();
    // }
    // //  Undo transofrmations
    // glPopMatrix();
    
}

static void passenger_box(double x,double y,double z,
                          double dx,double dy,double dz,
                          double th)
{
    //  Set specular color to white
    float white[] = {1,1,1,1};
    float Emission[]  = {0.0,0.0,0.01*emission,1.0};
    glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,shinyvec);
    glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
    glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,Emission);
    //  Save transformation
    glPushMatrix();
    //  Offset
    glTranslated(x,y,z);
    glRotated(th,0,1,0);
    glScaled(dx,dy,dz);
    person(0, +1.5, 0, 0.5, 0.5);
    // Textures
    //glEnable(GL_TEXTURE_2D);
    //glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
    //glColor3f(1,1,1);
    //glBindTexture(GL_TEXTURE_2D,texture[0]);
    //  passenger_box
    // ball(0, +1.5, 0, 0.5);
    //  Front
    // glBindTexture(GL_TEXTURE_2D,texture[0]);
    glBegin(GL_QUADS);
    glNormal3f( 0, 0, 1);
    glTexCoord2f(0,0); glVertex3f(-1,-1, 1);
    glTexCoord2f(1,0); glVertex3f(+1,-1, 1);
    glTexCoord2f(1,1); glVertex3f(+1,+1, 1);
    glTexCoord2f(0,1); glVertex3f(-1,+1, 1);
    glEnd();
    //  Back
    // glBindTexture(GL_TEXTURE_2D,texture[0]);
    glBegin(GL_QUADS);
    glNormal3f( 0, 0,-1);
    glTexCoord2f(0,0); glVertex3f(+1,-1,-1);
    glTexCoord2f(1,0); glVertex3f(-1,-1,-1);
    glTexCoord2f(1,1); glVertex3f(-1,+1,-1);
    glTexCoord2f(0,1); glVertex3f(+1,+1,-1);
    glEnd();
    //  Right
    // glBindTexture(GL_TEXTURE_2D,texture[0]);
    glBegin(GL_QUADS);
    glNormal3f(+1, 0, 0);
    glTexCoord2f(0,0); glVertex3f(+1,-1,+1);
    glTexCoord2f(1,0); glVertex3f(+1,-1,-1);
    glTexCoord2f(1,1); glVertex3f(+1,+1,-1);
    glTexCoord2f(0,1); glVertex3f(+1,+1,+1);
    glEnd();
    //  Left
    // glBindTexture(GL_TEXTURE_2D,texture[0]);
    glBegin(GL_QUADS);
    glNormal3f(-1, 0, 0);
    glTexCoord2f(0,0); glVertex3f(-1,-1,-1);
    glTexCoord2f(1,0); glVertex3f(-1,-1,+1);
    glTexCoord2f(1,1); glVertex3f(-1,+1,+1);
    glTexCoord2f(0,1); glVertex3f(-1,+1,-1);
    glEnd();
    //  Bottom
    // glBindTexture(GL_TEXTURE_2D,texture[0]);
    glBegin(GL_QUADS);
    glNormal3f( 0,-one, 0);
    glTexCoord2f(0,0); glVertex3f(-1,-1,-1);
    glTexCoord2f(1,0); glVertex3f(+1,-1,-1);
    glTexCoord2f(1,1); glVertex3f(+1,-1,+1);
    glTexCoord2f(0,1); glVertex3f(-1,-1,+1);
    glEnd();
    //  End
    //  Undo transofrmations
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

static void light(double x,double y,double z,double r, double start_angle, double end_angle)
{
    const int d=5;
    int th,ph;
    
    //  Save transformation
    glPushMatrix();
    //  Offset and scale
    glTranslated(x,y,z);
    glScaled(r,r,r);
    glColor3f(sin(10*x), sin(7*x+3*y), cos(y*y+x*4));
    //  Latitude bands
    for (ph=start_angle;ph<end_angle;ph+=d)
    {
        glBegin(GL_QUAD_STRIP);
        for (th=90;th<=270;th+=d)
        {
            Vertex(th,ph);
            Vertex(th,ph+d);
        }
        glEnd();
    }
    //  Undo transformations
    glPopMatrix();
}


static void ferris_wheel(double x, double y , double z, double size)
{
    double pi = M_PI;
    double step = 2*pi/10;
    double r = 3;
    int i;
    //  Draw passenger_boxes
    glPushMatrix();
    glTranslated(x,y,z);
    glScaled(size,size,size);
    for (i=0;i<10;i++)
    {
        double passenger_box_x = r * cos(i*step+rotation);
        double passenger_box_y = r * sin(i*step+rotation);
        passenger_box(passenger_box_x, passenger_box_y,0 , 0.3,0.3,0.3 , 0);
    }
    
    // Draw Circle
    outer_frame(0, 0, -0.31, r, 10, step);
    outer_frame(0, 0, 0.31, r, 10, step);
    
    
    // Draw the spokes
    glColor3f(0.5,0.5,0.5);
    double conversion = 180/pi;
    int current_spoke;
    int num_spokes = spokes;
    
    double offset = 0;
    for(current_spoke = 0; current_spoke < num_spokes; current_spoke++)
    {
        offset = current_spoke*(180/num_spokes);
        beam(0,0,-0.3, r-0.1, 0.1, 0.1,offset + conversion*rotation, offset+conversion*rotation, 0);
        beam(0,0,0.3, r-0.1, 0.1, 0.1,offset + conversion*rotation, offset+conversion*rotation, 0);
    }
    
    // center axis
    glColor3f(0.2,0.5,0.2);
    beam(0,0,0, 1, 0.1, 0.1, 90, 0, 0);
    
    // braces
    glColor3f(0.2,0.2,0.5);
    beam(1.7,-1.7,-1, 2.5, 0.1, 0.1, 135, 135, 0);
    beam(-1.7,-1.7,-1, 2.5, 0.1, 0.1, 45, 45, 0);
    beam(1.7,-1.7,1, 2.5, 0.1, 0.1, 135, 135, 0);
    beam(-1.7,-1.7,1, 2.5, 0.1, 0.1, 45, 45, 0);
    
    
    if(num_lights){
        int spacing;
        r = r-0.1;
        num_spokes = num_spokes*2;
        double x_angle, y_angle, radius;
        for(current_spoke = 0; current_spoke < num_spokes; current_spoke++)
        {
            offset = current_spoke*2*pi/num_spokes;
            for(spacing = 1; spacing < num_lights+1; spacing++)
            {
                // light(spacing*r*cos(offset + rotation)/num_lights,
                //       spacing*r*sin(offset + rotation)/num_lights, 0.35, 0.1, 90, 270);
                radius = spacing*r/num_lights;
                x_angle = cos(offset + rotation);
                y_angle = sin(offset + rotation);
                
                light(radius*x_angle, radius*y_angle, 0.35, 0.1, 0, 180);
                light(radius*x_angle, radius*y_angle, -0.35, 0.1, -180, 0);
            }
        }
    }
    glPopMatrix();
    
}

bool Ferris::Initialize(double x, double y, double z)
{
    display_list = glGenLists(1);
    glNewList(display_list, GL_COMPILE);
    
    
    
    glEndList();
    
    // We only do all this stuff once, when the GL context is first set up.
    initialized = true;
    
    return true;
}


// Draw just calls the display list we set up earlier.
void Ferris::Draw(void)
{
    glPushMatrix();
    glTranslated(-20, -30, 18);
    glRotated(45, 0, 0, 1);
    glRotated(90, 1, 0, 0);
    glDisable(GL_CULL_FACE);
    ferris_wheel(0, 0, 0, 5);
    glEnable(GL_CULL_FACE);
    glCallList(display_list);
    glPopMatrix();
}


void Ferris::Update(float dt)
{
    rotation += 0.01f;
    while(gDegreesRotated > 360.0f)
        gDegreesRotated -= 360.0f;
}


