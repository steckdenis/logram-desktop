all:
	@(cd desktopentries && qmake && $(MAKE))
	@(cd taskbar && qmake && $(MAKE))

clean:
	@(cd desktopentries && if test -f Makefile; then $(MAKE) clean; fi)
	@(cd taskbar && if test -f Makefile; then $(MAKE) clean; fi)
