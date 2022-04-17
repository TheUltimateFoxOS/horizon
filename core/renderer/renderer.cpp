#include <renderer/renderer.h>

#include <renderer/render2d.h>
#include <renderer/font.h>
#include <renderer/font_renderer.h>
#include <renderer/window_renderer.h>

#include <utils/log.h>
#include <memory/page_frame_allocator.h>

#include <boot/boot.h>

using namespace renderer;

namespace renderer {
	framebuffer_t default_framebuffer;
}

extern uint8_t default_font[];

void renderer::setup() {
	default_framebuffer = boot::boot_info.framebuffer;
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

	debugf("Loading deault font embedded in binary...\n");
	psf1_header_t* font_header = (psf1_header_t*) default_font;
	if(font_header->magic[0] != PSF1_MAGIC0 || font_header->magic[1] != PSF1_MAGIC1) {
		debugf("Looks like the font is corrupted continuing anyway!\n");
	}

	uint64_t glyph_buffer_size = font_header->charsize * 256;
	if(font_header->mode == 1) {
		glyph_buffer_size = font_header->charsize * 512;
	}

	void* glyph_buffer = (default_font + sizeof(psf1_header_t));

	psf1_font_t* finished_font = new psf1_font_t;
	finished_font->psf1_Header = font_header;
	finished_font->glyph_buffer = glyph_buffer;

	debugf("Creating global_font_renderer\n");
	font_renderer* _font_renderer = new font_renderer(&default_framebuffer, finished_font);
	global_font_renderer = _font_renderer;
	log::stdout_device = renderer::global_font_renderer;

	debugf("Creating global_window_renderer\n");
	window_renderer* window = new window_renderer(&default_framebuffer);
	global_window_renderer = window;
}