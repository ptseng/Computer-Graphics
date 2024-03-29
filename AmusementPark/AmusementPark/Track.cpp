/*
 * Track.cpp: A class that draws the train and its track.
 *
 * (c) 2001-2002: Stephen Chenney, University of Wisconsin at Madison.
 */


#include <iostream>
#include "Track.h"
#include <stdio.h>
#include <FL/math.h>
#include <OPENGL/glu.h>


// The control points for the track spline.
const int   Track::TRACK_NUM_CONTROLS = 4;
const float Track::TRACK_CONTROLS[TRACK_NUM_CONTROLS][3] =
		{ { -20.0, -20.0, -15.0 }, { 20.0, -20.0, 50.0 },
		  { 20.0, 20.0, -15.0 }, { -20.0, 20.0, 50.0 } };

// The carriage energy and mass
const float Track::TRAIN_ENERGY = 300.0f;


// Normalize a 3d vector.
static void
Normalize_3(float v[3])
{
    double  l = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);

    if ( l == 0.0 )
	return;

    v[0] /= (float)l;
    v[1] /= (float)l;
    v[2] /= (float)l;
}


// Destructor
Track::~Track(void)
{
    if ( initialized )
    {
	glDeleteLists(track_list, 1);
	glDeleteLists(train_list, 1);
    }
}


// Initializer. Would return false if anything could go wrong.
bool Track::Initialize(float x, float y, float z)
{
    CubicBspline    refined(3, true);
    int		    n_refined;
    float	    p[3];
    float       q[3];
    int		    i;

    // Create the track spline.
    track = new CubicBspline(3, true);
    for ( i = 0 ; i < TRACK_NUM_CONTROLS ; i++ )
	track->Append_Control(TRACK_CONTROLS[i]);

    // Refine it down to a fixed tolerance. This means that any point on
    // the track that is drawn will be less than 0.1 units from its true
    // location. In fact, it's even closer than that.
    track->Refine_Tolerance(refined, 0.1f);
    n_refined = refined.N();

    // Create the display list for the track - just a set of line segments
    // We just use curve evaluated at integer paramer values, because the
    // subdivision has made sure that these are good enough.
    track_list = glGenLists(1);
    glNewList(track_list, GL_COMPILE);
    glTranslatef(x, y, z);
	glColor3f(150/255.0f, 75/255.0f, 0.0f);
    
    glPushMatrix();
    glTranslated(-2.5f, 0, -2.5f);
    glDisable(GL_CULL_FACE);
    
    for ( float i = 0.0f ; i <= n_refined ; i = i + 1.5f )
    {
        refined.Evaluate_Point(i, q);
        
        glBegin(GL_QUADS);
        glNormal3f(0, 0, 1);
        glVertex3f(q[0], q[1], q[2]);
        glVertex3f(q[0]+5.0f, q[1], q[2]);
        glVertex3f(q[0]+5.0f, q[1]+1.0f, q[2]);
        glVertex3f(q[0], q[1]+1.0f, q[2]);
        glEnd();
        
        glBegin(GL_QUADS);
        glNormal3f(0, -1, 0);
        glVertex3f(q[0], q[1], q[2]);
        glVertex3f(q[0], q[1]+1.0f, q[2]);
        glVertex3f(q[0], q[1]+1.0f, q[2]-1.0f);
        glVertex3f(q[0], q[1], q[2]-1.0f);
        glEnd();
        
        glBegin(GL_QUADS);
        glVertex3f(q[0]+5.0f, q[1], q[2]);
        glVertex3f(q[0]+5.0f, q[1]+1.0f, q[2]);
        glVertex3f(q[0]+5.0f, q[1]+1.0f, q[2]-1.0f);
        glVertex3f(q[0]+5.0f, q[1], q[2]-1.0f);
        glEnd();
        
        glBegin(GL_QUADS);
        glNormal3f(0, 0, -1);
        
        glVertex3f(q[0], q[1], q[2]-1.0f);
        glVertex3f(q[0]+5.0f, q[1], q[2]-1.0f);
        glVertex3f(q[0]+5.0f, q[1]+1.0f, q[2]-1.0f);
        glVertex3f(q[0], q[1]+1.0f, q[2]-1.0f);
        glEnd();
        
        
    }
    
    glEnable(GL_CULL_FACE);
    glPopMatrix();
    
	glBegin(GL_LINE_STRIP);
	    for ( i = 0 ; i <= n_refined ; i++ )
	    {
            refined.Evaluate_Point((float)i, p);
            glVertex3fv(p);
	    }
	glEnd();
    
    glEndList();

    // Set up the train. At this point a cube is drawn. NOTE: The
    // x-axis will be aligned to point along the track. The origin of the
    // train is assumed to be at the bottom of the train.
    train_list = glGenLists(1);
    glNewList(train_list, GL_COMPILE);
    glColor3f(1.0, 0.0, 0.0);
    glBegin(GL_QUADS);
	glNormal3f(0.0f, 0.0f, 1.0f);
	glVertex3f(0.5f, 0.5f, 1.0f);
	glVertex3f(-0.5f, 0.5f, 1.0f);
	glVertex3f(-0.5f, -0.5f, 1.0f);
	glVertex3f(0.5f, -0.5f, 1.0f);

	glNormal3f(0.0f, 0.0f, -1.0f);
	glVertex3f(0.5f, -0.5f, 0.0f);
	glVertex3f(-0.5f, -0.5f, 0.0f);
	glVertex3f(-0.5f, 0.5f, 0.0f);
	glVertex3f(0.5f, 0.5f, 0.0f);

	glNormal3f(1.0f, 0.0f, 0.0f);
	glVertex3f(0.5f, 0.5f, 0.0f);
	glVertex3f(0.5f, 0.5f, 1.0f);
	glVertex3f(0.5f, -0.5f, 1.0f);
	glVertex3f(0.5f, -0.5f, 0.0f);

	glNormal3f(-1.0f, 0.0f, 0.0f);
	glVertex3f(-0.5f, 0.5f, 1.0f);
	glVertex3f(-0.5f, 0.5f, 0.0f);
	glVertex3f(-0.5f, -0.5f, 0.0f);
	glVertex3f(-0.5f, -0.5f, 1.0f);

	glNormal3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.5f, 0.5f, 1.0f);
	glVertex3f(0.5f, 0.5f, 0.0f);
	glVertex3f(-0.5f, 0.5f, 0.0f);
	glVertex3f(-0.5f, 0.5f, 1.0f);

	glNormal3f(0.0f, -1.0f, 0.0f);
	glVertex3f(0.5f, -0.5f, 0.0f);
	glVertex3f(0.5f, -0.5f, 1.0f);
	glVertex3f(-0.5f, -0.5f, 1.0f);
	glVertex3f(-0.5f, -0.5f, 0.0f);
    glEnd();
    glEndList();

    initialized = true;

    return true;
}


// Draw
GLfloat * Track::Draw(void)
{
    float   posn[3];
    float   tangent[3];
    double  angle;

    if ( ! initialized )
        return NULL;

    glPushMatrix();

    // Draw the track
    glCallList(track_list);

    glPushMatrix();

    // Figure out where the train is
    track->Evaluate_Point(posn_on_track, posn);
    
    // Translate the train to the point
    glTranslatef(posn[0], posn[1], posn[2]);

    // ...and what it's orientation is
    track->Evaluate_Derivative(posn_on_track, tangent);
    Normalize_3(tangent);
    
    // Rotate it to poitn along the track, but stay horizontal
    angle = atan2(tangent[1], tangent[0]) * 180.0 / M_PI;
    glRotatef((float)angle, 0.0f, 0.0f, 1.0f);

    // Another rotation to get the tilt right.
    angle = asin(-tangent[2]) * 180.0 / M_PI;
    
    //std::cout << angle << std::endl;
    
    glRotatef((float)angle, 0.0f, 1.0f, 0.0f);
    
    glGetFloatv(GL_MODELVIEW_MATRIX, ride_matrix);
    
    // Draw the train
    glCallList(train_list);

    glPopMatrix();
    glPopMatrix();
    
    return ride_matrix;
}


void
Track::Update(float dt)
{
    float   point[3];
    float   deriv[3];
    double  length;
    double  parametric_speed;

    if ( ! initialized )
	return;

    // First we move the train along the track with its current speed.

    // Get the derivative at the current location on the track.
    track->Evaluate_Derivative(posn_on_track, deriv);

    // Get its length.
    length = sqrt(deriv[0]*deriv[0] + deriv[1]*deriv[1] + deriv[2]*deriv[2]);
    if ( length == 0.0 )
	return;

    // The parametric speed is the world train speed divided by the length
    // of the tangent vector.
    parametric_speed = speed / length;

    // Now just evaluate dist = speed * time, for the parameter.
    posn_on_track += (float)(parametric_speed * dt);

    // If we've just gone around the track, reset back to the start.
    if ( posn_on_track > track->N() )
	posn_on_track -= track->N();

    // As the second step, we use conservation of energy to set the speed
    // for the next time.
    // The total energy = z * gravity + 1/2 speed * speed, assuming unit mass
    track->Evaluate_Point(posn_on_track, point);
    if ( TRAIN_ENERGY - 9.81 * point[2] < 0.0 )
	speed = 0.0;
    else
	speed = (float)sqrt(2.0 * ( TRAIN_ENERGY - 9.81 * point[2] ));
}


