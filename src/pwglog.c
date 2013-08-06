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
 *	Sensible log output for glib
 *=======================================================================*/
#include "pwglog.h"
#include <stdio.h>
#include <string.h>
#include <syslog.h>

#ifdef __GNUC__
#  define UNUSED(x) UNUSED_ ## x __attribute__((__unused__))
#else
#  define UNUSED(x) UNUSED_ ## x
#endif

#define LEVELS_UP_TO(l) (((l) | ((l) - 1)) & G_LOG_LEVEL_MASK)

typedef enum {
  DEST_STDERR,
  DEST_SYSLOG
} PwGLogDest;

typedef struct {
  PwGLogDest dest;
  GLogLevelFlags levels;	/* Which levels enabled */
} PwGLog;

/* Select levels for selected domains */
typedef struct _PwGLogConfig {
  gchar *domain;
  GLogLevelFlags levels;
  struct _PwGLogConfig *next;
} PwGLogConfig;

static PwGLog _pwglog = {
  DEST_STDERR,
  LEVELS_UP_TO(G_LOG_LEVEL_WARNING),
};

static gboolean _syslog_opened = FALSE;
static gboolean _inited = FALSE;
static PwGLogConfig *_pwglog_configs = NULL;

/* Direct all messages to syslog */
void
pwglog_to_syslog(void)
{
  _pwglog.dest = DEST_SYSLOG;
}

/* Set default logging level */
void
pwglog_set_level(GLogLevelFlags level)
{
  _pwglog.levels = LEVELS_UP_TO(level);
}

/*-----------------------------------------------------------------------
 *	Lookup levels by domain
 *-----------------------------------------------------------------------*/
static void
_pwglog_add_config(const gchar *domain, GLogLevelFlags levels)
{
  PwGLogConfig *conf = g_new0(PwGLogConfig, 1);
  conf->domain = g_strdup(domain);
  conf->levels = levels;
  conf->next = _pwglog_configs;
  _pwglog_configs = conf;
}

static PwGLogConfig *
_pwglog_find_config(const gchar *domain)
{
  PwGLogConfig *conf;
  for (conf=_pwglog_configs; conf; conf=conf->next) {
    if (strcmp(conf->domain, domain) == 0) {
      return conf;
    }
  }
  return NULL;
}

static void
_pwglog_init(void)
{
  /* Populate configs from G_DEBUG environment variable */
  const char *debug = g_getenv("G_DEBUG");
  if (debug != NULL) {
    gchar **tokens = g_strsplit_set(debug, " ,", -1);
    int i;
    gchar *token;
    for (i=0; (token=tokens[i]) != NULL; i++) {
      _pwglog_add_config(token, LEVELS_UP_TO(G_LOG_LEVEL_DEBUG));
    }
  }
  _inited = TRUE;
}

static GLogLevelFlags
_pwglog_domain_levels(const gchar *domain)
{
  if (! _inited) {
    _pwglog_init();
  }
  if (domain != NULL && domain[0] != '\0') {
    PwGLogConfig *conf = _pwglog_find_config(domain);
    if (conf != NULL) {
      return conf->levels;
    }
  }
  return _pwglog.levels;
}

/*-----------------------------------------------------------------------
 *	Handler to be installed
 *-----------------------------------------------------------------------*/
void
pwglog_handler(const gchar *domain, GLogLevelFlags level, const gchar *message,
	       gpointer UNUSED(userdata))
{
  PwGLog *self = &_pwglog;
  GLogLevelFlags levels = _pwglog_domain_levels(domain);
  GLogLevelFlags enabled = (GLogLevelFlags)(levels & level);

  if (enabled) {
    /* Output enabled for this level */
    const char *level_text = "?";
    int prio = LOG_INFO;
    if (enabled & G_LOG_LEVEL_ERROR) {
      level_text = "ERROR";
      prio = LOG_ERR;
    } else if (enabled & G_LOG_LEVEL_CRITICAL) {
      level_text = "CRITICAL";
      prio = LOG_CRIT;
    } else if (enabled & G_LOG_LEVEL_WARNING) {
      level_text = "WARNING";
      prio = LOG_WARNING;
    } else if (enabled & G_LOG_LEVEL_MESSAGE) {
      level_text = "MESSAGE";
      prio = LOG_NOTICE;
    } else if (enabled & G_LOG_LEVEL_INFO) {
      level_text = "INFO";
      prio = LOG_INFO;
    } else if (enabled & G_LOG_LEVEL_DEBUG) {
      level_text = "DEBUG";
      prio = LOG_DEBUG;
    }

    switch (self->dest) {
    case DEST_STDERR:
      if (domain != NULL) {
	fprintf(stderr, "[%s] %s: %s\n", level_text, domain, message);
      } else {
	fprintf(stderr, "[%s] %s\n", level_text, message);
      }
      break;
    case DEST_SYSLOG:
      /* Check first that logging facility is initialized */
      if (! _syslog_opened) {
	const char *ident = g_get_prgname();
	openlog(ident, LOG_PID, LOG_USER);
	_syslog_opened = TRUE;
      }
      if (domain != NULL) {
	syslog(prio, "[%s] %s: %s", level_text, domain, message);
      } else {
	syslog(prio, "[%s] %s", level_text, message);
      }
      break;
    }
  }
}
