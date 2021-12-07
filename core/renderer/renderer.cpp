#include <renderer/renderer.h>

#include <renderer/render2d.h>

#include <utils/log.h>
#include <memory/page_frame_allocator.h>

using namespace renderer;

namespace renderer {
	framebuffer_t default_framebuffer;
}

void renderer::setup(stivale2_struct* bootinfo) {
	stivale2_struct_tag_framebuffer* framebuffer = stivale2_tag_find<stivale2_struct_tag_framebuffer>(bootinfo, STIVALE2_STRUCT_TAG_FRAMEBUFFER_ID);

	default_framebuffer = {
		.base_address = (void*) framebuffer->framebuffer_addr,
		.buffer_size = (size_t) framebuffer->framebuffer_width * framebuffer->framebuffer_height * framebuffer->framebuffer_bpp,
		.width = framebuffer->framebuffer_width,
		.height = framebuffer->framebuffer_height
	};

	debugf("Framebuffer:\n");
	debugf("> base_address: 0x%x\n", default_framebuffer.base_address);
	debugf("> buffer_size: 0x%x\n", default_framebuffer.buffer_size);
	debugf("> width: %d\n", default_framebuffer.width);
	debugf("> height: %d\n", default_framebuffer.height);

	debugf("Creating global_renderer2d...\n");

	//render2d* renderer2d = (render2d*) memory::global_allocator.request_pages(sizeof(render2d) / 0x1000 + 1);
	//*renderer2d = render2d(&default_framebuffer);
	render2d* renderer2d = new render2d(&default_framebuffer);
	global_renderer_2d = renderer2d;
}
