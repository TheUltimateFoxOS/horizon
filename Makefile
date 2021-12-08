OBJDIR = lib
BUILDDIR = bin

build:
	make -C core

	make -C test_mod
	make -C ps2_keyboard

setup:
	@mkdir $(BUILDDIR)
	@mkdir $(OBJDIR)

clean:
	@rm -r $(BUILDDIR)
	@rm -r $(OBJDIR)
	@mkdir $(BUILDDIR)
	@mkdir $(OBJDIR)

.PHONY: build