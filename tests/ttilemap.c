#include <stdio.h>
#include <glib.h>
#include <pwutil.h>
#include <pwtilemap.h>

int
main(int argc, char *argv[])
{
  PwIntRect screen, picture;
  PwTileMap *tilemap;
  GOptionContext *context;
  GError *error = NULL;

  tilemap = pwtilemap_create();

  context = g_option_context_new("SCREEN PICTURE - check tile mapping");
  pwtilemap_add_options(tilemap, context);
  g_option_context_parse(context, &argc, &argv, &error);
  if (! error) {
    if (argc != 3) {
      g_set_error(&error, G_OPTION_ERROR, 0, "Wrong number of arguments %d",
		  argc);
    } else {
      pwintrect_from_string(&screen, argv[1], &error);
      if (! error) {
	pwintrect_from_string(&picture, argv[2], &error);
      }
    }
  }
  if (! error) {
    pwtilemap_set_screen(tilemap, &screen);
  }
  if (! error) {
    pwtilemap_define(tilemap, &error);
  }
  if (! error) {
    PwIntRect src, dest;
    PwVcTransform xform;
    pwtilemap_map_picture(tilemap, &picture,
			  &src, &dest, &xform,
			  &error);
    if (! error) {
      printf("src: %dx%d+%d+%d\n",
	     PWRECT_WIDTH(src), PWRECT_HEIGHT(src), src.x0, src.y0);
      printf("dest: %dx%d+%d+%d\n",
	     PWRECT_WIDTH(dest), PWRECT_HEIGHT(dest), dest.x0, dest.y0);
      printf("transform: %d\n", (int)xform);
    }
  }

  if (error) {
    fprintf(stderr, "%s\n", error->message);
    return 1;
  }
  return 0;
}
