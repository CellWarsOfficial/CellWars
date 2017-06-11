STDTAR := all check clean
SPCTAR := install uninstall
SUBDIR := client database game_server
ASIOD = asio-1.10.6
ASIOLOC = https://sourceforge.net/projects/asio/files/asio/1.10.6%20%28Stable%29/asio-1.10.6.tar.bz2
RM = rm -rf
DL = wget -q
UNTAR_BZ2 = tar xfj

$(STDTAR): $(ASIOD)/Makefile
	#$(MAKE) -C $(ASIOD) $@
	for dir in $(SUBDIR); do $(MAKE) -C $$dir $@; done

.phony: $(STDTAR) $(SPCTAR)
$(SPCTAR):
	for dir in $(SUBDIR); do $(MAKE) -C $$dir $@; done

$(ASIOD)/Makefile:
	#$(DL) $(ASIOLOC)
	#$(UNTAR_BZ2) asio-1.10.6.tar.bz2
	#$(RM) asio-1.10.6.tar.bz2
	#cd $(ASIOD) && ./configure --without-boost
	-mkdir $(ASIOD)
	-touch $(ASIOD)/Makefile

.phony: pings
pings:
	nc 146.169.45.167 1631
	-nc 89.122.28.235 1631
