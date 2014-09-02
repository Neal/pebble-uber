#include <pebble.h>
#include "products.h"
#include "../libs/pebble-assist.h"
#include "../uber.h"
#include "locations.h"

static uint16_t menu_get_num_sections_callback(struct MenuLayer *menu_layer, void *callback_context);
static uint16_t menu_get_num_rows_callback(struct MenuLayer *menu_layer, uint16_t section_index, void *callback_context);
static int16_t menu_get_header_height_callback(struct MenuLayer *menu_layer, uint16_t section_index, void *callback_context);
static int16_t menu_get_cell_height_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context);
static void menu_draw_header_callback(GContext *ctx, const Layer *cell_layer, uint16_t section_index, void *callback_context);
static void menu_draw_row_callback(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *callback_context);
static void menu_select_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context);
static void menu_select_long_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context);

static Window *window = NULL;
static MenuLayer *menu_layer = NULL;
static GBitmap *surge = NULL;

void products_init(void) {
	window = window_create();

	menu_layer = menu_layer_create_fullscreen(window);
	menu_layer_set_callbacks(menu_layer, NULL, (MenuLayerCallbacks) {
		.get_num_sections = menu_get_num_sections_callback,
		.get_num_rows = menu_get_num_rows_callback,
		.get_header_height = menu_get_header_height_callback,
		.get_cell_height = menu_get_cell_height_callback,
		.draw_header = menu_draw_header_callback,
		.draw_row = menu_draw_row_callback,
		.select_click = menu_select_callback,
		.select_long_click = menu_select_long_callback,
	});
	menu_layer_set_click_config_onto_window(menu_layer, window);
	menu_layer_add_to_window(menu_layer, window);

	surge = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_SURGE);

	window_stack_push(window, true);

	locations_init();
}

void products_deinit(void) {
	locations_deinit();
	gbitmap_destroy_safe(surge);
	menu_layer_destroy_safe(menu_layer);
	window_destroy_safe(window);
}

void products_reload_data_and_mark_dirty(void) {
	locations_reload_data_and_mark_dirty();
	menu_layer_reload_data_and_mark_dirty(menu_layer);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

static uint16_t menu_get_num_sections_callback(struct MenuLayer *menu_layer, void *callback_context) {
	return 1;
}

static uint16_t menu_get_num_rows_callback(struct MenuLayer *menu_layer, uint16_t section_index, void *callback_context) {
	return num_products ? num_products : 1;
}

static int16_t menu_get_header_height_callback(struct MenuLayer *menu_layer, uint16_t section_index, void *callback_context) {
	return 0;
}

static int16_t menu_get_cell_height_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context) {
	if (error) {
		return graphics_text_layout_get_content_size(error, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD), GRect(4, 2, 136, 128), GTextOverflowModeFill, GTextAlignmentLeft).h + 12;
	}
	if (cell_index->row < num_products && strlen(products[cell_index->row].surge) != 0) {
		return 48;
	}
	return 30;
}

static void menu_draw_header_callback(GContext *ctx, const Layer *cell_layer, uint16_t section_index, void *callback_context) {
}

static void menu_draw_row_callback(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *callback_context) {
	graphics_context_set_text_color(ctx, GColorBlack);
	if (error) {
		graphics_draw_text(ctx, error, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD), GRect(4, 2, 136, 128), GTextOverflowModeFill, GTextAlignmentLeft, NULL);
	} else if (num_products == 0) {
		graphics_draw_text(ctx, "Loading...", fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD), GRect(4, 2, 136, 22), GTextOverflowModeFill, GTextAlignmentLeft, NULL);
	} else {
		Product *product = product_get(cell_index->row);
		graphics_draw_bitmap_in_rect(ctx, product->image, product->image_bounds);
		graphics_draw_text(ctx, product->name, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD), GRect(24, 2, 116, 20), GTextOverflowModeFill, GTextAlignmentLeft, NULL);
		graphics_draw_text(ctx, product->estimate, fonts_get_system_font(FONT_KEY_GOTHIC_18), GRect(100, 2, 42, 22), GTextOverflowModeFill, GTextAlignmentRight, NULL);
		if (strlen(product->surge) != 0) {
			graphics_draw_text(ctx, product->surge, fonts_get_system_font(FONT_KEY_GOTHIC_18), GRect(24, 22, 116, 20), GTextOverflowModeFill, GTextAlignmentLeft, NULL);
			graphics_draw_bitmap_in_rect(ctx, surge, GRect(5, 28, 12, 13));
		}
	}
}

static void menu_select_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context) {
	if (num_products == 0) return;
	selected_product = cell_index->row;
	locations_show();
}

static void menu_select_long_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context) {
	uber_refresh();
}
