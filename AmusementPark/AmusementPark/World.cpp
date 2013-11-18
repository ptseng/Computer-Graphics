/*
 * World.cpp: Main program file for Project 3, CS 559
 *
 * (c) 2001-2002: Stephen Chenney
 */


#include <Fl/Fl.h>
#include "WorldWindow.h"
#include <stdio.h>
#include <FL/x.H>

// The time per frame, in seconds (enforced only by timeouts.)
const float  FRAME_TIME = 0.010f;
WorldWindow  * world_window = NULL; // The window with world view in it


// This callback is called every 40th of a second if the system is fast
// enough. You should change the variable FRAME_TIME defined above if you
// want to change the frame rate.
static void
Timeout_Callback(void *data)
{
    // Update the motion in the world. This both moves the view and
    // animates the train.
    world_window->Update(FRAME_TIME);
    world_window->redraw();

    // Do the timeout again for the next frame.
    Fl::repeat_timeout(FRAME_TIME, Timeout_Callback);
}


int main(int argc, char *argv[])
{
    world_window = new WorldWindow(0, 0, 1154, 700, "Amusement Park");
   
    Fl::visual(FL_RGB8|FL_DOUBLE|FL_DEPTH|FL_STENCIL);
    
    world_window->end();
    world_window->show(argc, argv);

    Fl::add_timeout(0.1, Timeout_Callback, NULL);

    Fl::flush();
    Fl::check();
    
    return Fl::run();
}


