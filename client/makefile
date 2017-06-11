NEWDIR = mkdir
RM     = rm -rf
CDIR   = html
NPMBLD = npm run build
CD     = cd
CP     = cp -r
NFHTML = not_found.html
FILE404 = $(CDIR)/$(NFHTML)

.phony: all
all: makedir reactf $(FILE404)

.phony: makedir
makedir: $(CDIR)

$(CDIR):
	$(NEWDIR) $@

.phony: reactf
reactf:
	-$(CD) ../react/my-app/ && $(NPMBLD) && $(CP) build/* ../../client/$(CDIR)/

$(FILE404):
	$(CP) not_found.html $(CDIR)/

.phony: check
check: all
	echo "not checking yet"

.phony: install
install: all
	echo "not installing yet"

.phony: uninstall
uninstall:
	echo "not uninstalling yet."

.phony: clean
clean: uninstall
	$(RM) $(CDIR)

