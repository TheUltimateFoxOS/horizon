#pragma once

#include <stdint.h>


namespace renderer {

	#define PSF1_MAGIC0 0x36
	#define PSF1_MAGIC1 0x04

	struct psf1_header_t {
		uint8_t magic[2];
		uint8_t mode;
		uint8_t charsize;
	};

	struct psf1_font_t {
		psf1_header_t* psf1_Header;
		void* glyph_buffer;
	};
}