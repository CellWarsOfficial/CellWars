STDTAR := all check install uninstall clean
SUBDIR := database game_server

.phony: $(STDTAR)
$(STDTAR):
	for dir in $(SUBDIR); do $(MAKE) -C $$dir $@; done

.phony: pings
pings:
	-ssh root@146.169.45.167 -p 1631 ':'
	-ssh root@89.122.28.235 -p 1631 ':'
