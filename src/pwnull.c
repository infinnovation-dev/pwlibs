/*=======================================================================
 *	Null interfaces
 *=======================================================================*/
#include <pwutil.h>

/*-----------------------------------------------------------------------
 *	pw_IPaint
 *-----------------------------------------------------------------------*/
static gboolean
pwnull_paint_row(void *self, guint x, guint y, gsize count, const void *pixels,
		 GError **error)
{ return TRUE; }

static gboolean
pwnull_fill_rect(void *self, guint x, guint y, guint width, guint height,
		 guint pixval,
		 GError **error)
{ return TRUE; }

static gboolean
pwnull_copy_rect(void *self, guint x, guint y, guint width, guint height, 
		 guint srcx,guint srcy,
		 GError **error)
{ return TRUE; }

static gboolean
pwnull_update_done(void *self, GError **error)
{ return TRUE; }

static const pw_MIPaint pwnull_mipaint = {
  sizeof(pwnull_mipaint),
  &pwnull_paint_row,
  &pwnull_fill_rect,
  &pwnull_copy_rect,
  &pwnull_update_done,
};

const pw_IPaint pwnull_ipaint = {
  &pwnull_mipaint,
  NULL,
};
