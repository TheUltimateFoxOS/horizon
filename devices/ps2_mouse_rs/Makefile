MODULE_NAME = ps2_mouse.o

pre:
	cargo build --release
	make module

include ../../Makefile.flags

CFLAGS += -I../../core/include

include ../../Makefile.module

USER_OBJECTS = target/x86_64/release/deps/*.o