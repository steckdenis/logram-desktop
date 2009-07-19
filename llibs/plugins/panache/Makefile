all:
	@(cd desktop && $(MAKE))
	@(cd menu && $(MAKE))
	@(cd wallpapers && $(MAKE))
	@(cd taskbar && $(MAKE))

install:
	cp *.so $(DESTDIR)/usr/lib
	cp ./include/*.h $(DESTDIR)/usr/include/logram

clean:
	@(cd desktop && if test -f Makefile; then $(MAKE) clean; fi)
	@(cd menu && if test -f Makefile; then $(MAKE) clean; fi)
	@(cd wallpapers && if test -f Makefile; then $(MAKE) clean; fi)
	@(cd taskbar && if test -f Makefile; then $(MAKE) clean; fi)
