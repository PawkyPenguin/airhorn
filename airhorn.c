#include "airhorn.h"

void setup(xcb_connection_t *connection) {
	xcb_screen_t *screen = xcb_setup_roots_iterator(xcb_get_setup(connection)).data;
        uint32_t keypress_masks[] = { XCB_EVENT_MASK_EXPOSURE       | XCB_EVENT_MASK_BUTTON_PRESS   |
                                    XCB_EVENT_MASK_BUTTON_RELEASE | XCB_EVENT_MASK_POINTER_MOTION
                                    };
	xcb_drawable_t draw = screen->root;
	xcb_change_window_attributes(connection, draw, XCB_CW_EVENT_MASK, keypress_masks);
	xcb_aux_sync(connection);
	if (xcb_poll_for_event(connection) != NULL)
	{
		perror("another window manager is already running");
		exit(1);
	};
	xcb_map_window(connection, draw);
        xcb_flush(connection);
}

int main(int argc, char *argv[]) {
	xcb_generic_event_t *e;
	Display *dpy = XOpenDisplay(NULL);
	xcb_connection_t *connection = XGetXCBConnection(dpy);
	setup(connection);
	while ((e = xcb_wait_for_event(connection))) {
		switch(e->response_type & ~0x80) {
			case XCB_BUTTON_PRESS:
				printf("It is work.\n");
				break;
			case XCB_KEY_PRESS:
				printf("go key\n");
				break;
			default:
				printf("it is no work.\n");
				break;
		}
	}
}
