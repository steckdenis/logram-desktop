all:
	@(cd newfile && qmake && $(MAKE))
	@(cd openwith && qmake && $(MAKE))

install:
	cp *.so $(DESTDIR)/usr/lib
	cp ./include/*.h  $(DESTDIR)/usr/include/logram

clean:
	@(cd newfile && if test -f Makefile; then $(MAKE) clean; fi)
	@(cd openwith && if test -f Makefile; then $(MAKE) clean; fi)
