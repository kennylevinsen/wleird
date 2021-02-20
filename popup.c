#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "client.h"

static struct wleird_toplevel toplevel = {0};
static struct wleird_surface popup_surface = { 0 };
static struct xdg_surface *popup_xdg_surface = NULL;
static struct xdg_popup *popup_xdg_popup = NULL;

void popup_xdg_surface_handle_configure(void *data,
		struct xdg_surface *xdg_surface, uint32_t serial) {
	xdg_surface_ack_configure(xdg_surface, serial);
	surface_render(&popup_surface);
}

struct xdg_surface_listener popup_xdg_surface_listener = {
	.configure = popup_xdg_surface_handle_configure,
};

int main(int argc, char *argv[]) {
	struct wl_display *display = wl_display_connect(NULL);
	if (display == NULL) {
		fprintf(stderr, "failed to create display\n");
		return EXIT_FAILURE;
	}

	registry_init(display);
	toplevel_init(&toplevel);

	float toplevel_color[4] = {1, 0, 0, 1};
	memcpy(toplevel.surface.color, toplevel_color, sizeof(float[4]));
	float popup_color[4] = {0, 1, 0, 1};
	memcpy(popup_surface.color, popup_color, sizeof(float[4]));

	surface_init(&popup_surface);

	struct xdg_positioner *positioner = xdg_wm_base_create_positioner(wm_base);
	xdg_positioner_set_size(positioner, 300, 400);
	xdg_positioner_set_anchor_rect(positioner, 10, 10, 20, 20);
	xdg_positioner_set_anchor(positioner, 6);
	xdg_positioner_set_gravity(positioner, 8);
	xdg_positioner_set_constraint_adjustment(positioner, 61);

	popup_xdg_surface = xdg_wm_base_get_xdg_surface(wm_base, popup_surface.wl_surface);
	xdg_surface_add_listener(popup_xdg_surface, &popup_xdg_surface_listener, NULL);

	popup_xdg_popup = xdg_surface_get_popup(popup_xdg_surface, toplevel.xdg_surface, positioner);
	xdg_positioner_destroy(positioner);

	wl_surface_commit(popup_surface.wl_surface);

	while (wl_display_dispatch(display) != -1) {
		// This space intentionally left blank
	}

	return EXIT_SUCCESS;
}
