all:
	@(cd ghetto && qmake && $(MAKE))
	@(cd Search && qmake && $(MAKE))

clean:
	@(cd ghetto && if test -f Makefile; then $(MAKE) clean; fi)
	@(cd Search if test -f Makefile; then $(MAKE) clean; fi)
