CC     = g++
NEWDIR = mkdir
SRCDIR = src
BINDIR = bin
RM     = rm -rf
TARGET = $(BINDIR)/gameserver
CFLAGS = -Wall -Werror -I $(SRCDIR)/ -std=c++11
HEADER = $(wildcard $(SRCDIR)/*.hpp)
OBJECT = $(BINDIR)/init.o $(BINDIR)/block.o $(BINDIR)/malloc_safety.o $(BINDIR)/database.o $(BINDIR)/game.o $(BINDIR)/server_ui.o $(BINDIR)/log.o

.phony: all
all: installdirs $(TARGET)

.phony: installdirs
installdirs: $(BINDIR)

$(BINDIR):
	$(NEWDIR) $@

$(TARGET): $(OBJECT)
	$(CC) $(CFLAGS) $^ -o $@

$(BINDIR)/init.o: $(SRCDIR)/init.cpp $(HEADER)
	$(CC) $(CFLAGS) -c $< -o $@

$(BINDIR)/block.o: $(SRCDIR)/block.cpp $(HEADER)
	$(CC) $(CFLAGS) -c $< -o $@

$(BINDIR)/malloc_safety.o: $(SRCDIR)/malloc_safety.cpp $(HEADER)
	$(CC) $(CFLAGS) -c $< -o $@

$(BINDIR)/database.o: $(SRCDIR)/database.cpp $(HEADER)
	$(CC) $(CFLAGS) -c $< -o $@

$(BINDIR)/game.o: $(SRCDIR)/game.cpp $(HEADER)
	$(CC) $(CFLAGS) -c $< -o $@

$(BINDIR)/server_ui.o: $(SRCDIR)/server_ui.cpp $(HEADER)
	$(CC) $(CFLAGS) -c $< -o $@

$(BINDIR)/log.o: $(SRCDIR)/log.cpp $(HEADER)
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
