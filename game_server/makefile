CC     = g++
NEWDIR = mkdir
CFLAGS = -Wall -Werror
SRCDIR = src
BINDIR = bin
RM     = rm -rf
TARGET = gameserver
HEADER = 

.phony: all
all: installdirs $(BINDIR)/$(TARGET)

.phony: installdirs
installdirs: $(BINDIR)

$(BINDIR):
	$(NEWDIR) $@

$(BINDIR)/$(TARGET): $(BINDIR)/init.o
	$(CC) $(CFLAGS) $^ -o $@

$(BINDIR)/init.o: $(SRCDIR)/init.cpp $(HEADER)
	$(CC) $(CFLAGS) -c $< -o $@

.phony: check
check: all
	echo "not checking yet"

.phony: install
install: all
	echo "not installing yet"

.phony: uninstall
uninstall:
	echo "not uninstalling yet"

.phony: clean
clean:
	$(RM) $(BINDIR)
