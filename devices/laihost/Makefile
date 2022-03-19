MODULE_NAME = laihost.o

include ../../Makefile.flags

CFLAGS += -I../../core/include -Ilai/include

all: ./lai
	make module

./lai:
	git clone https://github.com/managarm/lai.git

include ../../Makefile.module