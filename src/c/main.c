#include <pebble.h>

/*
Code written by Jordan Donaldson using the Pebble SDK, and C. Code written
  from scratch.

The code in this file handles the time, date, battery level capabilities, and vibration on disconnection.
*/

//Pointers
//Main window
static Window *s_main_window;
//TextLayers
static TextLayer *s_time_layer, *s_date_layer;
//Fonts
static GFont s_time_font, s_date_font;
//Images
static BitmapLayer *s_bt_icon_layer;
static GBitmap *s_bt_icon_bitmap;
//Other variables
static int s_battery_level;
static Layer *s_battery_layer;

//Code to actually update time
static void update_time()
{
  //Get a tm structure
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  //Write the current hours and minutes into a buffer
  static char s_buffer[8];
  strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ? 
           "%H:%M" : "%I:%M", tick_time);
  //Display this time on the TextLayer
  text_layer_set_text(s_time_layer, s_buffer);
  
  //Create a date buffer
  static char date_buffer[16];
  strftime(date_buffer, sizeof(date_buffer), "%a %b %e", tick_time);
  text_layer_set_text(s_date_layer, date_buffer);
}

//Function to handle time ticks
static void tick_handler(struct tm *tick_time, TimeUnits units_changed)
{
  update_time();
}

//Function to update battery level
static void battery_callback(BatteryChargeState state)
{
  //Record the new battery level
  s_battery_level = state.charge_percent;
  
  //Mark layer as dirty
  layer_mark_dirty(s_battery_layer);
}

static void battery_update_proc(Layer *layer, GContext *ctx)
{
  GRect bounds = layer_get_bounds(layer);
  
  //Find the width of the bar
  int width = (int)(float)(((float)s_battery_level / 100.0F) * 114.0F);
  
  //Draw the bar
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_rect(ctx, GRect(0, 0, width, bounds.size.h), 0, GCornerNone);
}

static void bluetooth_callback(bool connected)
{
  //Show icon if disconnected
  layer_set_hidden(bitmap_layer_get_layer(s_bt_icon_layer), connected);
  
  if(!connected)
  {
    //Issue a vibrating alert
    vibes_double_pulse();
  }
}

//Function to handle creation of watch face
static void main_window_load(Window *window)
{
  //Get information about Window
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  
  //Create TextLayer with specific bounds (time)
  s_time_layer = text_layer_create(
    GRect(0, PBL_IF_ROUND_ELSE(58,52), bounds.size.w, 50));
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_GEOSANS_LIGHT_44));
  //Improve layout
  text_layer_set_background_color(s_time_layer,GColorClear);
  text_layer_set_text_color(s_time_layer, GColorWhite);
  text_layer_set_font(s_time_layer, s_time_font);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  //Add layer as child to Window
  layer_add_child(window_layer,text_layer_get_layer(s_time_layer));
  
  //Create TextLayer with specific bounds (date)
  s_date_layer = text_layer_create(
    GRect(0, 105, bounds.size.w, 50));
  s_date_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_GEOSANS_LIGHT_20));
  //Improve layout
  text_layer_set_background_color(s_date_layer,GColorClear);
  text_layer_set_text_color(s_date_layer,GColorWhite);
  text_layer_set_font(s_date_layer, s_date_font);
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);
  //Add layer as child to Window
  layer_add_child(window_layer, text_layer_get_layer(s_date_layer));
  
  //Create battery meter layer
  s_battery_layer = layer_create(GRect(14, 160, 115, 3));
  layer_set_update_proc(s_battery_layer, battery_update_proc);
  //Add to Window
  layer_add_child(window_get_root_layer(window), s_battery_layer);
  
  //Create the Bluetooth icon GBitmap
  s_bt_icon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_BLUETOOTH_ICON);
  //Create the BitmapLayer to display the GBitmap
  s_bt_icon_layer = bitmap_layer_create(GRect(62, 12, 30, 30));
  bitmap_layer_set_bitmap(s_bt_icon_layer, s_bt_icon_bitmap);
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_bt_icon_layer));
  //Show the correct state of the BT connection from the start
  bluetooth_callback(connection_service_peek_pebble_app_connection());
}

//Function to handle destruction of watch face
static void main_window_unload(Window *window)
{
  //Destroy TextLayers
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_date_layer);
  
  //Destroy GFonts
  fonts_unload_custom_font(s_time_font);
  fonts_unload_custom_font(s_date_font);
  
  //Destroy Layers
  layer_destroy(s_battery_layer);
  bitmap_layer_destroy(s_bt_icon_layer);
  
  //Destroy Images
  gbitmap_destroy(s_bt_icon_bitmap);
}

//Function to create elements of watch face
static void init()
{
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
  
  //Make sure time is displayed from start
  update_time();
  
  //Sets background color of window
  window_set_background_color(s_main_window,GColorBlack);
  
  //Register with TickTimer service
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  
  //Register for battery level updates
  battery_state_service_subscribe(battery_callback);
  //Display battery level from the start
  battery_callback(battery_state_service_peek());
  
  //Register for bluetooth connection updates
  connection_service_subscribe((ConnectionHandlers) 
      {
        .pebble_app_connection_handler = bluetooth_callback
      });
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