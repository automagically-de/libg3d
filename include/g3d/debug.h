#ifndef _G3D_DEBUG_H
#define _G3D_DEBUG_H


#include <glib.h>
/* g_debug() is defined since glib 2.6 */

/*< private >*/
#ifndef g_debug
#define g_debug(...) g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, __VA_ARGS__)
#endif
/*< public >*/

#if DEBUG > 0
#include <string.h>
static const gchar *debug_padding_str = "                                    ";
static inline const gchar *debug_pad(guint32 level) {
	return debug_padding_str + (strlen(debug_padding_str) - level);
}
#endif

#endif /* _G3D_DEBUG_H */
