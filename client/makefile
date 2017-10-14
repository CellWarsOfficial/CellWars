SRCDIR = src
DEVDIR = build
PUBDIR = html
MAGICF = .finished # required to distinguish build target and build folder
CP     = cp -r
RM     = rm -rf
SOURCE = $(shell find $(SRCDIR))

.phony: all
all: build

.phony: dev
dev: package-lock.json
	npm run start

.phony: build
build: package-lock.json $(DEVDIR)/$(MAGICF)

$(DEVDIR)/$(MAGICF): $(SOURCE)
	npm run build

package-lock.json:
	npm install

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

