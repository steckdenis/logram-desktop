all:
	@(cd ghetto && qmake && $(MAKE))
	@(cd Search && qmake && $(MAKE))
	@(cd WikiSearch && qmake && $(MAKE))
 
clean:

	@(cd ghetto && if test -f Makefile; then $(MAKE) clean; fi)
	@(cd WikiSearch if test -f Makefile; then $(MAKE) clean; fi)