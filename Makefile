# Makefile général de Logram 
#
# Se charge de la construction et des tests de tout Logram

.PHONY: llibs lartworks lbase

all: llibs_all lartworks lbase

install: llibs_install lartworks_install lbase_install

clean: llibs_clean lartworks_clean lbase_clean

#Llibs
llibs_all: llibs llibs_install

llibs:
	@(cd llibs && $(MAKE))
	
llibs_install:
	@(cd llibs && $(MAKE) install)
	
llibs_clean:
	@(cd llibs && $(MAKE) clean)
	
#lartworks
lartworks:
	@(cd lartworks && $(MAKE))
	
lartworks_install:
	@(cd lartworks && $(MAKE) install)
	
lartworks_clean:
	@(cd lartworks && $(MAKE) clean)
	
#lbase
lbase:
	@(cd lbase && $(MAKE))
	
lbase_install:
	@(cd lbase && $(MAKE) install)

lbase_clean:
	@(cd lbase && $(MAKE) clean)
	
# Test
test: all install check_xephyr
	@Xephyr :1 -ac -kb -screen 1024x768&
	@export DISPLAY=:1.0
	@startlogram
	
check_xephyr:
	@if [ -f /usr/bin/Xephyr ]; \
	then \
		echo "Xephyr détecté"; \
	else \
		echo "Xephyr n'a pas été trouvé. Il n'est nécessaire que pour tester Logram facilement. Installez le paquet xserver-xephyr (pour Ubuntu) et réessayez"; \
		return 1; \
	fi
