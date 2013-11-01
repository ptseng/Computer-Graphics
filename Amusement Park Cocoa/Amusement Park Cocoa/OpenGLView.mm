//
//  OpenGLView.m
//  Amusement Park Cocoa
//
//  Created by Philip Tseng on 10/27/13.
//  Copyright (c) 2013 AppOven. All rights reserved.
//

#import "OpenGLView.h"
#import <OpenGL/gl.h>
#import <OpenGL/glu.h>

const double FOV_X = 45.0;

@implementation OpenGLView {
    long lastTicks;
    float cubeRotationAngle;
    
    Ground	    ground;	    // The ground object.
	Track  traintrack;	    // The train and track.
    
    const double FOV_X; // The horizontal field of view.
    
	float	phi;	// Viewer's inclination angle.
	float	theta;	// Viewer's azimuthal angle.
	float	dist;	// Viewer's distance from the look-at point.
	float	x_at;	// The x-coord to look at.
	float	y_at;	// The y-coord to look at. z_at is assumed 2.0.
    
	int     button;	// The mouse button that is down, -1 if none.
	int	x_last;	// The location of the most recent mouse event
	int	y_last;
	int	x_down; // The location of the mouse when the button was pushed
	int	y_down;
	float   phi_down;   // The view inclination angle when the mouse
    // button was pushed
	float   theta_down; // The view azimuthal angle when the mouse
    // button was pushed
	float	dist_down;  // The distance when the mouse button is pushed.
	float	x_at_down;  // The x-coord to look at when the mouse went down.
	float	y_at_down;  // The y-coord to look at when the mouse went down.
}

- (id)init {
    self = [super init];
    if (self) {
        lastTicks = clock();
        cubeRotationAngle = 0;
        
        // Initial viewing parameters.
        phi = 45.0f;
        theta = 0.0f;
        dist = 100.0f;
        x_at = 0.0f;
        y_at = 0.0f;
    }
    
    return self;
}


- (void)prepareOpenGL {
    
    float color[4];
    double  eye[3];
    float dir[4];

    // Turn on Retina if available
    [self setWantsBestResolutionOpenGLSurface:YES];
    
    // Gather bounds
    // Get view dimensions in pixels
    NSRect backingBounds = [self convertRectToBacking:[self bounds]];
    
    GLsizei backingPixelWidth  = (GLsizei)(backingBounds.size.width),
    backingPixelHeight = (GLsizei)(backingBounds.size.height);
    
    float width = (float) backingPixelWidth;
    float height = (float) backingPixelHeight;
    
    float aspectRatio = width / height;
    
    NSLog(@"Initial Window Size W:%d, H:%d", backingPixelWidth, backingPixelHeight);
    
    
    printf("%s %s\n", glGetString(GL_RENDERER), glGetString(GL_VERSION));
    
    
    // Synchronize buffer swaps with vertical refresh rate
    GLint swapInt = 1;
    [[self openGLContext] setValues:&swapInt forParameter:NSOpenGLCPSwapInterval];

    
    
    // Set
    
    // Sets the clear color to sky blue.
	glClearColor(0.53f, 0.81f, 0.92f, 1.0);
    
    // Turn on depth testing
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
    
	// Turn on back face culling. Faces with normals away from the viewer
	// will not be drawn.
	glEnable(GL_CULL_FACE);
    
	// Enable lighting with one light.
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);
    
	// Ambient and diffuse lighting track the current color.
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);
    
	// Turn on normal vector normalization. You don't have to give unit
	// normal vector, and you can scale objects.
	glEnable(GL_NORMALIZE);
    
    // Set up the persepctive transformation.
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
    double fov_y = 360.0f / M_PI * atan(height * tan(FOV_X * M_PI / 360.0) / width);
    //gluPerspective(fov_y, width / (float)height, 1.0f, 1000.0f);
    gluPerspective(100.0f, aspectRatio, 1.0f, 5.0f);
    // Do some light stuff. Diffuse color, and zero specular color
	// turns off specular lighting.
	color[0] = 1.0f; color[1] = 1.0f; color[2] = 1.0f; color[3] = 1.0f;
	glLightfv(GL_LIGHT0, GL_DIFFUSE, color);
	color[0] = 0.0f; color[1] = 0.0f; color[2] = 0.0f; color[3] = 1.0f;
	glLightfv(GL_LIGHT0, GL_SPECULAR, color);
    

    
    // Initialize all the objects.
	ground.Initialize();
	traintrack.Initialize();
    
    NSLog(@"Initialized Objects");
    glMatrixMode(GL_MODELVIEW);
    
}



- (void)drawRect:(NSRect)dirtyRect {
    
    double  eye[3];
    float dir[4];
    
    // Gather bounds
    // Get view dimensions in pixels
    NSRect backingBounds = [self convertRectToBacking:[self bounds]];
    
    GLsizei backingPixelWidth  = (GLsizei)(backingBounds.size.width),
    backingPixelHeight = (GLsizei)(backingBounds.size.height);
    
    float width = (float) backingPixelWidth;
    float height = (float) backingPixelHeight;
    
    long ticks = clock();
    //delta_t in millis.
    int delta_t = (int)((ticks - lastTicks)/(CLOCKS_PER_SEC/1000));
    int fps = delta_t > 0?(int) 1000/delta_t:1000;
    
    [[self window] setTitle:[NSString stringWithFormat:@"%d fps",
                             fps]];
    lastTicks = ticks;
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Set up the viewing transformation. The viewer is at a distance
    // dist from (x_at, y_ay, 2.0) in the direction (theta, phi) defined
    // by two angles. They are looking at (x_at, y_ay, 2.0) and z is up.
    eye[0] = x_at + dist * cos(theta * M_PI / 180.0) * cos(phi * M_PI / 180.0);
    eye[1] = y_at + dist * sin(theta * M_PI / 180.0) * cos(phi * M_PI / 180.0);
    eye[2] = 2.0 + dist * sin(phi * M_PI / 180.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    //gluLookAt(0, 0, 5.0f, 0, 0, 0, 0, 1, 0);
    //gluLookAt(eye[0], eye[1], eye[2], x_at, y_at, 2.0, 0.0, 0.0, 1.0);
    
    //NSLog(@"Update Eye: %f", dist);
    
    // Position the light source. This has to happen after the viewing
    // transformation is set up, so that the light stays fixed in world
    // space. This is a directional light - note the 0 in the w component.
    dir[0] = 1.0; dir[1] = 1.0; dir[2] = 1.0; dir[3] = 0.0;
    glLightfv(GL_LIGHT0, GL_POSITION, dir);
    
    
    
    // Draw stuff. Everything.
    ground.Draw();
    traintrack.Draw();
    //[self drawcube];
    
    
    glFlush();
    [self setNeedsDisplay:YES];
}


- (void) drawcube {
    
    //Draw
    
    float s = 1.0f;
    
    glBegin(GL_QUADS);
    {
        glColor3f(0, 0, 1);
        glVertex3f(-1*s,  1*s, -1*s); //F T L
        glColor3f(1, .75, 0);
        glVertex3f( 1*s,  1*s, -1*s); //F T R
        glColor3f(0, 1, 0);
        glVertex3f( 1*s, -1*s, -1*s); //F B R
        glColor3f(1, 0, 0);
        glVertex3f(-1*s, -1*s, -1*s); //F B L
        
        glColor3f(1, 0, 0);
        glVertex3f(-1*s, -1*s, -1*s); //F B L
        glColor3f(0, 1, 0);
        glVertex3f( 1*s, -1*s, -1*s); //F B R
        glColor3f(0, .5, 0);
        glVertex3f( 1*s, -1*s,  1*s); //B B R
        glColor3f(.5, 0, 0);
        glVertex3f(-1*s, -1*s,  1*s); //B B L
        
        glColor3f(0, 0, .5);
        glVertex3f(-1*s,  1*s,  1*s); //B T L
        glColor3f(0, 1, 1);
        glVertex3f( 1*s,  1*s,  1*s); //B T R
        glColor3f(0, .5, 0);
        glVertex3f( 1*s, -1*s,  1*s); //B B R
        glColor3f(.5, 0, 0);
        glVertex3f(-1*s, -1*s,  1*s); //B B L
        
        glColor3f(0, 0, .5);
        glVertex3f(-1*s,  1*s,  1*s); //B T L
        glColor3f(0, 0, 1);
        glVertex3f(-1*s,  1*s, -1*s); //F T L
        glColor3f(1, 0, 0);
        glVertex3f(-1*s, -1*s, -1*s); //F B L
        glColor3f(.5, 0, 0);
        glVertex3f(-1*s, -1*s,  1*s); //B B L
        
        glColor3f(0, 0, .5);
        glVertex3f(-1*s,  1*s,  1*s); //B T L
        glColor3f(0, 1, 1);
        glVertex3f( 1*s,  1*s,  1*s); //B T R
        glColor3f(1, .75, 0);
        glVertex3f( 1*s,  1*s, -1*s); //F T R
        glColor3f(0, 0, 1);
        glVertex3f(-1*s,  1*s, -1*s); //F T L
        
        glColor3f(1, .75, 0);
        glVertex3f( 1*s,  1*s, -1*s); //F T R
        glColor3f(0, 1, 1);
        glVertex3f( 1*s,  1*s,  1*s); //B T R
        glColor3f(0, .5, 0);
        glVertex3f( 1*s, -1*s,  1*s); //B B R
        glColor3f(0, 1, 0);
        glVertex3f( 1*s, -1*s, -1*s); //F B R
        
    }
    glEnd();
}



- (void)reshape {
    
    // Get view dimensions in pixels
    NSRect backingBounds = [self convertRectToBacking:[self bounds]];
    
    GLsizei backingPixelWidth  = (GLsizei)(backingBounds.size.width),
    backingPixelHeight = (GLsizei)(backingBounds.size.height);

    float width = (float) backingPixelWidth;
    float height = (float) backingPixelHeight;
    
    float aspectRatio = width / height;
    
    // Set viewport
    //glViewport(0, 0, backingPixelWidth, backingPixelHeight);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    double fov_y = 360.0f / M_PI * atan(height * tan(FOV_X * M_PI / 360.0) / width);
    //gluPerspective(fov_y, aspectRatio, 1.0, 1000.0);
    glTranslatef(0.0, 0.0, -5.0);
    
    glMatrixMode(GL_MODELVIEW);
}


/*
- (void)scrollWheel:(NSEvent *)theEvent
{
    // Gather bounds
    // Get view dimensions in pixels
    NSRect backingBounds = [self convertRectToBacking:[self bounds]];
    
    GLsizei backingPixelWidth  = (GLsizei)(backingBounds.size.width),
    backingPixelHeight = (GLsizei)(backingBounds.size.height);
    
    float width = (float) backingPixelWidth;
    float height = (float) backingPixelHeight;
    
    float wheelDelta = [theEvent deltaX] +[theEvent deltaY] + [theEvent deltaZ];
    
    if (wheelDelta)
    {
        //NSLog(@"Scroll Wheel Triggered: %f, current dist: %f", wheelDelta, dist);
        dist_down = dist;
        dist = dist*2;
        
        if ( dist < 1.0f )
            dist = 1.0f;
        // update projection matrix
        [self setNeedsDisplay: YES];
    }
}
*/

@end

