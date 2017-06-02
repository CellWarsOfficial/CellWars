NEWFIFO = mkfifo
ADAPTER1 = adapter.sh
ADAPTER2 = sv_adapter.sh
IGNORE = >/dev/null 2>&1
KILLNAME = killall -q
PIPE1 = fifo1
PIPE2 = fifo2
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
	nohup ./$(ADAPTER1) $(IGNORE) &
	nohup ./$(ADAPTER2) $(IGNORE) &
	$(NETCAT) batch1.doc.ic.ac.uk 7575 < stylesheets/body.css
	$(NETCAT) batch1.doc.ic.ac.uk 7576 < stylesheets/table.css
	$(NETCAT) batch1.doc.ic.ac.uk 7577 < stylesheets/wrapper.css
	$(NETCAT) batch1.doc.ic.ac.uk 7578 < images/favicon.gif

.phony: installpipe
installpipe: $(PIPE1) $(PIPE2) 

$(PIPE1):
	$(NEWFIFO) $@

$(PIPE2):
	$(NEWFIFO) $@

.phony: uninstall
uninstall: remove_main remove_netcat remove_pipe

.phony: remove_main remove_netcat remove_pipe
remove_main:
	-$(KILLNAME) $(ADAPTER1)
	-$(KILLNAME) $(ADAPTER2)

remove_netcat:
	-$(KILLNAME) $(NETCAT)

remove_pipe:
	-$(RM) $(PIPE1) $(PIPE2) 

.phony: clean
clean: uninstall
	echo "not cleaning yet"
