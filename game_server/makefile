CC     = g++
NEWDIR = mkdir
SRCDIR = src
BINDIR = bin
RM     = rm -rf
TARGET = $(BINDIR)/gameserver
CFLAGS = -Wall -Werror -I $(SRCDIR)/ -std=c++11
HEADER = $(wildcard $(SRCDIR)/*.hpp)

.phony: all
all: installdirs $(TARGET)

.phony: installdirs
installdirs: $(BINDIR)

$(BINDIR):
	$(NEWDIR) $@

$(TARGET): $(BINDIR)/init.o $(BINDIR)/block.o $(BINDIR)/malloc_safety.o $(BINDIR)/crank.o
	$(CC) $(CFLAGS) $^ -o $@

$(BINDIR)/init.o: $(SRCDIR)/init.cpp $(HEADER)
	$(CC) $(CFLAGS) -c $< -o $@

$(BINDIR)/block.o: $(SRCDIR)/block.cpp $(HEADER)
	$(CC) $(CFLAGS) -c $< -o $@

$(BINDIR)/malloc_safety.o: $(SRCDIR)/malloc_safety.cpp $(HEADER)
	$(CC) $(CFLAGS) -c $< -o $@

$(BINDIR)/malloc_safety.o: $(SRCDIR)/crank.cpp $(HEADER)
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
