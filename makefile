STDTAR := all check install uninstall clean
SUBDIR := game_server

.phony: $(STDTAR)
$(STDTAR):
	for dir in $(SUBDIR); do $(MAKE) -C game_server $@; done
