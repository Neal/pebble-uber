#pragma once

#include <pebble.h>

typedef struct {
	uint8_t index;
	char name[12];
	char estimate[8];
	char surge[24];
	GBitmap *image;
	GRect image_bounds;
} Product;

extern Product* products;
extern char* error;
extern uint8_t num_products;
extern uint8_t selected_product;

void uber_init(void);
void uber_deinit(void);
void uber_in_received_handler(DictionaryIterator *iter);
void uber_out_sent_handler(DictionaryIterator *sent);
void uber_out_failed_handler(DictionaryIterator *failed, AppMessageResult reason);
void uber_refresh();
void reload_data_and_mark_dirty();
Product* product();
