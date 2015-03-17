DIRS = lib cat revwords

all:
	for dir in $(DIRS); do \
		$(MAKE) -C $$dir; \
	done

clean:
	for dir in $(DIRS); do \
		$(MAKE) -C $$dir clean; \
	done	
