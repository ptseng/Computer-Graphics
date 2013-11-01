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


@implementation OpenGLView {

}

- (id)init {
    self = [super init];
    if (self) {
        [[self openGLContext] makeCurrentContext];
        lastTicks = clock();
        cubeRotationAngle = 0;
            }
    
    return self;
}

- (void)prepareOpenGL {
    
    // Turn on Retina if available
    [self setWantsBestResolutionOpenGLSurface:YES];
    
    // Gather bounds
    // Get view dimensions in pixels
    NSRect backingBounds = [self convertRectToBacking:[self bounds]];
    
    GLsizei backingPixelWidth  = (GLsizei)(backingBounds.size.width),
    backingPixelHeight = (GLsizei)(backingBounds.size.height);
    
    NSLog(@"Initial Window Size W:%d, H:%d", backingPixelWidth, backingPixelHeight);
    
    
    // Sets context to legacy version
    CGLContextObj ctx;
    CGLPixelFormatObj pix;
    GLint npix;
    
    CGLPixelFormatAttribute attribs[] = {
        kCGLPFAOpenGLProfile,
        (CGLPixelFormatAttribute) kCGLOGLPVersion_Legacy,
        0
    };
    
    CGLChoosePixelFormat(attribs, &pix, &npix);
    CGLCreateContext(pix, NULL, &ctx);
    CGLSetCurrentContext(ctx);
    
    printf("%s %s\n", glGetString(GL_RENDERER), glGetString(GL_VERSION));
    
    // Synchronize buffer swaps with vertical refresh rate
    GLint swapInt = 1;
    [[self openGLContext] setValues:&swapInt forParameter:NSOpenGLCPSwapInterval];
    
    // Set
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glMatrixMode(GL_MODELVIEW);
    
}



- (void)drawRect:(NSRect)cubeRect {
    
    long ticks = clock();
    //delta_t in millis.
    int delta_t = (int)((ticks - lastTicks)/(CLOCKS_PER_SEC/1000));
    int fps = delta_t > 0?(int) 1000/delta_t:1000;
    
    [[self window] setTitle:[NSString stringWithFormat:@"%d fps",
                             fps]];
    lastTicks = ticks;
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0, 0, 0, 0);

    
    cubeRotationAngle = cubeRotationAngle+(.5*delta_t);
    
    glLoadIdentity();
    glRotatef(cubeRotationAngle, 1, 0.3f, 0);
    
    [self createlist];
    
    [self drawcube];
    
    glFlush();
    [self setNeedsDisplay:YES];
}

- (void) createlist {
    
    // Now do the geometry. Create the display list.
    self->display_list = glGenLists(1);
    glNewList(GL_COMPILE_AND_EXECUTE, display_list);
    
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
    
    glEndList();

    
}
- (void) drawcube {

    //Draw
    glPushMatrix();
    glCallList(display_list);
    glPopMatrix();
    
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
    glViewport(0, 0, backingPixelWidth, backingPixelHeight);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(120.0f, aspectRatio, 0.1f, 256.0f);
    glTranslatef(0.0, 0.0, -3.0);
    
    glMatrixMode(GL_MODELVIEW);
}

@end

