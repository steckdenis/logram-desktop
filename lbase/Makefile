all:
	@(cd apps && $(MAKE))
	@(cd base && $(MAKE))
	@(cd workspace && $(MAKE))	

install:
	@(cd base && $(MAKE) install)
	@(cd apps && $(MAKE) install)
	@(cd workspace && $(MAKE) install)

clean:
	@(cd workspace && $(MAKE) clean)
	@(cd apps && $(MAKE) clean)
	@(cd base && $(MAKE) clean)
