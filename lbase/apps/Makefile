all:
	@(cd live && qmake && $(MAKE))
	@(cd magellan && qmake && $(MAKE))
	
install:
	@(cd live && $(MAKE) install INSTALL_ROOT=$(DESTDIR))
	@(cd magellan && $(MAKE) install INSTALL_ROOT=$(DESTDIR))
	
clean:
	@(cd live && if test -f Makefile; then $(MAKE) clean; fi)
	@(cd magellan && if test -f Makefile; then $(MAKE) clean; fi)
	
