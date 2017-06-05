CC     = g++
NEWDIR = mkdir
SRCDIR = src
TSTDIR = test
BINDIR = bin
RM     = rm -rf
TARGET = $(BINDIR)/gameserver
HEADER = $(wildcard $(SRCDIR)/*.hpp)
IP     = 146.169.45.167
RUNFLG = -db $(IP) -w 10 -log log.l -q
IGNORE = >/dev/null 2>&1
KILLNAME = pkill
OS := $(shell uname)
ifeq ($(OS), Darwin)
	CFLAGS = -Wall -Werror -I $(SRCDIR)/ -std=c++11
else
	CFLAGS = -Wall -Werror -I $(SRCDIR)/ -std=c++11 -pthread
endif
TFLAGS = 
OBJECT = $(BINDIR)/init.o $(BINDIR)/block.o $(BINDIR)/malloc_safety.o $(BINDIR)/database.o $(BINDIR)/game.o $(BINDIR)/server_ui.o $(BINDIR)/log.o $(BINDIR)/crank.o
TESTS  = $(BINDIR)/log_test $(BINDIR)/crank_test

.phony: all
all: installdirs $(TARGET) $(TESTS)

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

$(BINDIR)/crank.o: $(SRCDIR)/crank.cpp $(HEADER)
	$(CC) $(CFLAGS) -c $< -o $@

$(BINDIR)/log_test: $(TSTDIR)/log_test.cpp $(BINDIR)/log.o $(TSTDIR)/log_test.hpp $(HEADER)
	$(CC) $(CFLAGS) $(TFLAGS) $< $(BINDIR)/log.o -o $@

$(BINDIR)/crank_test: $(TSTDIR)/crank_test.cpp $(BINDIR)/block.o $(BINDIR)/crank.o $(BINDIR)/malloc_safety.o $(TSTDIR)/crank_test.hpp $(HEADER)
	$(CC) $(CFLAGS) $(TFLAGS) $<  $(BINDIR)/block.o $(BINDIR)/crank.o $(BINDIR)/malloc_safety.o -o $@

.phony: check
check: all
	./$(BINDIR)/log_test
	./$(BINDIR)/crank_test

.phony: install
install: all uninstall
	nohup ./$(TARGET) $(RUNFLG) $(IGNORE) &

.phony: uninstall
uninstall:
	-$(KILLNAME) -SIGKILL gameserver #faster closing time, but risky

.phony: clean
clean:
	$(RM) $(BINDIR)
