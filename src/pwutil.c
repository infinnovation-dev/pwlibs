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
 *	Utility functions
 *=======================================================================*/
#include "pwutil.h"
#include <stdlib.h>
#include <string.h>

#define PWUTIL_ERROR pwutil_error_quark()

GQuark
pwutil_error_quark(void)
{
  return g_quark_from_static_string("pwutil-error");
}

static GRegex *pwrect_rx = NULL;

static void
_compile_rx(void)
{
  pwrect_rx = g_regex_new("^(\\d+)x(\\d+)([-+]\\d+)([-+]\\d+)$", 0, 0,
			  NULL);
}

/* Parse e.g. "example.com:8765" as host and port */
gboolean
pwhostport_from_string(const gchar *hostport,
		       guint default_port,
		       gchar **host, guint *port,
		       GError **error)
{
  const char *colon = strchr(hostport, ':');
  if (colon == NULL) {
    *host = g_strdup(hostport);
    *port = default_port;
  } else {
    unsigned long ival;
    char *end;
    *host = g_strndup(hostport, colon - hostport);
    ival = strtoul(colon+1, &end, 10);
    if (colon == hostport || end == colon+1 || *end != '\0') {
      g_set_error(error, PWUTIL_ERROR,0, "Invalid host/port");
      return FALSE;
    }
    *port = ival;
  }
  return TRUE;
}

/* Convert e.g. "400x300+100+0" to PwRect or PwIntRect */
gboolean
pwintrect_from_string(PwIntRect *rect, const gchar *str, GError **error)
{
  GMatchInfo *match = NULL;

  /* Compile regex on demand */
  if (pwrect_rx == NULL) {
    _compile_rx();
  }

  if (! g_regex_match(pwrect_rx, str, 0, &match)) {
    g_set_error(error, PWUTIL_ERROR, 0, "Invalid rectangle \"%s\"", str);
    return FALSE;
  }

  rect->x0 = atoi(g_match_info_fetch(match, 3));
  rect->y0 = atoi(g_match_info_fetch(match, 4));
  rect->x1 = rect->x0 + atoi(g_match_info_fetch(match, 1));
  rect->y1 = rect->y0 + atoi(g_match_info_fetch(match, 2));

  return TRUE;
}

gboolean
pwrect_from_string(PwRect *rect, const gchar *str, GError **error)
{
  PwIntRect intrect;
  if (! pwintrect_from_string(&intrect, str, error)) {
    return FALSE;
  }
  rect->x0 = (gdouble) intrect.x0;
  rect->y0 = (gdouble) intrect.y0;
  rect->x1 = (gdouble) intrect.x1;
  rect->y1 = (gdouble) intrect.y1;
  return TRUE;
}

/* Convert e.g. "up" to PwOrient */
gboolean
pworient_from_string(PwOrient *orient, const gchar *str, GError **error)
{
  if (strcmp(str, "up") == 0) {
    *orient = PW_ORIENT_UP;
  } else if (strcmp(str, "down") == 0) {
    *orient = PW_ORIENT_DOWN;
  } else if (strcmp(str, "left") == 0) {
    *orient = PW_ORIENT_LEFT;
  } else if (strcmp(str, "right") == 0) {
    *orient = PW_ORIENT_RIGHT;
  } else {
    g_set_error(error, PWUTIL_ERROR,0, "Invalid orient \"%s\"", str);
    return FALSE;
  }
  return TRUE;
}

/* Convert e.g. "letterbox" to PwFit */
gboolean
pwfit_from_string(PwFit *fit, const gchar *str, GError **error)
{
  if (strcmp(str, "stretch") == 0) {
    *fit = PW_FIT_STRETCH;
  } else if (strcmp(str, "clip") == 0) {
    *fit = PW_FIT_CLIP;
  } else if (strcmp(str, "letterbox") == 0) {
    *fit = PW_FIT_LETTERBOX;
  } else {
    g_set_error(error, PWUTIL_ERROR,0, "Invalid fit \"%s\"", str);
    return FALSE;
  }
  return TRUE;
}
