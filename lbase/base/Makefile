all:
	@(cd startlogram && qmake && $(MAKE))

install:
	@(cd startlogram && $(MAKE) install INSTALL_ROOT=$(DESTDIR))

clean:
	@(cd startlogram && if test -f Makefile; then $(MAKE) clean; fi)
