#include <pebble.h>
#include "ppspinner.h"
#include "pptoaster.h"

static Window *window;
static TextLayer *temperature_text;
static TextLayer *ext_temperature_text;
static TextLayer *conso_text;
static TextLayer *alarm_text;
static TextLayer *alarmIcon;
static TextLayer *tempInHouseIcon;
static TextLayer *tempOutHouseIcon;
static TextLayer *electIcon;
static TextLayer *consoType;
static PPSpinnerLayer *spinner;

static const uint32_t const segments[] = { 100 };
VibePattern pat = {
  .durations = segments,
  .num_segments = ARRAY_LENGTH(segments),
};

enum WeatherKey {
  SENDING_KEY = 0x0,
  TEMPERATURE_KEY = 0x1,
  CONSO_KEY = 0x2,
  ALARM_KEY = 0x3,
  EXT_TEMPERATURE_KEY = 0x4,
  CONSO_HP_KEY = 0x5,
  ERROR_KEY = 0x6
};

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  //text_layer_set_text(text_layer, "Select");
  ppspinner_start(spinner);
  layer_set_hidden(spinner, false);
  
  Tuplet symbol_tuple = TupletCString(SENDING_KEY, "ref");

  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);

  dict_write_tuplet(iter, &symbol_tuple);
  dict_write_end(iter);

  app_message_outbox_send();
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Sending message to the phone");
  
  vibes_enqueue_custom_pattern(pat);
}

// Alarm ON
static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  //text_layer_set_text(text_layer, "Up");
  ppspinner_start(spinner);
  layer_set_hidden(spinner, false);
  
  Tuplet symbol_tuple = TupletCString(SENDING_KEY, "alOn");

  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);

  dict_write_tuplet(iter, &symbol_tuple);
  dict_write_end(iter);

  app_message_outbox_send();
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Sending message to the phone");
  vibes_enqueue_custom_pattern(pat);
}

// Alarm OFF
static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  //text_layer_set_text(text_layer, "Down");
  ppspinner_start(spinner);
  layer_set_hidden(spinner, false);
  
  Tuplet symbol_tuple = TupletCString(SENDING_KEY, "alOff");

  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);

  dict_write_tuplet(iter, &symbol_tuple);
  dict_write_end(iter);

  app_message_outbox_send();
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Sending message to the phone");
  vibes_enqueue_custom_pattern(pat);
}


// App Messages

enum {
    AKEY_NUMBER,
    AKEY_TEXT,
  };

static void in_received_handler(DictionaryIterator *iter, void *context) {
  // incoming message received
  ppspinner_stop(spinner);
  layer_set_hidden(spinner, true);
  
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Received message from the phone");

  Tuple *tuple = dict_read_first(iter);
  while (tuple) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, tuple->value->cstring);
  char *string = tuple->value->cstring;
  switch (tuple->key) {
    case TEMPERATURE_KEY:
      APP_LOG(APP_LOG_LEVEL_DEBUG, "Case temp : %s", string);
      text_layer_set_text(temperature_text, string);
      break;
    case CONSO_KEY:
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Case conso : %s", string);
      text_layer_set_text(conso_text, string);
      break;
    case ALARM_KEY:
        APP_LOG(APP_LOG_LEVEL_DEBUG, "%s", string);
        text_layer_set_text(alarm_text, string);
      break;
    case EXT_TEMPERATURE_KEY:
        APP_LOG(APP_LOG_LEVEL_DEBUG, "%s", string);
        text_layer_set_text(ext_temperature_text, string);
      break;
    case CONSO_HP_KEY:
        APP_LOG(APP_LOG_LEVEL_DEBUG, "%s", string);
        if (strcmp("0",string) != 0) {
          text_layer_set_text(consoType, "\uf0fe");
        } else {
          text_layer_set_text(consoType, "\uf056");
        }
      text_layer_set_text(ext_temperature_text, string);
      break;
    case ERROR_KEY:
        APP_LOG(APP_LOG_LEVEL_DEBUG, "%s", string);
        pptoaster_pop(string, 2000, PPToasterAppearFromBottom);
      break;
  }
  
  if (tuple->key != ERROR_KEY)
  {
    pptoaster_pop("Updated !", 2000, PPToasterAppearFromBottom);
  }

  tuple = dict_read_next(iter);
}

}

 void out_sent_handler(DictionaryIterator *sent, void *context) {
   // outgoing message was delivered

 }


 void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context) {
   // outgoing message failed

 }


 void in_dropped_handler(AppMessageResult reason, void *context) {
   // incoming message dropped
  ppspinner_stop(spinner);
  layer_set_hidden(spinner, true);
 }
// Fin App messages


static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void layer_update_callback(Layer *me, GContext* ctx) {
  // Line
  //graphics_draw_line(ctx, (GPoint){ 0, 42 }, (GPoint){ 820, 42 });
  
  // 144*168
  /*GBitmap *image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_NO_LITTER);
  
  
  GRect boundsIm = image->bounds;
  //graphics_draw_bitmap_in_rect(ctx, image, (GRect) { .origin = { 5, 5 }, .size = boundsIm.size });
  graphics_draw_bitmap_in_rect(ctx, image, (GRect) { .origin = { 0, 130 }, .size = {14,21} });
  gbitmap_destroy(image);*/
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  GFont awesome_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_AWESOME_18));
  spinner = ppspinner_create(
                               GRect((bounds.size.w/2)-12, 32, 25, 10), //frame
                               3, //segments
                               2, //corner radius
                               250 //ms between updates
                               );
  
  // External Temperature
  // ********************
  ext_temperature_text = text_layer_create((GRect) { .origin = { 20, 42 }, .size = { bounds.size.w - 20, 30 } });
  text_layer_set_text_alignment(ext_temperature_text, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(ext_temperature_text));
  text_layer_set_font(ext_temperature_text, fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21));
  text_layer_set_text(ext_temperature_text, "-");
  
  tempOutHouseIcon = text_layer_create((GRect) { .origin = {31, 45 }, .size = { 30, 30 } });
  text_layer_set_text_color(tempOutHouseIcon, GColorBlack); 
  text_layer_set_text_alignment(tempOutHouseIcon, GTextAlignmentCenter);
  text_layer_set_font(tempOutHouseIcon, awesome_font);
  text_layer_set_text(tempOutHouseIcon, "\uf0c2");
  layer_add_child(window_layer, text_layer_get_layer(tempOutHouseIcon));
  // ********************
  
  // Temperature
  // ********************
  temperature_text = text_layer_create((GRect) { .origin = { 20, 72 }, .size = { bounds.size.w - 20, 30 } });
  text_layer_set_text_alignment(temperature_text, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(temperature_text));
  text_layer_set_font(temperature_text, fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21));
  text_layer_set_text(temperature_text, "-");
  
  tempInHouseIcon = text_layer_create((GRect) { .origin = {30, 75 }, .size = { 30, 30 } });
  text_layer_set_text_color(tempInHouseIcon, GColorBlack);  
  text_layer_set_text_alignment(tempInHouseIcon, GTextAlignmentCenter);
  text_layer_set_font(tempInHouseIcon, awesome_font);
  text_layer_set_text(tempInHouseIcon, "\uf015");
  layer_add_child(window_layer, text_layer_get_layer(tempInHouseIcon));
  // ********************
  
  
  // Consommation
  // ********************
  conso_text = text_layer_create((GRect) { .origin = { 0, 115 }, .size = { bounds.size.w, 60 } });
  text_layer_set_text_alignment(conso_text, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(conso_text));
  text_layer_set_background_color(conso_text, GColorBlack);
  text_layer_set_font(conso_text, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text_color(conso_text, GColorWhite);
  text_layer_set_text(conso_text, "-");
  
  electIcon = text_layer_create((GRect) { .origin = {0, 120 }, .size = { 30, 40 } });
  text_layer_set_text_color(electIcon, GColorBlack);  
  text_layer_set_text_alignment(electIcon, GTextAlignmentCenter);
  text_layer_set_background_color(electIcon, GColorBlack);
  text_layer_set_text_color(electIcon, GColorWhite);
  text_layer_set_font(electIcon, awesome_font);
  text_layer_set_text(electIcon, "\uf0e7");
  layer_add_child(window_layer, text_layer_get_layer(electIcon));
  
  consoType = text_layer_create((GRect) { .origin = {bounds.size.w - 30, 120 }, .size = { 30, 40 } });
  text_layer_set_text_color(consoType, GColorBlack);  
  text_layer_set_text_alignment(consoType, GTextAlignmentCenter);
  text_layer_set_background_color(consoType, GColorBlack);
  text_layer_set_text_color(consoType, GColorWhite);
  text_layer_set_font(consoType, awesome_font);
  text_layer_set_text(consoType, "\uf153");
  layer_add_child(window_layer, text_layer_get_layer(consoType));
  
  // ********************
  
  // Alarme
  // ********************
  alarm_text = text_layer_create((GRect) { .origin = { 0, 0 }, .size = { bounds.size.w, 30 } });
  text_layer_set_background_color(alarm_text, GColorBlack);
  text_layer_set_text_color(alarm_text, GColorWhite); 
  text_layer_set_text_alignment(alarm_text, GTextAlignmentCenter);
  //GFont custom_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_CRAZY_RASCALS_14));
  //text_layer_set_font(alarm_text, custom_font);
  text_layer_set_font(alarm_text, fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21));
  text_layer_set_text(alarm_text, "-");
  layer_add_child(window_layer, text_layer_get_layer(alarm_text));
  
  // Alarm Icon
  alarmIcon = text_layer_create((GRect) { .origin = { 10, 0 }, .size = { 30, 30 } });
  text_layer_set_background_color(alarmIcon, GColorBlack);
  text_layer_set_text_color(alarmIcon, GColorWhite);  
  text_layer_set_text_alignment(alarmIcon, GTextAlignmentCenter);
  text_layer_set_font(alarmIcon, awesome_font);
  text_layer_set_text(alarmIcon, "\uf0f3");
  layer_add_child(window_layer, text_layer_get_layer(alarmIcon));
  // ********************

  layer_add_child(window_layer, spinner);
  ppspinner_start(spinner);

  // Drawing
  //layer_set_update_proc(window_layer, layer_update_callback);  
}

static void window_unload(Window *window) {
  text_layer_destroy(temperature_text);
  text_layer_destroy(conso_text);
  text_layer_destroy(ext_temperature_text);
  text_layer_destroy(alarm_text);
  text_layer_destroy(alarmIcon);
  text_layer_destroy(tempInHouseIcon);
  text_layer_destroy(tempOutHouseIcon);
  text_layer_destroy(electIcon);
  text_layer_destroy(consoType);
  
  ppspinner_destroy(spinner);
}

static void init(void) {
  window = window_create();
  window_set_click_config_provider(window, click_config_provider);
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });

  const bool animated = true;
  window_stack_push(window, animated);

  app_message_register_inbox_received(in_received_handler);
  app_message_register_inbox_dropped(in_dropped_handler);
  app_message_register_outbox_sent(out_sent_handler);
  app_message_register_outbox_failed(out_failed_handler);
  
  const uint32_t inbound_size = 64;
  const uint32_t outbound_size = 64;
  app_message_open(inbound_size, outbound_size);
}

static void deinit(void) {
  window_destroy(window);
}

int main(void) {
  init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

  app_event_loop();
  deinit();
}
