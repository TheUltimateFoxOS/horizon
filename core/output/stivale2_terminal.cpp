#include <output/stivale2_terminal.h>
#include <utils/string.h>

using namespace output;

stivale2_terminal::stivale2_terminal() {
	this->terminal_tag = stivale2_tag_find<stivale2_struct_tag_terminal>(global_bootinfo, STIVALE2_STRUCT_TAG_TERMINAL_ID);
}

typedef void (*term_write_t)(const char*, int len);

void stivale2_terminal::putchar(char c) {
	char* char_to_send = &c;

	term_write_t term_write = (term_write_t)this->terminal_tag->term_write;
	term_write(char_to_send, 1);
}

void stivale2_terminal::putstring(const char* str) {
	term_write_t term_write = (term_write_t)this->terminal_tag->term_write;
	term_write(str, strlen((char*) str));
}

namespace output {
	stivale2_terminal* global_terminal;
}
