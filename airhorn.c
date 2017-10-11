#include "airhorn.h"

void setup(xcb_connection_t *connection) {
	xcb_generic_error_t *err;
	xcb_screen_t *screen = xcb_setup_roots_iterator(xcb_get_setup(connection)).data;
	xcb_grab_pointer_cookie_t grab_cookie = xcb_grab_pointer(connection, True, screen->root, XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE, XCB_GRAB_MODE_ASYNC, XCB_GRAB_MODE_ASYNC, XCB_NONE, XCB_NONE, XCB_TIME_CURRENT_TIME);
	xcb_grab_pointer_reply_t *reply = xcb_grab_pointer_reply(connection, grab_cookie, &err);
	if (reply->status != XCB_GRAB_STATUS_SUCCESS) {
		perror("could not grab pointer");
		exit(1);
	}
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
			case XCB_MOTION_NOTIFY:
				printf("pointer motion\n");
				break;
			default:
				break;
		}
		free(e);
	}
	xcb_ungrab_pointer(connection, XCB_TIME_CURRENT_TIME);
}
