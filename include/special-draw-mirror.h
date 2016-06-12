//
//  special-draw-mirror.h
//  Special Draw
//
//  Created by Christian Reinbacher on 12.06.16.
//  Copyright © 2016 Christian Reinbacher. All rights reserved.
//

#ifndef special_draw_mirror_h
#define special_draw_mirror_h
#include <pebble.h>
#include "special-draw.h"

GSpecialSessionModifier * graphics_special_draw_create_mirror_modifier(
                                                                       bool horizontal);

#endif /* special_draw_mirror_h */
