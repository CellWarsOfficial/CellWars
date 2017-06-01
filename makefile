STDTAR := all check install uninstall clean
SUBDIR := game_server database

.phony: $(STDTAR)
$(STDTAR):
	for dir in $(SUBDIR); do $(MAKE) -C $$dir $@; done
