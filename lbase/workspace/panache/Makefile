all:
	@(cd desktop && qmake && $(MAKE))
	@(cd wm && qmake && $(MAKE))

install:
	@(cd desktop && $(MAKE) install INSTALL_ROOT=$(DESTDIR))
	@(cd wm && $(MAKE) install INSTALL_ROOT=$(DESTDIR))

clean:
	@(cd desktop && if test -f Makefile; then $(MAKE) clean; fi)
	@(cd wm && if test -f Makefile; then $(MAKE) clean; fi)
