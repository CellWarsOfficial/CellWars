LOG      = >db_log.l 2>&1
RM       = rm -rf
LOCKFILE = .dbprocess.lock

.phony: all
all: build

.phony: build
build: package-lock.json

package-lock.json:
	npm install

.phony: check
check: all
	echo "no checking yet"

.phony: install
install: build uninstall
	npm run bgdb $(LOG)

.phony: uninstall
uninstall:
	-cat $(LOCKFILE) | sed -e 's#^#kill #' | sh && $(RM) $(LOCKFILE)

.phony: clean
clean: uninstall
	$(RM) node_modules package-lock.json $(LOCKFILE)
