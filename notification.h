#ifndef NWM_NOTIF_H
#define NWM_NOTIF_H

#include <X11/Xlib.h>
#include <time.h>

typedef struct {
	Window win;
	struct timespec created_at;
} notif_t;

notif_t *create_notification_window(Display *display, Window root,
									int scr_width);
void cleanup_notification_window(Display *display, notif_t *nt);
void prepare_notification_window(notif_t *nt);

#endif
