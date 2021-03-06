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
#include "pwutil.h"
#include <stdlib.h>
#include <string.h>

struct _PwDefs {
  gint nrefs;
  gsize nfiles;
  GKeyFile **files;
};

#if 0
static GQuark
pwdefs_error_quark(void)
{
  return g_quark_from_static_string("pwdefs-error");
}
#endif

/*-----------------------------------------------------------------------
 *	Load definitions from .pitile and .piwall files
 *-----------------------------------------------------------------------*/
PwDefs *
pwdefs_create_tile(GError **error)
{
  PwDefs *self;
  const gchar *home;
  gchar *files[2];
  /* glib < 2.36 only looks in passwd database */
  home = g_getenv("HOME");
  if (home == NULL) {
    home = g_get_home_dir();
  }
  files[0] = g_build_filename(home, ".pitile", NULL);
  files[1] = g_build_filename(home, ".piwall", NULL);
  self = pwdefs_create(2, (const gchar **)files, error);
  g_free(files[0]);
  g_free(files[1]);
  return self;
}

PwDefs *
pwdefs_create(gsize nfiles, const gchar * const filenames[], GError **error)
{
  PwDefs *self = g_new0(PwDefs, 1);
  GKeyFile *kf;
  int i;

  self->nrefs = 1;
  self->files = g_new0(GKeyFile *, nfiles);
  for (i=0; i < nfiles; i++) {
    kf = g_key_file_new();
    if (! g_key_file_load_from_file(kf,
				    filenames[i], G_KEY_FILE_NONE,
				    error)) {
      if (g_error_matches(*error,
			  G_KEY_FILE_ERROR, G_KEY_FILE_ERROR_NOT_FOUND) ||
	  g_error_matches(*error,
			  G_FILE_ERROR, G_FILE_ERROR_NOENT)) {
	/* Allow file to be absent */
	g_clear_error(error);
	g_key_file_free(kf);
	kf = NULL;
      } else {
	g_prefix_error(error, "loading %s: ", filenames[i]);
	goto fail;
      }
    } else {
      self->files[self->nfiles ++] = kf;
    }
  }

  return self;

 fail:
  g_key_file_free(kf);
  pwdefs_free(self);
  return NULL;
}

/*-----------------------------------------------------------------------
 *	Deallocation
 *-----------------------------------------------------------------------*/
void
pwdefs_ref(PwDefs *self)
{
  ++ self->nrefs;
}

void
pwdefs_unref(PwDefs *self)
{
  if (-- self->nrefs <= 0) pwdefs_free(self);
}

void
pwdefs_free(PwDefs *self)
{
  int i;
  for (i=0; i < self->nfiles; i++) {
    g_key_file_free(self->files[i]);
  }
  g_free(self->files);
  g_free(self);
}

/*-----------------------------------------------------------------------
 *	Check if section exists
 *-----------------------------------------------------------------------*/
gboolean
pwdefs_has_section(PwDefs *self, const gchar *section)
{
  int i;
  for (i=0; i < self->nfiles; i++) {
    if (g_key_file_has_group(self->files[i], section)) {
      return TRUE;
    }
  }

  return FALSE;
}

/*-----------------------------------------------------------------------
 *	Fetch string value from named section and key
 *-----------------------------------------------------------------------*/
gchar *
pwdefs_string(PwDefs *self,
	      const gchar *section, const gchar *key,
	      GError **error)
{
  gchar *string = NULL;
  if (self->nfiles == 0) {
    g_set_error(error, G_KEY_FILE_ERROR, G_KEY_FILE_ERROR_NOT_FOUND,
		"No definition file found");
  } else {
    int i;
    for (i=0; i < self->nfiles; i++) {
      g_clear_error(error);
      string = g_key_file_get_string(self->files[i], section, key, error);
      if (string != NULL) {
	break;
      }
    }
  }
  return string;
}

/*-----------------------------------------------------------------------
 *	Fetch int value from named section and key
 *-----------------------------------------------------------------------*/
gint
pwdefs_int(PwDefs *self,
	   const gchar *section, const gchar *key,
	   GError **error)
{
  gint result = 0;
  gchar *value = pwdefs_string(self, section, key, error);
  if (value != NULL) {
    gchar *end;
    result = strtol(value, &end, 0);
    if (end == value || *end != '\0') {
      g_set_error(error, G_KEY_FILE_ERROR, G_KEY_FILE_ERROR_INVALID_VALUE,
		  "Invalid integer for key %s in group %s",
		  key, section);
    }
    g_free(value);
  }
  return result;
}

/*-----------------------------------------------------------------------
 *	Fetch double value from named section and key
 *-----------------------------------------------------------------------*/
gdouble
pwdefs_double(PwDefs *self,
	      const gchar *section, const gchar *key,
	      GError **error)
{
  gdouble result = 0;
  gchar *value = pwdefs_string(self, section, key, error);
  if (value != NULL) {
    gchar *end;
    result = g_ascii_strtod(value, &end);
    if (end == value || *end != '\0') {
      g_set_error(error, G_KEY_FILE_ERROR, G_KEY_FILE_ERROR_INVALID_VALUE,
		  "Invalid real number for key %s in group %s",
		  key, section);
    }
    g_free(value);
  }
  return result;
}

/*-----------------------------------------------------------------------
 *	List the keys in a section
 *-----------------------------------------------------------------------*/
gchar **
pwdefs_keys(PwDefs *self, const gchar *section, gsize *length)
{
  gsize nalloc = 0;
  gsize nused = 0;
  gchar **keys = NULL;
  int i;
  for (i=0; i < self->nfiles; i++) {
    gsize nfkeys;
    gchar **fkeys = g_key_file_get_keys(self->files[i], section,
					&nfkeys, NULL);
    if (fkeys) {
      if (keys == NULL) {
	/* None previously, so just use these */
	keys = fkeys;
	nused = nfkeys;
	nalloc = nfkeys + 1;
      } else {
	/* Need to merge with existing */
	int j, k;
	for (j=0; j < nfkeys; j++) {
	  gboolean found = FALSE;
	  for (k=0; k < nused; k++) {
	    if (strcmp(fkeys[j], keys[k]) == 0) {
	      found = TRUE;
	      break;
	    }
	  }
	  if (! found) {
	    /* Need to add to list */
	    if (nused == nalloc-1) {
	      /* ... which needs enlarging */
	      nalloc = MAX(nalloc * 2, 20);
	      keys = g_renew(gchar *, keys, nalloc);
	    }
	    keys[nused ++] = g_strdup(fkeys[j]);
	  }
	}
	g_strfreev(fkeys);
      }
    }
  }
  if (keys) {
    keys[nused] = NULL;
    if (length) *length = nused;
  }
  return keys;
}
