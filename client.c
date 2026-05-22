#include "client.h"
#include <X11/X.h>
#include <X11/Xlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

client_t *find_client_using_window(client_t **client_head, Window win) {
	for (client_t *it = *client_head; it != NULL; it = it->next) {
		if (it->win == win)
			return it;
	}

	return NULL;
}

void add_window_to_list(client_t **client_head, client_t *c) {
	c->next = *client_head;
	*client_head = c;
}

void remove_window_from_list(client_t **client_head, client_t *c) {
	client_t *prev = NULL;
	client_t *cur = *client_head;

	while (cur != NULL && cur != c) {
		prev = cur;
		cur = cur->next;
	}

	if (cur == NULL)
		return;

	if (prev == NULL) {
		*client_head = cur->next;
	} else {
		prev->next = cur->next;
	}

	free(cur);
}

XWindowAttributes get_window_attr(Display *display, Window *win) {
	XWindowAttributes xc;
	XGetWindowAttributes(display, *win, &xc);
	return xc;
}

void on_window_focus(Display *display, client_t **client_head,
					 client_t *focused_client, bool move_window,
					 bool resize_window) {
	if (focused_client != NULL) {
		XSetInputFocus(display, focused_client->win, RevertToPointerRoot,
					   CurrentTime);
		XRaiseWindow(display, focused_client->frame);
		XMoveWindow(display, focused_client->frame, focused_client->x,
					focused_client->y);

		XResizeWindow(display, focused_client->win, focused_client->width,
					  focused_client->height);

		XResizeWindow(display, focused_client->frame, focused_client->width,
					  focused_client->height);

		if (move_window) {
			XSetWindowBorder(display, focused_client->frame, COLOR_RED);
		} else if (resize_window) {
			XSetWindowBorder(display, focused_client->frame, COLOR_GREEN);
		} else {
			XSetWindowBorder(display, focused_client->frame, COLOR_BLUE);
		}
	}

	for (client_t *it = *client_head; it != NULL; it = it->next) {
		if (focused_client == NULL || it->frame != focused_client->frame) {
			XSetWindowBorder(display, it->frame, WhitePixel(display, 0));
		}
	}
}

void map_window(Display *display, Window *root, client_t **client_head,
				client_t **focused_client, XMapRequestEvent *mr) {
	XWindowAttributes win_attr = get_window_attr(display, &mr->window);

	if (win_attr.override_redirect) {
		printf("override redirect!\n");
		return;
	}

	client_t *c = malloc(sizeof(client_t));
	c->win = mr->window;
	c->x = win_attr.x;
	c->y = win_attr.y;
	c->width = win_attr.width;
	c->height = win_attr.height;
	c->is_fullscreen = false;
	c->prev_width = c->width;
	c->prev_height = c->height;

	XSelectInput(display, c->win,
				 EnterWindowMask | LeaveWindowMask | PropertyChangeMask |
					 ExposureMask | StructureNotifyMask);

	XAddToSaveSet(display, c->win);
	c->frame =
		XCreateSimpleWindow(display, *root, c->x, c->y, c->width + 2, c->height,
							2, WhitePixel(display, 0), BlackPixel(display, 0));
	add_window_to_list(client_head, c);
	XSelectInput(display, c->frame,
				 EnterWindowMask | LeaveWindowMask | ButtonPressMask |
					 ButtonReleaseMask);

	XReparentWindow(display, c->win, c->frame, 0, 0);
	XMapWindow(display, c->win);
	XMapWindow(display, c->frame);
	*focused_client = c;
}

destroyed_winbuf init_destroyed_winbuf() {
	int cap = 10;
	Window *buf = malloc(cap * sizeof(Window));
	return (destroyed_winbuf){buf, 0, cap};
}

void append_to_destroyed_winbuf(destroyed_winbuf *winbuf, Window win) {
	if (winbuf->pos >= winbuf->cap) {
		winbuf->cap += 10;
		winbuf->windows =
			realloc(winbuf->windows, winbuf->cap * sizeof(Window));
	}
	winbuf->windows[winbuf->pos] = win;
	winbuf->pos++;
}
