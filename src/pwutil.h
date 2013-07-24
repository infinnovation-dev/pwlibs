/*=======================================================================
 *	Utility functions.  No external dependencies except glib.
 *=======================================================================*/
#ifndef INC_pwutil_h
#define INC_pwutil_h

#include "pwtypes.h"

/* Parse e.g. "example.com:8765" as host and port */
extern gboolean pwhostport_from_string(const gchar */*hostport*/,
				       guint /*default_port*/,
				       gchar **/*host*/, guint */*port*/,
				       GError **);

/* Convert e.g. "400x300+100+0" to PwRect or PwIntRect */
extern gboolean pwrect_from_string(PwRect *, const gchar *, GError **);
extern gboolean pwintrect_from_string(PwIntRect *, const gchar *, GError **);

/* Convert e.g. "up" to PwOrient */
extern gboolean pworient_from_string(PwOrient *, const gchar *, GError **);

/* Convert e.g. "letterbox" to PwFit */
extern gboolean pwfit_from_string(PwFit *, const gchar *, GError **);

#endif /* INC_pwutil_h */
