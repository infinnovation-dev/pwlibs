/*=======================================================================
 * pwlibs - Libraries used by the PiWall video wall
 * Copyright (C) 2013-2014  Colin Hogben <colin@piwall.co.uk>
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

#include <pwtypes.h>
#include <pw_IPaint.h>
#include <time.h>		/* struct timespec */

/* Parse e.g. "example.com:8765" as host and port */
extern gboolean pwhostport_from_string(const gchar */*hostport*/,
				       guint /*default_port*/,
				       gchar **/*host*/, guint */*port*/,
				       GError **);

/* Convert e.g. "400x300+100+0" to PwRect or PwIntRect */
extern gboolean pwrect_from_string(PwRect *, const gchar *, GError **);
extern gboolean pwintrect_from_string(PwIntRect *, const gchar *, GError **);
/* Similar, but allow percentage */
extern gboolean pwrectp_from_string(PwRect *, gboolean */*percent*/,
				    const gchar *str, GError **error);

/* Convert e.g. "+0+100" or "-12.5+70%" to position */
extern gboolean pwpos_from_string(gdouble *, gdouble *, gboolean */*percent*/,
				  const gchar *, GError **error);

/* Convert e.g. "up" to PwOrient */
extern gboolean pworient_from_string(PwOrient *, const gchar *, GError **);

/* Convert e.g. "letterbox" to PwFit */
extern gboolean pwfit_from_string(PwFit *, const gchar *, GError **);

/* Convert e.g. "left" to PwJustify */
extern gboolean pwjustify_from_string(PwJustify *, const gchar *, GError **);

/* Convert e.g. "sw" to PwAnchor */
extern gboolean pwanchor_from_string(PwAnchor *, const gchar *, GError **);

/* Convert e.g. "#996631" to colour */
extern gboolean pwrgba_from_string(PwRGBA *, const gchar *, GError **);

/* Trace into a file, controlled by environment variable(s) */
typedef struct _PwTrace PwTrace;

extern PwTrace *pwtrace_open(const char */*name*/);
extern void pwtracef(PwTrace *, const char */*format*/, ...);
extern void pwtrace_close(PwTrace *);

/*-----------------------------------------------------------------------
 *	Definitions from INI-style files
 *-----------------------------------------------------------------------*/
typedef struct _PwDefs PwDefs;

/* Load definitions from .piwall and .pitile files */
extern PwDefs *pwdefs_create_tile(GError **);

/* Load definitions from a set of files */
extern PwDefs *pwdefs_create(gsize /*nfiles*/,
			     const gchar *const[]/*filenames*/,
			     GError **);

extern void pwdefs_ref(PwDefs *);
extern void pwdefs_unref(PwDefs *);
extern void pwdefs_free(PwDefs *);

/* Check if section exists */
extern gboolean pwdefs_has_section(PwDefs *, const gchar */*section*/);

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

/* Get list of keys in a section */
extern gchar **pwdefs_keys(PwDefs *, const gchar */*section*/,
			   gsize */*length*/);

/*-----------------------------------------------------------------------
 *	Logging support
 *-----------------------------------------------------------------------*/
extern void
pwglog_to_syslog(void);

extern void
pwglog_set_level(GLogLevelFlags);

extern void
pwglog_handler(const gchar */*domain*/, GLogLevelFlags /*level*/,
	       const gchar */*message*/, gpointer /*userdata*/);

/*-----------------------------------------------------------------------
 *	Network throttling
 *-----------------------------------------------------------------------*/
typedef struct _PwThrottle PwThrottle;

extern PwThrottle *pwthrottle_create(double /*buffer_size*/, double /*rate*/);
extern int pwthrottle_check(PwThrottle *, size_t /*nbytes*/,
			    struct timespec */*wait*/);

/*-----------------------------------------------------------------------
 *	Null interface implementations
 *-----------------------------------------------------------------------*/
extern const pw_IPaint pwnull_ipaint;

#endif /* INC_pwutil_h */
