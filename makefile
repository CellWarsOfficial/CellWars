include envfile
export $(shell sed 's/=.*//' envfile)
STDTAR := build check clean install uninstall
SUBDIR := client database game_server

.pnony: all
all:
	echo No default script here!, consider running one of [$STDTAR]

.phony: $(STDTAR)
$(STDTAR):
	for dir in $(SUBDIR); do $(MAKE) -C $$dir $@; done

