#include "client.h"
#include <X11/X.h>
#include <X11/Xlib.h>
#include <stdlib.h>

void add_window_to_list(client_t **client_head, client_t *c) {
	c->next = *client_head;
	*client_head = c;
}

XWindowAttributes get_window_attr(Display *display, Window *win) {
	XWindowAttributes xc;
	XGetWindowAttributes(display, *win, &xc);
	return xc;
}

void on_window_focus(Display *display, client_t *focused_client,
					 bool move_window, bool resize_window) {
	if (focused_client != NULL) {
		XSetInputFocus(display, focused_client->win, RevertToPointerRoot,
					   CurrentTime);
		XRaiseWindow(display, focused_client->frame);
		XMoveWindow(display, focused_client->frame, focused_client->x,
					focused_client->y);

		XResizeWindow(display, focused_client->frame, focused_client->width,
					  focused_client->height);

		XResizeWindow(display, focused_client->win, focused_client->width,
					  focused_client->height);

		if (move_window) {
			XSetWindowBorder(display, focused_client->frame, COLOR_RED);
		} else if (resize_window) {
			XSetWindowBorder(display, focused_client->frame, COLOR_GREEN);
		} else {
			XSetWindowBorder(display, focused_client->frame, COLOR_BLUE);
		}
	}
}

void map_window(Display *display, Window *root, client_t **client_head,
				client_t **focused_client, XMapRequestEvent *mr) {
	XWindowAttributes win_attr = get_window_attr(display, &mr->window);

	if (win_attr.override_redirect)
		return;

	client_t *c = malloc(sizeof(client_t));
	c->win = mr->window;
	c->x = win_attr.x;
	c->y = win_attr.y;
	c->width = win_attr.width;
	c->height = win_attr.height;

	c->frame =
		XCreateSimpleWindow(display, *root, c->x, c->y, c->width + 2, c->height,
							2, WhitePixel(display, 0), BlackPixel(display, 0));
	add_window_to_list(client_head, c);
	XSelectInput(display, c->frame,
				 EnterWindowMask | LeaveWindowMask | ButtonPressMask |
					 ButtonReleaseMask);

	XReparentWindow(display, c->win, c->frame, 0, 0);
	XMapWindow(display, c->frame);
	XMapWindow(display, c->win);
	*focused_client = c;
}
