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
 *	Abstract interface to write data stream
 *=======================================================================*/
#ifndef INC_pw_IWrite_h
#define INC_pw_IWrite_h

#include <pwinterface.h>
#include <glib.h>

typedef gboolean pw_IWrite_write(void *, const void *, gsize, GError **);
typedef gboolean pw_IWrite_flush(void *, GError **);

typedef struct {
  gsize size;
  pw_IWrite_write *write;
  pw_IWrite_flush *flush;
} pw_MIWrite;

typedef struct {
  const pw_MIWrite *methods;
  void *priv;
} pw_IWrite;

#endif /* INC_pw_IWrite_h */
