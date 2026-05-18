#ifndef NWM_CLIENT_H
#define NWM_CLIENT_H

#include <X11/X.h>
#include <X11/Xlib.h>
#include <stdbool.h>

#define COLOR_BLUE 0x5E85BF // somewhat blue afterall
#define COLOR_RED 0xC91400
#define COLOR_GREEN 0x008C0C

typedef struct client_t {
	Window win;
	Window frame;
	int x, y;
	int width, height;
	struct client_t *next;
} client_t;

XWindowAttributes get_window_attr(Display *display, Window *win);
void add_window_to_list(client_t **client_head, client_t *c);
void on_window_focus(Display *display, client_t *client_focused,
					 bool move_window, bool resize_window);
void map_window(Display *display, Window *root, client_t **client_head,
				client_t **focused_client, XMapRequestEvent *mr);

#endif
