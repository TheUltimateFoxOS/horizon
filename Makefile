OBJDIR = lib
BUILDDIR = bin

build:
	make -C core

	make -C devices
	make -C filesystems
	make -C features

setup:
	@mkdir $(BUILDDIR)
	@mkdir $(OBJDIR)

clean:
	@rm -r $(BUILDDIR)
	@rm -r $(OBJDIR)
	@mkdir $(BUILDDIR)
	@mkdir $(OBJDIR)

.PHONY: build