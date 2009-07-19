all:
	@(cd misc && qmake &&$(MAKE))
	@(cd windowsystem && qmake && $(MAKE))
	@(cd lio && qmake && $(MAKE))
	@(cd widgets && qmake && $(MAKE))
	@(cd plugins && $(MAKE))

install:
	@(cd misc &&$(MAKE) install INSTALL_ROOT=$(DESTDIR))
	@(cd plugins && $(MAKE) install)
	@(cd widgets && $(MAKE) install INSTALL_ROOT=$(DESTDIR))
	@(cd windowsystem && $(MAKE) install INSTALL_ROOT=$(DESTDIR))
	@(cd lio && $(MAKE) install INSTALL_ROOT=$(DESTDIR))

clean:
	@(cd misc && if test -f Makefile; then $(MAKE) clean; fi)
	@(cd plugins && $(MAKE) clean)
	@(cd widgets && if test -f Makefile; then $(MAKE) clean; fi)
	@(cd windowsystem && if test -f Makefile; then $(MAKE) clean; fi)
	@(cd lio && if test -f Makefile; then $(MAKE) clean; fi)
