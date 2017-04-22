# Note that gcc must be built with --enable-lto option (gcc 4.5 or newer)!
# At them momennt gold linker supports only x86 platform, so we are not going to use libraries

all:
	+make -C src

clean:
	+make -C src clean

distclean:
	+make -C src distclean

.PHONY: all clean distclean
