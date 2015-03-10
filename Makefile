DIRS = lib cat

all:
	for dir in $(DIRS); do \
		$(MAKE) -C $$dir clean; \
	done

clean:
	for dir in $(DIRS); do \
		$(MAKE) -C $$dir; \
	done	
