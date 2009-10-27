all:
	@(cd live && qmake && $(MAKE))
	@(cd magellan && qmake && $(MAKE))
	@(cd pixgrab && qmake && $(MAKE))
	@(cd ugo && qmake && $(MAKE))
	@(cd Ghetto && qmake && $(MAKE))
	@(cd QStudio && qmake && $(MAKE))
	
install:
	@(cd live && $(MAKE) install INSTALL_ROOT=$(DESTDIR))
	@(cd magellan && $(MAKE) install INSTALL_ROOT=$(DESTDIR))
	@(cd pixgrab && $(MAKE) install INSTALL_ROOT=$(DESTDIR))
	@(cd ugo && $(MAKE) install INSTALL_ROOT=$(DESTDIR))
	@(cd Ghetto && $(MAKE) install INSTALL_ROOT=$(DESTDIR))
	@(cd QStudio && $(MAKE) install INSTALL_ROOT=$(DESTDIR))

clean:
	@(cd live && if test -f Makefile; then $(MAKE) clean; fi)
	@(cd magellan && if test -f Makefile; then $(MAKE) clean; fi)
	@(cd pixgrab && if test -f Makefile; then $(MAKE) clean; fi)
	@(cd ugo && if test -f Makefile; then $(MAKE) clean; fi)
	@(cd Ghetto && if test -f Makefile; then $(MAKE) clean; fi)
	@(cd QStudio && if test -f Makefile; then $(MAKE) clean; fi)
	
