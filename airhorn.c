#include "airhorn.h"
#define EVENT_MASK ~0x80
#define CROSSHAIR_WIDTH 32
#define CROSSHAIR_HEIGHT 32

// generate ids for graphics context and pixmap and load pixmap. Graphics context is for the root window.
void setup_pixmap(xcb_connection_t *connection, xcb_window_t window, xcb_pixmap_t *pixmap_ptr, xcb_gcontext_t *gcontext, xcb_image_t *img) {
	/* create backing pixmap
	pmap = xcb_generate_id(c);
	xcb_create_pixmap(c, 24, pmap, w, image->width, image->height);

	/* create pixmap plot gc
	mask = XCB_GC_FOREGROUND | XCB_GC_BACKGROUND;
	values[0] = s->black_pixel;
	values[1] = 0xffffff;

	gc = xcb_generate_id (c);
	xcb_create_gc (c, gc, pmap, mask, values);

	/* put the image into the pixmap 
	xcb_image_put(c, pmap, gc, image, 0, 0, 0);*/

	*pixmap_ptr = xcb_generate_id(connection);
	xcb_void_cookie_t cookie = xcb_create_pixmap(connection, XCB_COPY_FROM_PARENT, *pixmap_ptr, window, img->width, img->height);// this segfaults, apparently img->width and img->height point to the wrong place
	*gcontext = xcb_generate_id(connection);
	u_int32_t values[] = {0x000000, 0xffffff};
	xcb_create_gc(connection, *gcontext, *pixmap_ptr, XCB_GC_FOREGROUND | XCB_GC_BACKGROUND, values);
	xcb_void_cookie_t put_img_cookie = xcb_image_put(connection, *pixmap_ptr, *gcontext, img, 0, 0, 0);// this segfaults
	xcb_generic_error_t *error = xcb_request_check(connection, put_img_cookie);
	if(error != NULL) {
		perror("Can't put image into X window. This is not supposed to happen, quitting gracefully.");
		xcb_disconnect(connection);
		xcb_ungrab_pointer(connection, XCB_TIME_CURRENT_TIME);
		xcb_free_pixmap(connection, *pixmap_ptr);
		xcb_image_destroy(img);
		exit(1);
	}
}

int main(int argc, char *argv[]) {
	// Setup xcb connection
	xcb_generic_event_t *e;
	xcb_generic_error_t *err;
	xcb_connection_t *connection = xcb_connect(NULL, NULL);
	xcb_screen_t *screen = xcb_setup_roots_iterator(xcb_get_setup(connection)).data;
	xcb_window_t cursor_window = xcb_generate_id(connection);
	
	// Grab left mouse button
	xcb_void_cookie_t grab_cookie = xcb_grab_button_checked(connection, True, screen->root, XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE, XCB_GRAB_MODE_SYNC, XCB_GRAB_MODE_ASYNC, XCB_NONE, XCB_NONE, XCB_BUTTON_INDEX_1, XCB_MOD_MASK_ANY);
	// Do some error handling
	xcb_generic_error_t *error = xcb_request_check(connection, grab_cookie);
	if (error != NULL) {
		xcb_disconnect(connection);
		perror("Could not subscribe to events on a window, bailing out");
		exit(1);
	}  
	free(error);

	// setup pixmaps and graphics context pointers
	xcb_pixmap_t *pixmap_p = malloc(sizeof(xcb_pixmap_t));
	xcb_gcontext_t *gc_p = malloc(sizeof(xcb_gcontext_t));
	// load crosshair image for cursor
	//unsigned char *img_base = (unsigned char *) malloc(CROSSHAIR_WIDTH * CROSSHAIR_HEIGHT * 4);
	unsigned int img_dimensions = gimp_image.width * gimp_image.height * gimp_image.bytes_per_pixel;
	unsigned char *img_data = (unsigned char *)malloc(img_dimensions);
	for (int i = 0; i < gimp_image.height * gimp_image.width; i++) {
		//img_data[i] = gimp_image.pixel_data[i];
		img_data[i++] = rand()%256;
		img_data[i++] = rand()%256;
		img_data[i++] = rand()%256;
	}
	xcb_image_t *img = xcb_image_create_native(connection, gimp_image.width, gimp_image.height, XCB_IMAGE_FORMAT_Z_PIXMAP, 32, img_data, img_dimensions, img_data);
	printf("Dimensions: %d, %d.\n", img->width, img->height);
	printf("Some values: %d, %d.\n", rand() % 256, rand() % 256);
	//xcb_image_destroy(img);

	// setup the pixmap we'll draw the image into
	setup_pixmap(connection, cursor_window, pixmap_p, gc_p, img);
	// setup the window that will follow around our cursor to display the crosshair
	u_int32_t values[2];
	values[0] = screen->white_pixel;
	values[1] = XCB_EVENT_MASK_EXPOSURE;
	xcb_create_window(connection, XCB_COPY_FROM_PARENT, cursor_window, screen->root, 0, 0, 500, 500, 0, XCB_WINDOW_CLASS_INPUT_OUTPUT, screen->root_visual, XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK, values);
	// set override_redirect to true so the window manager does not tamper with our window.
	u_int32_t override_value[] = {True};

	xcb_change_window_attributes(connection, cursor_window, XCB_CW_OVERRIDE_REDIRECT, override_value);
	// draw image into window and do some error checking.
	xcb_void_cookie_t copy_answer = xcb_copy_area_checked(connection, *pixmap_p, cursor_window, *gc_p, 0, 0, 100, 100, img->width, img->height);
	if (!xcb_request_check(connection, copy_answer)){
		perror("Could not draw crosshair pixmap onto window!");
		exit(1);
	}
	// map window to screen
	xcb_flush(connection);
	xcb_map_window(connection, cursor_window);
	xcb_flush(connection);
	// begin checking for mouse events
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
		xcb_map_window(connection, cursor_window);
		xcb_flush(connection);
		free(e);
	} while(1);
	xcb_ungrab_pointer(connection, XCB_TIME_CURRENT_TIME);
	xcb_free_pixmap(connection, *pixmap_p);
	xcb_image_destroy(img);
	xcb_disconnect(connection);
}
