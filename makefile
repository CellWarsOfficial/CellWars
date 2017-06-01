STDTAR := all check install uninstall clean
SUBDIR := database game_server

.phony: $(STDTAR)
$(STDTAR):
	for dir in $(SUBDIR); do $(MAKE) -C $$dir $@; done
