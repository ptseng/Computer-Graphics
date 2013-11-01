//
//  OpenGLView.h
//  Amusement Park Cocoa
//
//  Created by Philip Tseng on 10/27/13.
//  Copyright (c) 2013 AppOven. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface OpenGLView : NSOpenGLView
{
    long lastTicks;
    float cubeRotationAngle;
    GLubyte display_list;
}

@end
