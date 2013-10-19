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
 *	Encapsulate configuration from .piwall and .pitile
 *=======================================================================*/
#ifndef INC_pwdefs_h
#define INC_pwdefs_h

typedef struct _PwDefs PwDefs;

/* Load definitions from .piwall and .pitile files */
extern PwDefs *pwdefs_create_tile(GError **);

/* Load definitions from a set of files */
extern PwDefs *pwdefs_create(gsize /*nfiles*/, const gchar **/*files*/,
			     GError **);

/* Fetch string value from named section and key */
extern gchar *pwdefs_string(PwDefs *,
			    const gchar */*section*/, const gchar */*key*/,
			    GError **);

/* Fetch int value from named section and key */
extern gint pwdefs_int(PwDefs *,
		       const gchar */*section*/, const gchar */*key*/,
		       GError **);

/* Fetch double value from named section and key */
extern gdouble pwdefs_double(PwDefs *,
			     const gchar */*section*/, const gchar */*key*/,
			     GError **);

extern void pwdefs_free(PwDefs *);

#endif /* INC_pwdefs_h */
