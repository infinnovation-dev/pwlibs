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
 *	Abstract interface for displaying data
 *=======================================================================*/
#ifndef INC_pw_IPaint_h
#define INC_pw_IPaint_h

#include "pwinterface.h"
#include <glib.h>

typedef gboolean pw_IPaint_paint_row(void *, guint /*x*/,guint /*y*/,
				     gsize /*count*/, const void */*pixels*/,
				     GError **);
typedef gboolean pw_IPaint_fill_rect(void *, guint /*x*/,guint /*y*/,
				     guint /*width*/,guint /*height*/,
				     guint /*pixel*/,
				     GError **);
typedef gboolean pw_IPaint_copy_rect(void *, guint /*x*/,guint /*y*/,
				     guint /*width*/,guint /*height*/,
				     guint /*srcx*/,guint /*srcy*/,
				     GError **);
typedef gboolean pw_IPaint_update_done(void *, GError **);

typedef struct {
  gsize size;
  pw_IPaint_paint_row *paint_row;
  pw_IPaint_fill_rect *fill_rect;
  pw_IPaint_copy_rect *copy_rect;
  pw_IPaint_update_done *update_done;
} pw_MIPaint;

typedef struct {
  const pw_MIPaint *methods;
  void *priv;				/* Private data */
} pw_IPaint;

#endif /* INC_pw_IPaint_h */
