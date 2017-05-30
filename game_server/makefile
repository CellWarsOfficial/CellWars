CC     = g++
NEWDIR = mkdir
SRCDIR = src
TSTDIR = test
BINDIR = bin
RM     = rm -rf
TARGET = $(BINDIR)/gameserver
CFLAGS = -Wall -Werror -I $(SRCDIR)/ -std=c++11
EFLAGS = -lpthread
TFLAGS = 
HEADER = $(wildcard $(SRCDIR)/*.hpp) 
OBJECT = $(BINDIR)/init.o $(BINDIR)/block.o $(BINDIR)/malloc_safety.o $(BINDIR)/database.o $(BINDIR)/game.o $(BINDIR)/server_ui.o $(BINDIR)/log.o
TESTS  = $(BINDIR)/log_test

.phony: all
all: installdirs $(TARGET) $(TESTS)

.phony: installdirs
installdirs: $(BINDIR)

$(BINDIR):
	$(NEWDIR) $@

$(TARGET): $(OBJECT)
	$(CC) $(CFLAGS) $^ -o $@ $(EFLAGS)

$(BINDIR)/init.o: $(SRCDIR)/init.cpp $(HEADER)
	$(CC) $(CFLAGS) -c $< -o $@ $(EFLAGS)

$(BINDIR)/block.o: $(SRCDIR)/block.cpp $(HEADER)
	$(CC) $(CFLAGS) -c $< -o $@ $(EFLAGS)

$(BINDIR)/malloc_safety.o: $(SRCDIR)/malloc_safety.cpp $(HEADER)
	$(CC) $(CFLAGS) -c $< -o $@ $(EFLAGS)

$(BINDIR)/database.o: $(SRCDIR)/database.cpp $(HEADER)
	$(CC) $(CFLAGS) -c $< -o $@ $(EFLAGS)

$(BINDIR)/game.o: $(SRCDIR)/game.cpp $(HEADER)
	$(CC) $(CFLAGS) -c $< -o $@ $(EFLAGS)

$(BINDIR)/server_ui.o: $(SRCDIR)/server_ui.cpp $(HEADER)
	$(CC) $(CFLAGS) -c $< -o $@ $(EFLAGS)

$(BINDIR)/log.o: $(SRCDIR)/log.cpp $(HEADER)
	$(CC) $(CFLAGS) -c $< -o $@ $(EFLAGS)

$(BINDIR)/log_test: $(TSTDIR)/log_test.cpp $(BINDIR)/log.o $(TSTDIR)/log_test.hpp $(HEADER)
	$(CC) $(CFLAGS) $(TFLAGS) $< $(BINDIR)/log.o -o $@ $(EFLAGS)

.phony: check
check: all
	./$(BINDIR)/log_test

.phony: install
install: all
	echo "not installing yet"

.phony: uninstall
uninstall:
	echo "not uninstalling yet"

.phony: clean
clean:
	$(RM) $(BINDIR)
