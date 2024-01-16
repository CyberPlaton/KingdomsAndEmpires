#include "app_module.hpp"

#define SOKOL_IMPL
#if defined (_WIN32) ||defined(_WIN64)
#define SOKOL_D3D11
#elif defined(__linux__)
#define SOKOL_GLCORE33
#elif defined(__APPLE__)
#define SOKOL_METAL
#endif
#include <sokol/sokol_app.h>
#include <sokol/sokol_glue.h>

// Called on every frame of the application.
static void frame(void) {
	// Get current window size.
	int width = sapp_width(), height = sapp_height();
	float ratio = width / (float)height;

	// Begin recording draw commands for a frame buffer of size (width, height).
	sgp_begin(width, height);
	// Set frame buffer drawing region to (0,0,width,height).
	sgp_viewport(0, 0, width, height);
	// Set drawing coordinate space to (left=-ratio, right=ratio, top=1, bottom=-1).
	sgp_project(-ratio, ratio, 1.0f, -1.0f);

	// Clear the frame buffer.
	sgp_set_color(0.1f, 0.1f, 0.1f, 1.0f);
	sgp_clear();

	// Draw an animated rectangle that rotates and changes its colors.
	float time = sapp_frame_count() * sapp_frame_duration();
	float r = sinf(time) * 0.5 + 0.5, g = cosf(time) * 0.5 + 0.5;
	sgp_set_color(r, g, 0.3f, 1.0f);
	sgp_rotate_at(time, 0.0f, 0.0f);
	sgp_draw_filled_rect(-0.5f, -0.5f, 1.0f, 1.0f);

	// Begin a render pass.
	sg_pass_action pass_action = { 0 };
	sg_begin_default_pass(&pass_action, width, height);
	// Dispatch all draw commands to Sokol GFX.
	sgp_flush();
	// Finish a draw command queue, clearing it.
	sgp_end();
	// End render pass.
	sg_end_pass();
	// Commit Sokol render.
	sg_commit();
}

// Called when the application is initializing.
static void init(void) {
	// Initialize Sokol GFX.
	sg_desc sgdesc;
	sgdesc.context = sapp_sgcontext();
	sgdesc.logger.func = slog_func;

	sg_setup(&sgdesc);
	if (!sg_isvalid()) {
	}

	// Initialize Sokol GP, adjust the size of command buffers for your own use.
	sgp_desc sgpdesc = { 0 };
	sgp_setup(&sgpdesc);
	if (!sgp_is_valid()) {
	}
}

// Called when the application is shutting down.
static void cleanup(void) {
	// Cleanup Sokol GP and Sokol GFX resources.
	sgp_shutdown();
	sg_shutdown();
}


sapp_desc sokol_main(int argc, char* argv[])
{
	sapp_desc desc;
	desc.init_cb = init;
	desc.frame_cb = frame;
	desc.cleanup_cb = cleanup;
	desc.window_title = "Rectangle (Sokol GP)";
	desc.logger.func = slog_func;

	return desc;
}