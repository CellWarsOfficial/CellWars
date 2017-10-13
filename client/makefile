SRCDIR = src
DEVDIR = build
PUBDIR = html
CP     = cp -r
RM     = rm -rf
NFHTML = not_found.html
FIL404 = $(PUBDIR)/$(NFHTML)
SOURCE = $(wildcard $(SRCDIR)/*.hpp)

.phony: all
all: build

.phony: dev
dev: package-lock.json
	npm run start

.phony: build
build: package-lock.json $(DEVDIR) $(FIL404)

$(DEVDIR):
	npm run build

package-lock.json:
	npm install

$(FIL404):
	$(CP) $(SRCDIR)/$(NFHTML) $(DEVDIR)/

.phony: check
check: build
	echo "not checking yet"

.phony: install
install: uninstall build
	$(CP) $(DEVDIR) $(PUBDIR)

.phony: uninstall
uninstall:
	$(RM) $(PUBDIR)

.phony: clean
clean: uninstall
	$(RM) node_modules package-lock.json $(DEVDIR)

