all:
	@(cd fileprovider && qmake && $(MAKE))
	@(cd ftpprovider && qmake && $(MAKE))

install:
	cp *.so $(DESTDIR)/usr/lib
	cp ./include/*.h $(DESTDIR)/usr/include/logram

clean:
	@(cd fileprovider && if test -f Makefile; then $(MAKE) clean; fi)
	@(cd ftpprovider && if test -f Makefile; then $(MAKE) clean; fi)
