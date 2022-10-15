#include <renderer/renderer.h>

extern "C" {
	renderer::framebuffer_t __rust_get_default_framebuffer() {
		return renderer::default_framebuffer;
	}
}