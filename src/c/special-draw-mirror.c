//
//  special-draw-mirror.c
//  Special Draw
//
//  Created by Christian Reinbacher on 12.06.16.
//  Copyright © 2016 Christian Reinbacher. All rights reserved.
//

#include "special-draw-mirror.h"

static bool prv_byte_get_bit(uint8_t *byte, uint8_t bit) {
  return ((*byte) >> bit) & 1;
}

static void prv_byte_set_bit(uint8_t *byte, uint8_t bit, uint8_t value) {
  *byte ^= (-value ^ *byte) & (1 << bit);
}

static GColor prv_get_pixel_color(GBitmapDataRowInfo info, int x, bool has_color) {
  if(has_color) {
    // Read the single byte color pixel
    return (GColor){ .argb = info.data[x] };
  } else {
    // Read the single bit of the correct byte
    uint8_t byte = x / 8;
    uint8_t bit = x % 8;
    return prv_byte_get_bit(&info.data[byte], bit) ? GColorWhite : GColorBlack;
  }
}

static void prv_set_pixel_color(GBitmapDataRowInfo info, int x,
                            GColor color, bool has_color) {
  if(has_color) {
  // Write the pixel's byte color
  memset(&info.data[x], color.argb, 1);
  } else {
    // Find the correct byte, then set the appropriate bit
    uint8_t byte = x / 8;
    uint8_t bit = x % 8;
    prv_byte_set_bit(&info.data[byte], bit, gcolor_equal(color, GColorWhite) ? 1 : 0);
  }
}

static void prv_mirror_bitmap(GBitmap * bitmap, bool horizontal) {
  GRect bounds = gbitmap_get_bounds(bitmap);
  GBitmapFormat bitmap_format = gbitmap_get_format(bitmap);
  
  bool has_color = !(bitmap_format == GBitmapFormat1Bit || bitmap_format == GBitmapFormat1BitPalette);
  if(horizontal) {
    for (int y = 0; y < bounds.size.h / 2; y++) {
      GBitmapDataRowInfo row_info_source = gbitmap_get_data_row_info(bitmap, y);
      GBitmapDataRowInfo row_info_dest   = gbitmap_get_data_row_info(bitmap, bounds.size.h-y-1);
      for (int x = row_info_source.min_x; x < row_info_source.max_x; x++) {
        GColor temp_pixel = prv_get_pixel_color(row_info_source,x,has_color);
        prv_set_pixel_color(row_info_source,x,prv_get_pixel_color(row_info_dest,x,has_color),has_color);
        prv_set_pixel_color(row_info_dest,x,temp_pixel,has_color);
      }
    }
  } else {
    for (int y = 0; y < bounds.size.h ; y++) {
      GBitmapDataRowInfo row_info = gbitmap_get_data_row_info(bitmap, y);
      int max_x = (row_info.max_x-row_info.min_x)/2 + row_info.min_x;
      for (int x = row_info.min_x; x < max_x; x++) {
        GColor temp_pixel = prv_get_pixel_color(row_info,x,has_color);
        prv_set_pixel_color(row_info,x,prv_get_pixel_color(row_info,bounds.size.w-x-1,has_color),has_color);
        prv_set_pixel_color(row_info,bounds.size.w-x-1,temp_pixel,has_color);
      }
    }
  }
}

static void prv_run_modifier(GSpecialSessionModifier * modifier,
                             GBitmap * session_bitmap) {
  prv_mirror_bitmap(session_bitmap, *((bool *) modifier->context));
}

static void prv_destroy_modifier(GSpecialSessionModifier * modifier) {
  free(modifier->context);
  free(modifier);
}

GSpecialSessionModifier * graphics_special_draw_create_mirror_modifier(
                                                                       bool horizontal) {
  GSpecialSessionModifier * mod = malloc(sizeof(GSpecialSessionModifier));
  mod->overrides_draw = false;
  mod->action.modifier_run = prv_run_modifier;
  mod->destroy = prv_destroy_modifier;
  mod->context = malloc(sizeof(bool));
  *((bool *) mod->context) = horizontal;
  return mod;
}