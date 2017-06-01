NEWFIFO = mkfifo
ADAPTER = adapter.sh
IGNORE = >/dev/null 2>&1
KILLNAME = killall -q
PIPENAME = fifo
NETCAT = nc
RM = rm -rf

.phony: all
all:
	echo "no compilation required"

.phony: check
check: all
	echo "not checking yet"

.phony: install
install: all uninstall installpipe
	nohup ./$(ADAPTER) $(IGNORE) &

.phony: installpipe
installpipe: $(PIPENAME)

$(PIPENAME):
	$(NEWFIFO) $@

.phony: uninstall
uninstall: remove_main remove_netcat remove_pipe

.phony: remove_main remove_netcat remove_pipe
remove_main:
	-$(KILLNAME) $(ADAPTER)

remove_netcat:
	-$(KILLNAME) $(NETCAT)

remove_pipe:
	-$(RM) $(PIPENAME)

.phony: clean
clean:
	echo "not cleaning yet"
