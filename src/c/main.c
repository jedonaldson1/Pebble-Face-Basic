#include <pebble.h>

/*
Code written by Jordan Donaldson using the Pebble SDK, C, and JavaScript. Code written
  from scratch.

The code in this file handles the time, date, and battery level capabilities.
*/

//Pointers
//Main window
static Window *s_main_window;
//TextLayers
static TextLayer *s_time_layer;

//Function to handle creation of watch face
static void main_window_load(Window *window)
{
  //Get information about Window
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  
  //Create TextLayer with specific bounds (time)
  s_time_layer = text_layer_create(
    GRect(0, PBL_IF_ROUND_ELSE(58,52), bounds.size.w, 50));
  //Improve layout
  text_layer_set_background_color(s_time_layer,GColorClear);
  text_layer_set_text_color(s_time_layer, GColorWhite);
  text_layer_set_text(s_time_layer,"00:00");
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  //Add layer as child to Window
  layer_add_child(window_layer,text_layer_get_layer(s_time_layer));
}

//Function to handle destruction of watch face
static void main_window_unload(Window *window)
{
  //Destroy TextLayer (time)
  text_layer_destroy(s_time_layer);
}

//Function to create elements of watch face
static void init() {
  // Create main Window element and assign to pointer
  s_main_window = window_create();
  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers)
  {
    .load = main_window_load,
    .unload = main_window_unload
  });
  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
  //Sets background color of window
  window_set_background_color(s_main_window,GColorPurple);
}

//Function to destroy elements of watch face
static void deinit()
{
  //Destroy Window
  window_destroy(s_main_window);
}

//Main function to create and destroy watch face
int main(void)
{
  init();
  app_event_loop();
  deinit();
}