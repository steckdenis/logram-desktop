all:
	@(cd datetime && qmake && $(MAKE))
	@(cd notify && qmake && $(MAKE))

clean:
	@(cd datetime && if test -f Makefile; then $(MAKE) clean; fi)
	@(cd notify && if test -f Makefile; then $(MAKE) clean; fi)
