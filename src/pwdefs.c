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
#include "pwdefs.h"

struct _PwDefs {
  gsize nkeyfiles;
  GKeyFile **keyfiles;
};

static GQuark
pwdefs_error_quark(void)
{
  return g_quark_from_static_string("pwdefs-error");
}

/*-----------------------------------------------------------------------
 *	Load definitions from .pitile and .piwall files
 *-----------------------------------------------------------------------*/
PwDefs *
pwdefs_create_tile(GError **error)
{
  PwDefs *self;
  const gchar *home = g_get_home_dir();
  gchar *files[2];
  files[0] = g_build_filename(home, ".pitile", NULL);
  files[1] = g_build_filename(home, ".piwall", NULL);
  self = pwdefs_create(2, files, error);
  g_free(files[0]);
  g_free(files[1]);
}

PwDefs *
pwdefs_create(gsize nfiles, const gchar **files, GError **error)
{
  PwDefs *self = g_new0(PwDefs, 1);
  GKeyFile *kf;
  int i;

  self->files = g_new0(GKeyFile *, nfiles);
  for (i=0; i < nfiles; i++) {
    kf = g_key_file_new();
    if (! g_key_file_load_from_file(self->pitile, files[i], G_KEY_FILE_NONE,
				    error)) {
      if (g_error_matches(*error, G_KEY_FILE_ERROR, G_KEY_FILE_NOT_FOUND)) {
	/* Allow file to be absent */
	g_clear_error(error);
	g_key_file_free(kf);
	kf = NULL
      } else {
	g_prefix_error(error, "loading %s: ", files[i]);
	goto fail;
      }
    } else {
      self->keyfiles[self->nkeyfiles ++] = kf;
    }
  }

  return self;

 fail:
  g_key_file_free(kf);
  pwdefs_free(self);
  return NULL;
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
  if (self->nkeyfiles == 0) {
    g_set_error(error, G_KEY_FILE_ERROR, G_KEY_FILE_ERROR_NOT_FOUND,
		"No definition file found");
  } else {
    int i;
    for (i=0; i < self->nkeyfiles; i++) {
      g_clear_error(error);
      string = g_key_file_get_string(self->pitile, section, key, error);
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
pwdefs_double(PwDefs *self,
	      const gchar *section, const gchar *key,
	      GError **error)
{
  gint result = 0;
  gchar *value = pwdefs_string(self, section, key, error);
  if (value != NULL) {
    gchar *end;
    result = g_ascii_strtod(value, &end);
    if (end == value || *end != '\0') {
      g_set_error(error, G_KEY_FILE_ERROR, G_KEY_FILE_ERROR_INVALID_VALUE,
		  "Invalid real number for key %s in group %s",
		  key, group);
    }
    g_free(value);
  }
  return result;
}

/* Fetch double value from named section and key */
gdouble
pwdefs_double(PwDefs *,
	      const gchar */*section*/, const gchar */*key*/,
	      GError **);

void
pwdefs_free(PwDefs *self)
{
  int i;
  for (i=0; i < self->nkeyfiles; i++) {
    g_key_file_free(self->keyfiles[i]);
  }
  g_free(self->keyfiles);
  g_free(self);
}

#endif /* INC_pwdefs_h */
