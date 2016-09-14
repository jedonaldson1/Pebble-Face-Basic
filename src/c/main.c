#include <pebble.h>

/*
Code written by Jordan Donaldson using the Pebble SDK, C, and JavaScript. Code written from
  scratch.

The code in this file handles the time, date, and battery level capabilities.
*/

//Pointers
//Main window
static Window *s_main_window;

//Function to handle creation of watch face
static void main_window_load(Window *window)
{
  //code
}

//Function to handle destruction of watch face
static void main_window_unload(Window *window)
{
  //code
}

//Function to create elements of watch face
static void init()
{
  //code
}

//Function to destroy elements of watch face
static void deinit()
{
  //code
}

//Main function to create and destroy watch face
int main(void)
{
  init();
  app_event_loop();
  deinit();
}