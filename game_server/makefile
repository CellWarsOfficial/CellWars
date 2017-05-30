CC      = g++
NEWDIR  = mkdir
SRCDIR  = src
BINDIR  = bin
TESTDIR = test
RM      = rm -rf
TARGET  = $(BINDIR)/gameserver
TEST = $(BINDIR)/test
CFLAGS  = -Wall -Werror -I $(SRCDIR)/ -std=c++11
HEADER  = $(wildcard $(SRCDIR)/*.hpp)
TESTHEADER  = $(wildcard $(TESTDIR)/*.hpp)

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

$(BINDIR)/crank.o: $(SRCDIR)/crank.cpp $(HEADER)
	$(CC) $(CFLAGS) -c $< -o $@

$(TEST): $(BINDIR)/init.o $(BINDIR)/block.o $(BINDIR)/malloc_safety.o $(BINDIR)/crank.o  $(BINDIR)/crank_test.o 
	$(CC) $(CFLAGS) $^ -o $@

$(BINDIR)/crank_test.o: $(TESTDIR)/crank_test.cpp $(TESTHEADER)
	$(CC) $(CFLAGS) -c $< -o $@

.phony: check
check: $(TEST); $(BINDIR)/test 

.phony: install
install: all
	echo "not installing yet"

.phony: uninstall
uninstall:
	echo "not uninstalling yet"

.phony: clean
clean:
	$(RM) $(BINDIR)
