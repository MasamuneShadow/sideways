#include "pebble_os.h"
#include "pebble_app.h"

#define MY_UUID { 0xF3, 0x8F, 0x53, 0xB0, 0x63, 0x37, 0x44, 0x13, 0x87, 0xC1, 0x92, 0xE4, 0x82, 0xE7, 0x25, 0x15 }
PBL_APP_INFO(MY_UUID,
             "Sideways Watchface", "@Masamune_Shadow",
             1, 0, /* App version */
             RESOURCE_ID_IMAGE_MENU_ICON,
             APP_INFO_WATCH_FACE);

static Window window;
static Layer time_layer;
static Layer date_layer;
static Layer line_layer;
static Layer day_layer;

static const int BIG_DIGIT_IMAGE_RESOURCE_IDS[] = {
  RESOURCE_ID_IMAGE_BIG_0,
  RESOURCE_ID_IMAGE_BIG_1,
  RESOURCE_ID_IMAGE_BIG_2,
  RESOURCE_ID_IMAGE_BIG_3,
  RESOURCE_ID_IMAGE_BIG_4,
  RESOURCE_ID_IMAGE_BIG_5,
  RESOURCE_ID_IMAGE_BIG_6,
  RESOURCE_ID_IMAGE_BIG_7,
  RESOURCE_ID_IMAGE_BIG_8,
  RESOURCE_ID_IMAGE_BIG_9
};

static const int MED_DIGIT_IMAGE_RESOURCE_IDS[] = {
  RESOURCE_ID_IMAGE_MED_0,
  RESOURCE_ID_IMAGE_MED_1,
  RESOURCE_ID_IMAGE_MED_2,
  RESOURCE_ID_IMAGE_MED_3,
  RESOURCE_ID_IMAGE_MED_4,
  RESOURCE_ID_IMAGE_MED_5,
  RESOURCE_ID_IMAGE_MED_6,
  RESOURCE_ID_IMAGE_MED_7,
  RESOURCE_ID_IMAGE_MED_8,
  RESOURCE_ID_IMAGE_MED_9
};

static const int IMAGE_RESOURCE_DAY[] = {
    RESOURCE_ID_IMAGE_DAY_0,
    RESOURCE_ID_IMAGE_DAY_1,
    RESOURCE_ID_IMAGE_DAY_2,
    RESOURCE_ID_IMAGE_DAY_3,
    RESOURCE_ID_IMAGE_DAY_4,
    RESOURCE_ID_IMAGE_DAY_5,
    RESOURCE_ID_IMAGE_DAY_6
    };

static const int IMAGE_RESOURCE_SLASH[] = {
    RESOURCE_ID_IMAGE_SLASH,
    RESOURCE_ID_IMAGE_SLASH
    };
static const int IMAGE_RESOURCE_LINE[] = {
    RESOURCE_ID_IMAGE_LINE,
    RESOURCE_ID_IMAGE_LINE
    };
    
static const int IMAGE_RESOURCE_COLON[] = {
    RESOURCE_ID_IMAGE_COLON,
    RESOURCE_ID_IMAGE_COLON
    };


#define TOTAL_TIME_DIGITS 4
static BmpContainer time_digits_images[TOTAL_TIME_DIGITS];
static int time_resource_ids[TOTAL_TIME_DIGITS];

#define TOTAL_DATE_DIGITS 8
static BmpContainer date_digits_images[TOTAL_DATE_DIGITS];
static int date_resource_ids[TOTAL_DATE_DIGITS];

#define TOTAL_SLASHES 4
static BmpContainer slash[TOTAL_SLASHES];
static int slash_id[TOTAL_SLASHES];

#define TOTAL_COLONS 4 //just like irl lawl
static BmpContainer colon[TOTAL_COLONS];
static int colon_id[TOTAL_COLONS];

static BmpContainer line[TOTAL_SLASHES];
static int line_id[TOTAL_SLASHES];

#define TOTAL_DAYS 7
static BmpContainer day[TOTAL_DAYS];
static int day_id[TOTAL_DAYS];

unsigned short get_display_hour(unsigned short hour) {
  if (clock_is_24h_style()) {
    return hour;
  }
  unsigned short display_hour = hour % 12;
  // Converts "0" to "12"
  return display_hour ? display_hour : 12;
}

void set_container_image(BmpContainer *bmp_container, Layer *container_layer, int previous_resource_id, const int resource_id, GPoint origin) {
  if (previous_resource_id != resource_id) {
    layer_remove_from_parent(&bmp_container->layer.layer);
    bmp_deinit_container(bmp_container);
  
    bmp_init_container(resource_id, bmp_container);
  
    GRect frame = layer_get_frame(&bmp_container->layer.layer);
    frame.origin.x = origin.x;
    frame.origin.y = origin.y;
    layer_set_frame(&bmp_container->layer.layer, frame);
  
    layer_add_child(container_layer, &bmp_container->layer.layer);
  }
}

void update_display(PblTm *current_time) {
  unsigned short display_hour = get_display_hour(current_time->tm_hour);

short month = current_time->tm_mon + 1;
//Day Shorthand
//set_container_image(&date_digits_images[0], &date_layer, date_resource_ids[0], MED_DIGIT_IMAGE_RESOURCE_IDS[current_time->tm_mday/10], GPoint(0,  0));  
//  set_container_image(&date_digits_images[1], &date_layer, date_resource_ids[1], MED_DIGIT_IMAGE_RESOURCE_IDS[current_time->tm_mday%10], GPoint(0, 10));  
  set_container_image(&day[current_time->tm_wday], &day_layer, day_id[current_time->tm_wday], IMAGE_RESOURCE_DAY[current_time->tm_wday], GPoint(0, 0));  
//Month
  set_container_image(&date_digits_images[2], &date_layer, date_resource_ids[2], MED_DIGIT_IMAGE_RESOURCE_IDS[month/10], GPoint(0, 70));
  set_container_image(&date_digits_images[3], &date_layer, date_resource_ids[3], MED_DIGIT_IMAGE_RESOURCE_IDS[month%10], GPoint(0, 84));
  set_container_image(&slash[0], &date_layer, slash_id[0], IMAGE_RESOURCE_SLASH[0], GPoint(0, 98));
  set_container_image(&date_digits_images[0], &date_layer, date_resource_ids[0], MED_DIGIT_IMAGE_RESOURCE_IDS[current_time->tm_mday/10], GPoint(0,  105));  
  set_container_image(&date_digits_images[1], &date_layer, date_resource_ids[1], MED_DIGIT_IMAGE_RESOURCE_IDS[current_time->tm_mday%10], GPoint(0, 119));
  set_container_image(&slash[1], &date_layer, slash_id[1], IMAGE_RESOURCE_SLASH[1], GPoint(0, 133));
  static char yearText[] = "xxxx";
  string_format_time(yearText,sizeof(yearText),"%y", current_time);
  int yearDigitThree = yearText[0]-'0';
  /*if (i > 10)
  {
      i = i/10;
  }*/
  
  int yearDigitFour = yearText[1]-'0';
  /*while (i2 > 10)
  {
    i2 =- 10;
  }*/
  set_container_image(&date_digits_images[4], &date_layer, date_resource_ids[4], MED_DIGIT_IMAGE_RESOURCE_IDS[yearDigitThree], GPoint(0,  140));  
  set_container_image(&date_digits_images[5], &date_layer, date_resource_ids[5], MED_DIGIT_IMAGE_RESOURCE_IDS[yearDigitFour], GPoint(0, 154));  
  set_container_image(&line[0], &line_layer, line_id[0], IMAGE_RESOURCE_LINE[0], GPoint(0, 65));  
  //set_container_image(&date_digits_images[6], &date_layer, date_resource_ids[6], MED_DIGIT_IMAGE_RESOURCE_IDS[current_time->tm_mday/10], GPoint(0,  135));  
  //set_container_image(&date_digits_images[7], &date_layer, date_resource_ids[7], MED_DIGIT_IMAGE_RESOURCE_IDS[current_time->tm_mday/10], GPoint(0, 150));  
  
  set_container_image(&time_digits_images[0], &time_layer, time_resource_ids[0], BIG_DIGIT_IMAGE_RESOURCE_IDS[display_hour/10], GPoint(0, 0));  
  set_container_image(&time_digits_images[1], &time_layer, time_resource_ids[1], BIG_DIGIT_IMAGE_RESOURCE_IDS[display_hour%10], GPoint(0, 40));
  set_container_image(&colon[0], &time_layer, colon_id[0], IMAGE_RESOURCE_COLON[0], GPoint(0,  80));
  set_container_image(&time_digits_images[2], &time_layer, time_resource_ids[2], BIG_DIGIT_IMAGE_RESOURCE_IDS[current_time->tm_min/10], GPoint(0, 88));
  set_container_image(&time_digits_images[3], &time_layer, time_resource_ids[3], BIG_DIGIT_IMAGE_RESOURCE_IDS[current_time->tm_min%10], GPoint(0, 128));
  
  
  
}

void handle_init(AppContextRef ctx) {
  (void)ctx;

  window_init(&window, "Spencers Watchface");
  window_stack_push(&window, true /* Animated */);
  window_set_background_color(&window, GColorBlack);
  resource_init_current_app(&APP_RESOURCES);
  
  layer_init(&day_layer, GRect(100,5,50,168));
  layer_add_child(&window.layer, &day_layer);
  
  layer_init(&date_layer, GRect(112,0,32,168));
  layer_add_child(&window.layer, &date_layer);
  
  layer_init(&line_layer, GRect(103,0,6,168));
  layer_add_child(&window.layer, &line_layer);
  
  layer_init(&time_layer, GRect(20,0,80,168));
  layer_add_child(&window.layer, &time_layer);

  // fill containers so they are correctly released on first display
  for (int i = 0; i < TOTAL_TIME_DIGITS; i++) {
    bmp_init_container(RESOURCE_ID_IMAGE_BIG_0, &time_digits_images[i]);
    time_resource_ids[i] = -1;
  }

  for (int i = 0; i < TOTAL_DATE_DIGITS; i++) {
    bmp_init_container(RESOURCE_ID_IMAGE_MED_0, &date_digits_images[i]);
    date_resource_ids[i] = -1;
  }

  for (int i = 0; i < TOTAL_SLASHES; i++) {
    bmp_init_container(RESOURCE_ID_IMAGE_SLASH, &slash[i]);
    slash_id[i] = -1;
  }
  
  for (int i = 0; i < TOTAL_COLONS; i++) {
    bmp_init_container(RESOURCE_ID_IMAGE_COLON, &colon[i]);
    colon_id[i] = -1;
  }
  
  for (int i = 0; i < TOTAL_DAYS; i++) {
    bmp_init_container(RESOURCE_ID_IMAGE_DAY_0, &day[i]);
    day_id[i] = -1;
  }
  
  for (int i = 0; i < TOTAL_SLASHES; i++) {
    bmp_init_container(RESOURCE_ID_IMAGE_LINE, &line[i]);
    line_id[i] = -1;
  }
  PblTm tick_time;
  get_time(&tick_time);
  update_display(&tick_time);
}

void handle_deinit(AppContextRef ctx) {
  (void)ctx;
  for (int i = 0; i < TOTAL_TIME_DIGITS; i++) {
    bmp_deinit_container(&time_digits_images[i]);
  }
  for (int i = 0; i < TOTAL_DATE_DIGITS; i++) {
    bmp_deinit_container(&date_digits_images[i]);
  }
  for (int i = 0; i < TOTAL_SLASHES; i++) {
    bmp_deinit_container(&slash[i]);
  }
  for (int i = 0; i < TOTAL_COLONS; i++) {
    bmp_deinit_container(&colon[i]);
  }
  
  for (int i = 0; i < TOTAL_DAYS; i++) {
    bmp_deinit_container(&day[i]);
  }
  for (int i = 0; i < TOTAL_SLASHES; i++) {
    bmp_deinit_container(&line[i]);
  }
}


void handle_minute_tick(AppContextRef ctx, PebbleTickEvent *t) {
  (void)ctx;

  update_display(t->tick_time);
}

void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init,
    .tick_info = {
        .tick_handler = &handle_minute_tick,
        .tick_units = MINUTE_UNIT
    },
	.deinit_handler = &handle_deinit
  };
  app_event_loop(params, &handlers);
}
