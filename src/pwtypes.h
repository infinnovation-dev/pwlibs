/*=======================================================================
 * pwlibs - Libraries used by the PiWall video wall
 * Copyright (C) 2013  Colin Hogben <colin@piwall.co.uk>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *-----------------------------------------------------------------------
 *	Types used in PiWall
 *=======================================================================*/
#ifndef INC_pwtypes_h
#define INC_pwtypes_h

#include <glib.h>

typedef struct {
  gdouble x0, y0;
  gdouble x1, y1;
} PwRect;

typedef struct {
  gint x0, y0;
  gint x1, y1;
} PwIntRect;

#define PWRECT_SET(r,_x0,_y0,_x1,_y1) \
  do {(r).x0=(_x0); (r).y0=(_y0); (r).x1=(_x1); (r).y1=(_y1);} while (0)
#define PWRECT_SET0(r,_w,_h) PWRECT_SET(r,0,0,_w,_h)

#define PWRECT_WIDTH(r) ((r).x1 - (r).x0)
#define PWRECT_HEIGHT(r) ((r).y1 - (r).y0)

#define PWRECT_EQUAL(r1,r2) ((r1).x0 == (r2).x0 && (r1).y0 == (r2).y0 && \
			     (r1).x1 == (r2).x1 && (r1).y1 == (r2).y1)

/* Test of rectangle is degenerate */
#define PWRECT_EMPTY(r) (PWRECT_WIDTH(r) == 0 || PWRECT_HEIGHT(r) == 0)

/* printf formats and arguments */
#define PWRECT_FORMAT "%gx%g%+g%+g"
#define PWINTRECT_FORMAT "%dx%d%+d%+d"
#define PWRECT_ARGS(r) PWRECT_WIDTH(r), PWRECT_HEIGHT(r), (r).x0, (r).y0

/* How to fit a picture into a window of different aspect ration */
typedef enum {
  PW_FIT_STRETCH,		/* Change aspect ratio to match */
  PW_FIT_CLIP,			/* Enlarge to fill and lose edges */
  PW_FIT_LETTERBOX		/* Expand without losing any */
} PwFit;

/* Orientation */
typedef enum {
  PW_ORIENT_UP,			/* Normal, no rotation */
  PW_ORIENT_LEFT,		/* Anticlockwise so top is now to left */
  PW_ORIENT_DOWN,		/* Upside-down */
  PW_ORIENT_RIGHT		/* Clockwise so top is now to right */
} PwOrient;

/* VideoCore tranformation with values the same as for the following:
 * MMAL_DISPLAYTRANSFORM_T (interface/mmal/mmal_parameters_video.h)
 * VC_IMAGE_TRANSFORM_T (interface/vctypes/vc_image_types.h)
 * VC_DISPMAN_TRANSFORM_T (interface/vmcs_host/vc_dispservice_defs.h)
 * OMX_DISPLAYTRANSFORMTYPE (interface/vmcs_host/khronos/IL/OMX_Broadcom.h)
 * BUFMAN_TRANSFORM_T (interface/vmcs_host/vc_vchi_bufman.h)
 */
typedef enum {
  PW_VCTRANSFORM_ROT0 = 0,
  PW_VCTRANSFORM_MIRROR_ROT0 = 1,
  PW_VCTRANSFORM_MIRROR_ROT180 = 2,
  PW_VCTRANSFORM_ROT180 = 3,
  PW_VCTRANSFORM_MIRROR_ROT90 = 4,
  PW_VCTRANSFORM_ROT270 = 5,
  PW_VCTRANSFORM_ROT90 = 6,
  PW_VCTRANSFORM_MIRROR_ROT270 = 7
} PwVcTransform;

/* Alignment etc. */
typedef enum {
  PW_JUSTIFY_LEFT,
  PW_JUSTIFY_CENTRE,
  PW_JUSTIFY_RIGHT
} PwJustify;

typedef enum {
  PW_ANCHOR_N,
  PW_ANCHOR_S,
  PW_ANCHOR_E,
  PW_ANCHOR_W,
  PW_ANCHOR_C,
  PW_ANCHOR_NW,
  PW_ANCHOR_NE,
  PW_ANCHOR_SW,
  PW_ANCHOR_SE
} PwAnchor;

#endif /* INC_pwtypes_h */
