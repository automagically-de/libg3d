#ifndef _G3D_DEBUG_H
#define _G3D_DEBUG_H

#include <glib.h>
/* g_debug() is defined since glib 2.6 */
#ifndef g_debug
#define g_debug(...) g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, __VA_ARGS__)
#endif

#endif /* _G3D_DEBUG_H */
