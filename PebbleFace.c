#include <pebble.h>

static Window *window;
static TextLayer *text_layer;
bool safe_mode = false;
bool notif_mode = false;
int tick = 0;
bool check = false;
int avoid_counter = 0;

//THIS BE DA TEXT FUNCTION
static void TEXT(int n) {
	text_layer_set_text(text_layer, "You've been avoiding me :|");
	//from a to b
	//Your friend has not been responding n times
}

//this is for when ignored
static void alert() {
	text_layer_set_text(text_layer, "Alert has been sent out");
	//call 911
}

//this is what vibrates every tick_time
static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  	if (tick == 0)
		vibes_double_pulse();
	else {
		if (tick % 6 == 0 && !check)
			TEXT(0);
		else if (tick % 6 == 0) {
			check = false;
			vibes_double_pulse();
		}
	}
	tick++;
}
//these are the basic buttons that help verify a vibrate
static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
	if (!notif_mode)
		return;
	text_layer_set_text(text_layer, "Checked in!");
	avoid_counter = 0;
	check = true;
	tick_timer_service_unsubscribe();
	tick_timer_service_unsubscribe();
	tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
	if (!notif_mode)
		return;
	text_layer_set_text(text_layer, "Checked in!");
	avoid_counter = 0;
	check = true;
	tick_timer_service_unsubscribe();
	tick_timer_service_unsubscribe();
	tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
	if (!notif_mode)
		return;
	text_layer_set_text(text_layer, "Checked in!");
	avoid_counter = 0;
	check = true;
	tick_timer_service_unsubscribe();
	tick_timer_service_unsubscribe();
	tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
}

//this is the simple back button
static void back_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(text_layer, "Back");
  if (safe_mode || notif_mode)
    alert();
}

//this is for safety mode (when you manually click) and notify mode (when you get vibrations)
static void safety_mode(ClickRecognizerRef recognizer, void *context) {
  const uint16_t count = click_number_of_clicks_counted(recognizer);
	
	if (count == 2) {
		text_layer_set_text(text_layer, "Safety");
  		if (safe_mode) {
    		safe_mode = false;
    		notif_mode = false;
  		}
  		else
    		safe_mode = true;
	}
	else {
		text_layer_set_text(text_layer, "Notify");
		tick = 0;
  		if (notif_mode) {
    		safe_mode = false;
    		notif_mode = false;
    		tick_timer_service_unsubscribe();
  		}
  		else {
    		notif_mode = true;
    		tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
  		}
	}
}

//this makes all buttons follow their functions (pun intended)
static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
  window_multi_click_subscribe(BUTTON_ID_BACK, 2, 3, 0, true, safety_mode);
  window_single_click_subscribe(BUTTON_ID_BACK, back_click_handler);
}

//preliminary stuff for basic window running
static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  text_layer = text_layer_create(GRect(0, 72, bounds.size.w, 20));
  text_layer_set_text(text_layer, "Press a button");
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(text_layer));
}

//more prelim stuff
static void window_unload(Window *window) {
  text_layer_destroy(text_layer);
}

//the init function
static void init(void) {
	
	// Largest expected inbox and outbox message sizes
	const uint32_t inbox_size = 64;
	const uint32_t outbox_size = 256;

	// Open AppMessage
	app_message_open(inbox_size, outbox_size);
	
  window = window_create();
  window_set_click_config_provider(window, click_config_provider);
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  window_stack_push(window, animated);
}

//deinit
static void deinit(void) {
  window_destroy(window);
}

//main
int main(void) {
  init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

  app_event_loop();
  deinit();
}