#include "../include/notification.h"
#include "../include/utils.h"
#include <X11/Xlib.h>
#include <bits/time.h>
#include <stdlib.h>
#include <time.h>

notif_t *create_notification_window(Display *display, Window root,
									int scr_width) {
	notif_t *nt = malloc(sizeof(notif_t));
	nt->win =
		XCreateSimpleWindow(display, root, scr_width - 50, 0, 250, 50, 1,
							XBlackPixel(display, 0), XBlackPixel(display, 0));
	clock_gettime(CLOCK_MONOTONIC, &nt->created_at);

	XMapWindow(display, nt->win);
	return nt;
}

void cleanup_notification_window(Display *display, notif_t *nt) {
	if (nt != NULL && elapsed_ms(nt->created_at) > 2000) {
		XDestroyWindow(display, nt->win);
		prepare_notification_window(nt);
	}
}

void prepare_notification_window(notif_t *nt) {
	if (nt != NULL) {
		free(nt);
		nt = NULL;
	}
}
