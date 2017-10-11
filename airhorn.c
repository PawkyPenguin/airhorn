#include "airhorn.h"
#define EVENT_MASK ~0x80

int main(int argc, char *argv[]) {
	// Setup xcb connection
	xcb_generic_event_t *e;
	xcb_generic_error_t *err;
	xcb_connection_t *connection = xcb_connect(NULL, NULL);
	xcb_screen_t *screen = xcb_setup_roots_iterator(xcb_get_setup(connection)).data;
	// Grab left mouse button
	xcb_void_cookie_t grab_cookie = xcb_grab_button_checked(connection, True, screen->root, XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE, XCB_GRAB_MODE_SYNC, XCB_GRAB_MODE_ASYNC, XCB_NONE, XCB_NONE, XCB_BUTTON_INDEX_1, XCB_MOD_MASK_ANY);
	// Do some error handling
	xcb_generic_error_t *error = xcb_request_check(connection, grab_cookie);
	if (error != NULL) {
		xcb_disconnect(connection);
		perror("could not subscribe to events on a window, bailing out");
		exit(1);
	}  
	free(error);
	do {
		sleep(0.05);
		// Replay button press events
		xcb_allow_events(connection, XCB_ALLOW_REPLAY_POINTER, XCB_CURRENT_TIME);
		// Poll asynchronously. Unfortunately I didn't find out how to have this work with waiting synchronously because the xcb manual is too good for mere humans to comprehend.
		e = xcb_poll_for_event(connection);
		if(!e) {
			continue;
		}
		// Handle events within the application
		switch(e->response_type & EVENT_MASK) {
			case XCB_BUTTON_RELEASE:
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
	} while(1);
	xcb_ungrab_pointer(connection, XCB_TIME_CURRENT_TIME);
}
