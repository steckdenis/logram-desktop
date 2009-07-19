all:
	@(cd live && qmake && $(MAKE))
	
install:
	@(cd live && $(MAKE) install INSTALL_ROOT=$(DESTDIR))

clean:
	@(cd live && if test -f Makefile; then $(MAKE) clean; fi)
