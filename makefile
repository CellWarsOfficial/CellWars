STDTAR := all check install uninstall clean
SUBDIR := database game_server

.phony: $(STDTAR)
$(STDTAR):
	for dir in $(SUBDIR); do $(MAKE) -C $$dir $@; done

.phony: pings
pings:
	nc 146.169.45.167 1631
	-nc 89.122.28.235 1631
