ADAPTER1 = adapter.sh
ADAPTER2 = sv_adapter.sh
IGNORE   = >/dev/null 2>&1
KILLCOM  = /bin/kill -9
RM       = rm -rf

.phony: all
all:
	echo "no compilation required"

.phony: check
check: all
	echo "not checking yet"

.phony: install
install: all uninstall
	#nohup ./$(ADAPTER1) $(IGNORE) &
	nohup ./$(ADAPTER2) $(IGNORE) &

.phony: uninstall
uninstall: remove_main

.phony: remove_main
remove_main:
	-ps -A | grep $(ADAPTER1) | awk '{print "echo " $$1 ";pgrep -P " $$1}' | sh | sed -e 's#^#/bin/kill -9 #' | sh        

.phony: clean
clean: uninstall
	echo "not cleaning yet"
