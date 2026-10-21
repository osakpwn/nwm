#include "../include/client.h"
#include "../include/keys.h"
#include "../include/notification.h"
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/cursorfont.h>
#include <X11/keysym.h>
#include <bits/time.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include "../include/ipc.h"

#define MODIFIER MOD4_KEY
#define APP_LAUNCHER "dmenu_run"
#define EXIT_WM_KEY XK_q
#define CLOSE_APP_KEY XK_c
#define OPEN_APP_LAUNCHER_KEY XK_space
#define MOVE_VEL 10
#define RESIZE_VEL 10

#define MOVE_WINDOW_KEY XK_m
#define RESIZE_WINDOW_KEY XK_r

#define FULL_SCREEN_KEY XK_f
#define MINIMIZE_WINDOW_KEY XK_u

Display *display;
Window root;
client_t *client_head = NULL;
client_t *focused_client = NULL;
client_t *minimized_client = NULL;
notif_t *cur_notif = NULL;
bool move_window = false;
bool resize_window = false;
int screen_width = 0;
int screen_height = 0;
int dragging = 0;
int initial_mouse_x = 0, initial_mouse_y = 0;
int initial_win_x = 0, initial_win_y = 0;



client_t *get_client_head ()
{
	return client_head;
}
void set_client_head (client_t *ch)
{
	client_head = ch;
}

void grab_key_from_keysym(KeySym k, unsigned int mod) {
	XGrabKey(display, XKeysymToKeycode(display, k), mod, root, True,
			 GrabModeAsync, GrabModeAsync);
}

int error_handler(Display *d, XErrorEvent *ee) {
	if (ee->error_code == BadWindow || ee->error_code == BadDrawable ||
		ee->error_code == BadMatch) {
		return 0; // Ignore safely
	}
	fprintf(stderr, "Fatal X11 WM Error: %d\n", ee->error_code);
	return 0;
}

void request_window_close(Window win) {
	Atom wm_delete = XInternAtom(display, "WM_DELETE_WINDOW", False);
	Atom wm_protocols = XInternAtom(display, "WM_PROTOCOLS", False);

	XEvent ev = {0};

	ev.xclient.type = ClientMessage;
	ev.xclient.window = win;
	ev.xclient.message_type = wm_protocols;
	ev.xclient.format = 32;
	ev.xclient.data.l[0] = wm_delete;
	ev.xclient.data.l[1] = CurrentTime;

	XSendEvent(display, win, False, NoEventMask, &ev);
}


void handle_workspace_switching_keys (KeySym ksym)
{
	switch (ksym) 
	{
		case WORKSPACE_1_KEY: 
			switch_to_workspace(display,& client_head, 1);
			break;
		case WORKSPACE_2_KEY: 
			switch_to_workspace(display,& client_head, 2);
			break;

		case WORKSPACE_3_KEY: 
			switch_to_workspace(display,& client_head, 3);
			break;

		case WORKSPACE_4_KEY: 
			switch_to_workspace(display,& client_head, 4);
			break;
		case WORKSPACE_5_KEY: 
			switch_to_workspace(display,& client_head, 5);
			break;

		case WORKSPACE_6_KEY: 
			switch_to_workspace(display,& client_head, 6);
			break;
		
		case WORKSPACE_7_KEY: 
			switch_to_workspace(display,& client_head, 7);
			break;

		case WORKSPACE_8_KEY: 
			switch_to_workspace(display,& client_head, 8);
			break;
	
		case WORKSPACE_9_KEY: 
			switch_to_workspace(display,& client_head, 9);
			break;

	}
}

int main() {
	display = XOpenDisplay(NULL);
	root = XDefaultRootWindow(display);
	Cursor cursor = XCreateFontCursor(display, XC_left_ptr);

	screen_width = XDisplayWidth(display, DefaultScreen(display));
	screen_height = XDisplayHeight(display, DefaultScreen(display));

	XClearWindow(display, root);
	XSelectInput(display, root,
				 SubstructureNotifyMask | SubstructureRedirectMask |
					 ButtonPressMask | ButtonReleaseMask | PointerMotionMask);

	XDefineCursor(display, root, cursor);
	XGrabButton(display, Button1, 0, root, False, ButtonPressMask, GrabModeSync,
				GrabModeAsync, root, cursor);

	grab_key_from_keysym(EXIT_WM_KEY, MODIFIER | ShiftMask);
	grab_key_from_keysym(OPEN_APP_LAUNCHER_KEY, MODIFIER);
	grab_key_from_keysym(CLOSE_APP_KEY, MODIFIER);
	grab_key_from_keysym(CLOSE_APP_KEY, MODIFIER | ShiftMask);
	grab_key_from_keysym(LEFT_KEY, MODIFIER);
	grab_key_from_keysym(RIGHT_KEY, MODIFIER);
	grab_key_from_keysym(UP_KEY, MODIFIER);
	grab_key_from_keysym(DOWN_KEY, MODIFIER);
	grab_key_from_keysym(MOVE_WINDOW_KEY, MODIFIER);
	grab_key_from_keysym(RESIZE_WINDOW_KEY, MODIFIER);
	grab_key_from_keysym(FULL_SCREEN_KEY, MODIFIER);
	grab_key_from_keysym(MINIMIZE_WINDOW_KEY, MODIFIER);
	
	grab_key_from_keysym(WORKSPACE_1_KEY, MODIFIER);
	grab_key_from_keysym(WORKSPACE_2_KEY, MODIFIER);
	grab_key_from_keysym(WORKSPACE_3_KEY, MODIFIER);
	grab_key_from_keysym(WORKSPACE_4_KEY, MODIFIER);
	grab_key_from_keysym(WORKSPACE_5_KEY, MODIFIER);
	grab_key_from_keysym(WORKSPACE_6_KEY, MODIFIER);
	grab_key_from_keysym(WORKSPACE_7_KEY, MODIFIER);
	grab_key_from_keysym(WORKSPACE_8_KEY, MODIFIER);
	grab_key_from_keysym(WORKSPACE_9_KEY, MODIFIER);


	pthread_t thread;
	pthread_create(&thread, NULL, new_socket_session, NULL);


	XEvent e;
	XSetErrorHandler(error_handler);
	while (1) {
		XNextEvent(display, &e);
		switch (e.type) {
		case MapRequest: {
			XMapRequestEvent *mr = &e.xmaprequest;
			map_window(display, &root, &client_head, &focused_client, mr);
			break;
		}
		case KeyPress: {
			KeySym ksym = XLookupKeysym(&e.xkey, 0);
			unsigned int state = e.xkey.state;


			if (ksym == EXIT_WM_KEY) {
					exit(0);
				}
			if (state & MODIFIER) {
				handle_workspace_switching_keys (ksym);
				 if (ksym == OPEN_APP_LAUNCHER_KEY) {
					system(APP_LAUNCHER);
				} else if (ksym == CLOSE_APP_KEY) {
					if (focused_client != NULL) {
						// XKillClient(display, focused_client->win);
						// XDestroyWindow(display, focused_client->frame);
						request_window_close(focused_client->win);
						focused_client->window_close_request_sent = true;
						clock_gettime(
							CLOCK_MONOTONIC,
							&focused_client->time_window_close_request_sent);
					}
				}

				if (ksym == MOVE_WINDOW_KEY) {
					move_window = !move_window;
					resize_window = false;
				}
				if (ksym == RESIZE_WINDOW_KEY) {
					resize_window = !resize_window;
					move_window = false;
				}
				if (ksym == MINIMIZE_WINDOW_KEY) {
					if (minimized_client != NULL) {
						minimized_client->is_minimized = false;
						focused_client = minimized_client;
						minimized_client = NULL;
						XMapWindow(display, focused_client->frame);
					} else if (focused_client != NULL) {
						if (!focused_client->is_minimized &&
							minimized_client == NULL) {
							focused_client->is_minimized = true;
							resize_window = false;
							move_window = false;
							minimized_client = focused_client;
							focused_client = NULL;
							XUnmapWindow(display, minimized_client->frame);
						}
					}
				}
				if (focused_client != NULL) {

					if (ksym == FULL_SCREEN_KEY) {
						focused_client->is_fullscreen =
							!focused_client->is_fullscreen;
						if (focused_client->is_fullscreen) {
							focused_client->prev_width = focused_client->width;
							focused_client->prev_height =
								focused_client->height;
							focused_client->prev_x = focused_client->x;
							focused_client->prev_y = focused_client->y;

							focused_client->width = screen_width - 13;
							focused_client->height = screen_height - 13;
							focused_client->x = 4;
							focused_client->y = 4;
						} else {
							focused_client->width = focused_client->prev_width;
							focused_client->height =
								focused_client->prev_height;
							focused_client->x = focused_client->prev_x;
							focused_client->y = focused_client->prev_y;
						}
					}

					if (move_window) {
						if (ksym == LEFT_KEY) {
							if (focused_client->x > 0)
								focused_client->x -= MOVE_VEL;
						} else if (ksym == RIGHT_KEY) {
							if (focused_client->x <
								screen_width - focused_client->width - 7)
								focused_client->x += MOVE_VEL;
						} else if (ksym == UP_KEY) {
							if (focused_client->y > 0)
								focused_client->y -= MOVE_VEL;
						} else if (ksym == DOWN_KEY) {
							if (focused_client->y <
								screen_height - focused_client->height - 8)
								focused_client->y += MOVE_VEL;
						}

						resize_window = false;
					}

					if (resize_window) {
						if (ksym == LEFT_KEY) {
							focused_client->width -= RESIZE_VEL;
						} else if (ksym == RIGHT_KEY) {
							if (focused_client->x <
								screen_width - focused_client->width - 7)

								focused_client->width += RESIZE_VEL;
						} else if (ksym == UP_KEY) {
							focused_client->height -= RESIZE_VEL;
						} else if (ksym == DOWN_KEY) {
							if (focused_client->y <
								screen_height - focused_client->height - 8)

								focused_client->height += RESIZE_VEL;
						}

						move_window = false;
					}
				}
			}

			on_window_focus(display, &client_head, focused_client, move_window,
							resize_window);
			break;
		}
		case ButtonPress: {
			Window win = e.xbutton.subwindow;
			for (client_t *it = client_head; it != NULL; it = it->next) {
				if (it->frame == win && win != root) {
					focused_client = it;
				}
			}
			XAllowEvents(display, ReplayPointer, CurrentTime);
			on_window_focus(display, &client_head, focused_client, move_window,
							resize_window);


			if (focused_client != NULL) 
			{
				XWindowAttributes attr; 
				XGetWindowAttributes(display, focused_client->win, &attr);
				focused_client->prev_x = attr.x; 
				focused_client->prev_y = attr.y; 
			}
		
			break;
		}
		case MotionNotify: {
			break;
		}

		case ButtonRelease:
			break;
		case DestroyNotify: {
			Window win = e.xdestroywindow.window;
			if (focused_client != NULL && focused_client->frame == win) {
				remove_window_from_list(&client_head, focused_client);
				focused_client = NULL;
				move_window = false;
				resize_window = false;
			}

			for (client_t *c = client_head; c != NULL; c = c->next) {
				if (win == c->win) {
					XDestroyWindow(display, c->frame);
					remove_window_from_list(
						&client_head,
						find_client_using_window(&client_head, win));
				}

				if (win == c->frame) {
					XDestroyWindow(display, c->win);
					remove_window_from_list(&client_head, c);
				}
			}
			break;
		}
		case ConfigureRequest: {
			XConfigureRequestEvent *ev = &e.xconfigurerequest;
			XWindowChanges changes = {
				.x = ev->x,
				.y = ev->y,
				.width = ev->width,
				.height = ev->height,
				.border_width = ev->border_width,
				.sibling = ev->above,
				.stack_mode = ev->detail,
			};
			XConfigureWindow(display, ev->window, ev->value_mask, &changes);

			XConfigureEvent ce = {.type = ConfigureNotify,
								  .display = display,
								  .event = ev->window,
								  .window = ev->window,
								  .x = ev->x,
								  .y = ev->y,
								  .width = ev->width,
								  .height = ev->height,
								  .border_width = ev->border_width,
								  .above = None};
			XSendEvent(display, ev->window, False, StructureNotifyMask,
					   (XEvent *)&ce);
			break;
		}
		}

		client_cleanup_loop(display, &client_head);
		cleanup_notification_window(display, cur_notif);
		XSync(display, 0);
	}
	return 0;
}
