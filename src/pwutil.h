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
 *	Utility functions.  No external dependencies except glib.
 *=======================================================================*/
#ifndef INC_pwutil_h
#define INC_pwutil_h

#include "pwtypes.h"

/* Parse e.g. "example.com:8765" as host and port */
extern gboolean pwhostport_from_string(const gchar */*hostport*/,
				       guint /*default_port*/,
				       gchar **/*host*/, guint */*port*/,
				       GError **);

/* Convert e.g. "400x300+100+0" to PwRect or PwIntRect */
extern gboolean pwrect_from_string(PwRect *, const gchar *, GError **);
extern gboolean pwintrect_from_string(PwIntRect *, const gchar *, GError **);

/* Convert e.g. "up" to PwOrient */
extern gboolean pworient_from_string(PwOrient *, const gchar *, GError **);

/* Convert e.g. "letterbox" to PwFit */
extern gboolean pwfit_from_string(PwFit *, const gchar *, GError **);

#endif /* INC_pwutil_h */
