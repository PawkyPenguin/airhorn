#include "airhorn.h"

void setup(xcb_connection_t *connection) {
	xcb_generic_error_t *err;
	xcb_screen_t *screen = xcb_setup_roots_iterator(xcb_get_setup(connection)).data;
	//xcb_void_cookie_t grab_cookie = xcb_grab_button(connection, True, screen->root, XCB_NONE, XCB_GRAB_MODE_ASYNC, XCB_GRAB_MODE_ASYNC, XCB_NONE, XCB_NONE, XCB_BUTTON_INDEX_1, XCB_MOD_MASK_ANY);
	//xcb_generic_error_t *error = xcb_request_check(connection, grab_cookie);
	//if (error != NULL) {
	//	xcb_disconnect(connection);
	//	perror("could not subscribe to events on a window, bailing out");
	//	exit(1);
	//}  
	//status = XGrabPointer(dpy, root, False,
	//		ButtonPressMask|ButtonReleaseMask, GrabModeSync,
	//		GrabModeAsync, root, cursor, CurrentTime);
	//if (status != GrabSuccess) {
	//	fprintf(stderr, "%s: Can't grab the mouse.\n", ProgramName);
	//	exit(1);
	//}

	/* Let the user select a window... */

	//free(error);
        xcb_flush(connection);
}

int main(int argc, char *argv[]) {
	XEvent event;
	xcb_generic_event_t *e;
	Display *dpy = XOpenDisplay(NULL);
	xcb_connection_t *connection = XGetXCBConnection(dpy);
	//setup(connection);
	xcb_screen_t *screen = xcb_setup_roots_iterator(xcb_get_setup(connection)).data;
	u_int32_t status = XGrabPointer(dpy, screen->root, False,
			ButtonPressMask|ButtonReleaseMask, GrabModeSync,
			GrabModeAsync, screen->root, 0, CurrentTime);
	if (status != GrabSuccess) {
		perror("can't grab.\n");
		exit(1);
	}

	XAllowEvents(dpy, SyncPointer, CurrentTime);
	do {
		/* allow one more event */
		XAllowEvents(dpy, SyncPointer, CurrentTime);
		XWindowEvent(dpy, screen->root, ButtonPressMask|ButtonReleaseMask, &event);
		switch (event.type) {
			case ButtonPress:
				printf("Hey.\n");
				break;
			case ButtonRelease:
				break;
		}
	} while(1);
	//while ((e = xcb_wait_for_event(connection))) {
	//	switch(e->response_type & ~0x80) {
	//		case XCB_BUTTON_PRESS:
	//			printf("It is work.\n");
	//			break;
	//		case XCB_KEY_PRESS:
	//			printf("go key\n");
	//			break;
	//		case XCB_MOTION_NOTIFY:
	//			printf("pointer motion\n");
	//			break;
	//		default:
	//			break;
	//	}
	//	free(e);
	//}
	//xcb_ungrab_pointer(connection, XCB_TIME_CURRENT_TIME);
}
