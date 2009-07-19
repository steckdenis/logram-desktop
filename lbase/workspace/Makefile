all:
	@(cd panache && $(MAKE))
	@(cd bloq && qmake && $(MAKE))
	@(cd locomrunner && qmake && $(MAKE))
	@(cd locom && qmake && $(MAKE))

install:
	@(cd panache && $(MAKE) install)
	@(cd bloq && $(MAKE) install INSTALL_ROOT=$(DESTDIR))
	@(cd locomrunner && $(MAKE) install INSTALL_ROOT=$(DESTDIR))
	@(cd locom && $(MAKE) install INSTALL_ROOT=$(DESTDIR))

clean:
	@(cd panache && $(MAKE) clean)
	@(cd bloq && if test -f Makefile; then $(MAKE) clean; fi)
	@(cd locomrunner && if test -f Makefile; then $(MAKE) clean; fi)
	@(cd locom && if test -f Makefile; then $(MAKE) clean; fi)
