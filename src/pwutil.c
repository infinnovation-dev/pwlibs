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
 *	Utility functions
 *=======================================================================*/
#include "pwutil.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define PWUTIL_ERROR pwutil_error_quark()

GQuark
pwutil_error_quark(void)
{
  return g_quark_from_static_string("pwutil-error");
}

static GRegex *pwintrect_rx = NULL;
static GRegex *pwrectp_rx = NULL;
static GRegex *pwpos_rx = NULL;

static void
_compile_rx(void)
{
#define FP_RX "\\d+(?:\\.\\d+)?"
  pwintrect_rx = g_regex_new("^(\\d+)x(\\d+)([-+]\\d+)([-+]\\d+)$",
			  0, 0, NULL);
  pwrectp_rx = g_regex_new("^(("FP_RX")x("FP_RX"))?"
			   "(([-+]"FP_RX")([-+]"FP_RX"))?(%)?$",
			   0, 0, NULL);
  pwpos_rx = g_regex_new("^([-+]"FP_RX")([-+]"FP_RX")(%?)$",
			 0, 0, NULL);
#undef FP_RX
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
  if (pwintrect_rx == NULL) {
    _compile_rx();
  }

  if (! g_regex_match(pwintrect_rx, str, 0, &match)) {
    g_set_error(error, PWUTIL_ERROR, 0, "Invalid rectangle \"%s\"", str);
    return FALSE;
  }

  rect->x0 = atoi(g_match_info_fetch(match, 3));
  rect->y0 = atoi(g_match_info_fetch(match, 4));
  rect->x1 = rect->x0 + atoi(g_match_info_fetch(match, 1));
  rect->y1 = rect->y0 + atoi(g_match_info_fetch(match, 2));

  g_match_info_free(match);
  return TRUE;
}

#define RECT_SIZE 0x1		/* Size needed | set */
#define RECT_POS  0x2		/* Position needed | set */
#define RECT_ABS  0x4		/* Absolute (no %) needed | set */

/*-----------------------------------------------------------------------
 *	Parse rectangle in the most lax way, then check constraints.
 *-----------------------------------------------------------------------*/
static gboolean
_pwrectp_parse(const gchar *str, guint need,
	       PwRect *rect, guint *avail, GError **error)
{
  GMatchInfo *match;
  gboolean ok = FALSE;

  /* Compile regex on demand */
  if (pwrectp_rx == NULL) {
    _compile_rx();
  }

  if (! g_regex_match(pwrectp_rx, str, 0, &match)) {
    g_set_error(error, PWUTIL_ERROR, 0, "Invalid rectangle \"%s\"", str);
    return FALSE;
  }

  *avail = 0;
  if (g_match_info_fetch(match, 4)) {
    rect->x0 = g_ascii_strtod(g_match_info_fetch(match, 5), NULL);
    rect->y0 = g_ascii_strtod(g_match_info_fetch(match, 6), NULL);
    *avail |= RECT_POS;
  } else {
    if (need & RECT_POS) {
      g_set_error(error, PWUTIL_ERROR, 0, "Position missing");
      goto fail;
    }
    rect->x0 = 0;
    rect->y0 = 0;
  }
  
  if (g_match_info_fetch(match, 1)) {
    rect->x1 = rect->x0 + g_ascii_strtod(g_match_info_fetch(match, 5), NULL);
    rect->y1 = rect->y0 + g_ascii_strtod(g_match_info_fetch(match, 6), NULL);
    *avail |= RECT_SIZE;
  } else {
    if (need & RECT_SIZE) {
      g_set_error(error, PWUTIL_ERROR, 0, "Size of rectangle missing");
      goto fail;
    }
    rect->x1 = rect->x0;
    rect->y1 = rect->y0;
  }

  if (! g_match_info_fetch(match, 7)) {
    /* Absence of "%" means absolute coordinates */
    *avail |= RECT_ABS;
  } else {
    if (need & RECT_ABS) {
      g_set_error(error, PWUTIL_ERROR, 0, "Percentage not allowed");
      goto fail;
    }
  }
  ok = TRUE;
  
 fail:
  g_match_info_free(match);
  return ok;
}
	       
gboolean
pwrect_from_string(PwRect *rect, const gchar *str, GError **error)
{
  guint avail;
  if (! _pwrectp_parse(str, RECT_SIZE | RECT_POS | RECT_ABS,
		       rect, &avail, error)) {
    return FALSE;
  }
  return TRUE;
}
	       
gboolean
pwrectp_from_string(PwRect *rect, gboolean *percent, const gchar *str, 
		    GError **error)
{
  guint avail;
  if (! _pwrectp_parse(str, RECT_SIZE,
		       rect, &avail, error)) {
    return FALSE;
  }
  *percent = ! (avail & RECT_ABS);
  return TRUE;
}

gboolean
pwpos_from_string(gdouble *x, gdouble *y, gboolean *percent, const gchar *str,
		  GError **error)
{
  PwRect rect;
  guint avail;
  if (! _pwrectp_parse(str, RECT_POS, &rect, &avail, error)) {
    return FALSE;
  }
  if (avail & RECT_SIZE) {
    g_set_error(error, PWUTIL_ERROR, 0, "Unexpected size before position");
    return FALSE;
  }
  *x = rect.x0;
  *y = rect.y0;
  *percent = ! (avail & RECT_ABS);
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

/* Convert e.g. "left" to PwJustify */
gboolean
pwjustify_from_string(PwJustify *justify, const gchar *str, GError **error)
{
  if (strcmp(str, "left") == 0) {
    *justify = PW_JUSTIFY_LEFT;
  } else if (strcmp(str, "centre") == 0 ||
	     strcmp(str, "center") == 0) { /* For our cousins */
    *justify = PW_JUSTIFY_CENTRE;
  } else if (strcmp(str, "right") == 0) {
    *justify = PW_JUSTIFY_RIGHT;
  } else {
    g_set_error(error, PWUTIL_ERROR,0, "Invalid justify \"%s\"", str);
    return FALSE;
  }
  return TRUE;
}

/* Convert e.g. "sw" to PwAnchor */
gboolean
pwanchor_from_string(PwAnchor *anchor, const gchar *str, GError **error)
{
  if (strcmp(str, "n") == 0) {
    *anchor = PW_ANCHOR_N;
  } else if (strcmp(str, "s") == 0) {
    *anchor = PW_ANCHOR_S;
  } else if (strcmp(str, "e") == 0) {
    *anchor = PW_ANCHOR_E;
  } else if (strcmp(str, "w") == 0) {
    *anchor = PW_ANCHOR_W;
  } else if (strcmp(str, "c") == 0) {
    *anchor = PW_ANCHOR_C;
  } else if (strcmp(str, "nw") == 0) {
    *anchor = PW_ANCHOR_NW;
  } else if (strcmp(str, "ne") == 0) {
    *anchor = PW_ANCHOR_NE;
  } else if (strcmp(str, "sw") == 0) {
    *anchor = PW_ANCHOR_SW;
  } else if (strcmp(str, "se") == 0) {
    *anchor = PW_ANCHOR_SE;
  } else {
    g_set_error(error, PWUTIL_ERROR,0, "Invalid anchor \"%s\"", str);
    return FALSE;
  }
  return TRUE;
}

/*-----------------------------------------------------------------------
 *	Convert e.g. "#996600" to PwRGBA
 *-----------------------------------------------------------------------*/
static gboolean
parse_hex2(guint8 *value, const gchar *str)
{
  int i;
  *value = 0;
  for (i=0; i < 2; i++) {
    unsigned char c = *str++;
    if (! isxdigit(c)) {
      return FALSE;
    }
    c = toupper(c);
    *value *= 16;
    if ('0' <= c && c <= '9') {
      *value += c - '0';
    } else if ('A' <= c && c <= 'F') {
      *value += c - 'A' + 10;
    } else {
      return FALSE;
    }
  }
  return TRUE;
}

gboolean
pwrgba_from_string(PwRGBA *rgba, const gchar *str, GError **error)
{
  if (str[0] == '#') {
    if (! parse_hex2(&rgba->r, str+1)) goto badhex;
    if (! parse_hex2(&rgba->g, str+3)) goto badhex;
    if (! parse_hex2(&rgba->b, str+5)) goto badhex;
    str += 7;
#if WANT_COLOUR_ALPHA
    if (*str == ';') {
      char *end;
      gdouble opacity = strtod(str+1, &end);
      if (end == str+1) goto badhex;
      rgba->a = CLAMP((int)(255 * (1.0 - opacity)), 0, 255);
      str = end;
    }
#else
    rgba->a = 255;
#endif
    if (*str != '\0') goto badhex;
    return TRUE;
  badhex:
    g_set_error(error, PWUTIL_ERROR,0, "Invalid hex colour");
    return FALSE;
  } else {
    g_set_error(error, PWUTIL_ERROR,0, "Unknown colour representation");
    return FALSE;
  }
}

/*-----------------------------------------------------------------------
 *	Trace object
 *-----------------------------------------------------------------------*/
struct _PwTrace {
  FILE *file;
  guint left;			/* How many more records to write */
};

/* Limit tracing to this number of calls */
#define PWTRACE_DEFAULT_COUNT	1000

/*-----------------------------------------------------------------------
 *	Open trace object
 *	Filename from ${name}_TRACEFILE environment variable
 *	Number of records from ${name}_COUNT environment variable
 *-----------------------------------------------------------------------*/
PwTrace *
pwtrace_open(const char *name)
{
  FILE *file;
  gchar *envvar;
  const gchar *filename, *svalue;
  PwTrace *self = NULL;

  if (name == NULL) {
    envvar = g_strdup("TRACEFILE");
  } else {
    envvar = g_strdup_printf("%s_TRACEFILE", name);
  }
  filename = g_getenv(envvar);
  if (filename != NULL) {
    file = fopen(filename, "a");
    if (file == NULL) {
      g_printerr("Cannot open %s", filename);
    } else {
      guint stublen = strlen(envvar) - 9;
      guint count;
      gchar *buffer = NULL;

      /* Replace TRACEFILE with BUFSIZE */
      strcpy(envvar + stublen, "BUFSIZE");
      svalue = g_getenv(envvar);
      if (svalue != NULL) {
	guint bufsize = atoi(svalue);
	buffer = g_malloc(bufsize);
	setvbuf(file, buffer, _IOFBF, bufsize);
      }
	
      /* Replace TRACEFILE with COUNT */
      strcpy(envvar + stublen, "COUNT");
      svalue = g_getenv(envvar);
      if (svalue == NULL) {
	count = PWTRACE_DEFAULT_COUNT;
      } else {
	count = atoi(svalue);
      }

      self = g_new(PwTrace, 1);
      self->file = file;
      self->left = count;
    }
  }
  g_free(envvar);
  return self;
}

void
pwtrace_close(PwTrace *self)
{
  if (self == NULL || self->file == NULL) return;
  fclose(self->file);
  self->file = NULL;
}

/*-----------------------------------------------------------------------
 *	Formatted trace output
 *-----------------------------------------------------------------------*/
void
pwtracef(PwTrace *self, const char *fmt, ...)
{
  va_list ap;
  struct timespec now;
  guint count, i;
  guchar c;

  if (self == NULL || self->file == NULL) return;

  clock_gettime(CLOCK_MONOTONIC_RAW, &now);
  fprintf(self->file, "%lu.%06ld",
	  now.tv_sec, now.tv_nsec / 1000);

  va_start(ap, fmt);
  while (1) {
    c = *fmt++;
    if (c == '\0') break;
    /* Optional repeat count */
    if (isdigit(c)) {
      count = 0;
      do {
	count = 10 * count + c-'0';
	c = *fmt++;
	if (c == '\0') break;
      } while (isdigit(c));
    } else if (c == '*') {
      count = va_arg(ap, unsigned int);
      c = *fmt++;
      if (c == '\0') break;
    } else {
      count = 1;
    }
    if (c == 'i') {
      for (i=0; i < count; i++) {
	int ival = va_arg(ap, int);
	fprintf(self->file, " %d", ival);
      }
    } else if (c == 'u') {
      for (i=0; i < count; i++) {
	unsigned int ival = va_arg(ap, unsigned int);
	fprintf(self->file, " %u", ival);
      }
    } else if (c == 'x') {
      for (i=0; i < count; i++) {
	unsigned int ival = va_arg(ap, unsigned int);
	fprintf(self->file, " %x", ival);
      }
    } else if (c == 's') {
      for (i=0; i < count; i++) {
	const char *str = va_arg(ap, const char *);
	fprintf(self->file, " %s", str);
      }
    } else if (c == 'b') {
      const guchar *bytes = va_arg(ap, const guchar *);
      for (i=0; i < count; i++) {
	guchar b = *bytes++;
	fprintf(self->file, " %02x", b);
      }
    } else {
      fprintf(self->file, "?");
      break;
    }
  }
  va_end(ap);
  fprintf(self->file, "\n");

  /* Close if requested count reached */
  if (-- self->left == 0) {
    fclose(self->file);
    self->file = NULL;
  }
}

