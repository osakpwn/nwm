#include "client.h"
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/cursorfont.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define MODIFIER Mod4Mask
#define APP_LAUNCHER "dmenu_run"
#define EXIT_WM_KEY                                                            \
	"q" // this translates to MODIFIER + q and the same applies to all _KEY
		// definitions
#define CLOSE_APP_KEY "c"
#define OPEN_APP_LAUNCHER_KEY "space"
#define LEFT_KEY "Left"
#define RIGHT_KEY "Right"
#define UP_KEY "Up"
#define DOWN_KEY "Down"

#define MOVE_VEL 10
#define RESIZE_VEL 10

#define MOVE_WINDOW_KEY "m"
#define RESIZE_WINDOW_KEY "r"

Display *display;
Window root;
client_t *client_head = NULL;
client_t *focused_client = NULL;
bool move_window = false;
bool resize_window = false;

KeyCode gen_key_code_from_string(const char *k) {
	KeySym ksym = XStringToKeysym(k);
	return XKeysymToKeycode(display, ksym);
}

void grab_key_from_string(const char *k, unsigned int mod) {
	KeyCode kcode = gen_key_code_from_string(k);
	XGrabKey(display, kcode, mod, root, True, GrabModeAsync, GrabModeAsync);
}

int main() {
	display = XOpenDisplay(NULL);
	root = XDefaultRootWindow(display);
	Cursor cursor = XCreateFontCursor(display, XC_left_ptr);

	XClearWindow(display, root);
	XSelectInput(display, root,
				 SubstructureNotifyMask | SubstructureRedirectMask |
					 ButtonPressMask | ButtonReleaseMask | PointerMotionMask);

	XDefineCursor(display, root, cursor);
	XGrabButton(display, Button1, 0, root, False, ButtonPressMask, GrabModeSync,
				GrabModeAsync, root, cursor);

	grab_key_from_string(EXIT_WM_KEY, MODIFIER);
	grab_key_from_string(OPEN_APP_LAUNCHER_KEY, MODIFIER);
	grab_key_from_string(CLOSE_APP_KEY, MODIFIER);
	grab_key_from_string(LEFT_KEY, MODIFIER);
	grab_key_from_string(RIGHT_KEY, MODIFIER);
	grab_key_from_string(UP_KEY, MODIFIER);
	grab_key_from_string(DOWN_KEY, MODIFIER);
	grab_key_from_string(MOVE_WINDOW_KEY, MODIFIER);
	grab_key_from_string(RESIZE_WINDOW_KEY, MODIFIER);

	XEvent e;
	while (1) {
		XNextEvent(display, &e);
		switch (e.type) {
		case MapRequest: {
			XMapRequestEvent *mr = &e.xmaprequest;
			map_window(display, &root, &client_head, &focused_client, mr);
			break;
		}
		case KeyPress: {
			KeyCode keycode = e.xkey.keycode;
			unsigned int state = e.xkey.state;
			KeyCode quit_kcode = gen_key_code_from_string(EXIT_WM_KEY);
			KeyCode launcher_kcode =
				gen_key_code_from_string(OPEN_APP_LAUNCHER_KEY);
			KeyCode kill_client_kcode = gen_key_code_from_string(CLOSE_APP_KEY);

			KeyCode left_kcode = gen_key_code_from_string(LEFT_KEY);
			KeyCode right_kcode = gen_key_code_from_string(RIGHT_KEY);
			KeyCode up_kcode = gen_key_code_from_string(UP_KEY);
			KeyCode down_kcode = gen_key_code_from_string(DOWN_KEY);

			KeyCode move_kcode = gen_key_code_from_string(MOVE_WINDOW_KEY);
			KeyCode resize_kcode = gen_key_code_from_string(RESIZE_WINDOW_KEY);

			if (state & MODIFIER) {
				if (keycode == quit_kcode) {
					exit(0);
				} else if (keycode == launcher_kcode) {
					system(APP_LAUNCHER);
				} else if (keycode == kill_client_kcode) {
					if (focused_client != NULL) {
						XDestroyWindow(display, focused_client->frame);
						focused_client = NULL;
					}
				}

				if (keycode == move_kcode) {
					move_window = !move_window;
					resize_window = false;
				}
				if (keycode == resize_kcode) {
					resize_window = !resize_window;
					move_window = false;
				}

				if (focused_client != NULL) {
					if (move_window) {
						if (keycode == left_kcode) {
							if (focused_client->x > 0)
								focused_client->x -= MOVE_VEL;
						} else if (keycode == right_kcode) {
							focused_client->x += MOVE_VEL;
						} else if (keycode == up_kcode) {
							if (focused_client->y > 0)
								focused_client->y -= MOVE_VEL;
						} else if (keycode == down_kcode) {
							focused_client->y += MOVE_VEL;
						}

						resize_window = false;
					}

					if (resize_window) {
						if (keycode == left_kcode) {
							focused_client->width -= RESIZE_VEL;
						} else if (keycode == right_kcode) {
							focused_client->width += RESIZE_VEL;
						} else if (keycode == up_kcode) {
							focused_client->height -= RESIZE_VEL;
						} else if (keycode == down_kcode) {
							focused_client->height += RESIZE_VEL;
						}

						move_window = false;
					}
				}
			}
			break;
		}
		case ButtonPress: {
			Window win = e.xbutton.subwindow;
			printf("win: 0x%lu\n", win);
			for (client_t *it = client_head; it != NULL; it = it->next) {
				if (it->frame == win && win != root) {
					focused_client = it;
				}
			}

			XAllowEvents(display, ReplayPointer, CurrentTime);
		}
		}

		on_window_focus(display, focused_client, move_window, resize_window);
		XSync(display, 0);
	}
	return 0;
}
