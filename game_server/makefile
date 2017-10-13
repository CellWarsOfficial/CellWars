CC     = g++
NEWDIR = mkdir
SRCDIR = src
TSTDIR = test
BINDIR = bin
RM     = rm -rf
TARGET = $(BINDIR)/gameserver
HEADER = $(wildcard $(SRCDIR)/*.hpp)
SOURCE = $(wildcard $(SRCDIR)/*.cpp)
OBJECT = $(patsubst %.cpp,%.o, $(patsubst $(SRCDIR)/%,$(BINDIR)/%, $(SOURCE)))
IP     = localhost
RUNFLG = $$CW_GS_RUNFLAGS
IGNORE = >>run.l 2>&1
EFLAGS = -lssl -lcrypto
CFLAGS = -Wall -Werror -I $(SRCDIR)/ -std=c++11 -O2 -pthread
TFLAGS = 
TESTS  = $(BINDIR)/log_test $(BINDIR)/crank_test $(BINDIR)/block_test $(BINDIR)/misc_test
LOCKFILE = .gsprocess.lock

.phony: all
all: build

.phony: build
build: $(BINDIR) $(TARGET) $(TESTS)

$(BINDIR):
	$(NEWDIR) $@

$(TARGET): $(OBJECT)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@ $(EFLAGS)

$(BINDIR)/%.o: $(SRCDIR)/%.cpp $(HEADER)
	$(CC) $(CFLAGS) -c $< -o $@ $(EFLAGS)

$(BINDIR)/log_test: $(TSTDIR)/log_test.cpp $(BINDIR)/log.o $(BINDIR)/strings.o $(BINDIR)/math.o $(TSTDIR)/log_test.hpp $(HEADER)
	$(CC) $(CFLAGS) $(TFLAGS) $< $(BINDIR)/log.o $(BINDIR)/strings.o $(BINDIR)/math.o -o $@ $(EFLAGS)

$(BINDIR)/crank_test: $(TSTDIR)/crank_test.cpp $(BINDIR)/block.o $(BINDIR)/crank.o $(BINDIR)/malloc_safety.o $(TSTDIR)/crank_test.hpp $(HEADER)
	$(CC) $(CFLAGS) $(TFLAGS) $< $(BINDIR)/block.o $(BINDIR)/crank.o $(BINDIR)/malloc_safety.o -o $@ $(EFLAGS)

$(BINDIR)/block_test: $(TSTDIR)/block_test.cpp $(BINDIR)/block.o $(BINDIR)/malloc_safety.o $(TSTDIR)/block_test.hpp $(HEADER)
	$(CC) $(CFLAGS) $(TFLAGS) $< $(BINDIR)/block.o $(BINDIR)/malloc_safety.o -o $@ $(EFLAGS)

$(BINDIR)/misc_test: $(TSTDIR)/misc_test.cpp $(BINDIR)/math.o $(BINDIR)/strings.o $(BINDIR)/malloc_safety.o $(TSTDIR)/misc_test.hpp $(HEADER)
	$(CC) $(CFLAGS) $(TFLAGS) $< $(BINDIR)/math.o $(BINDIR)/strings.o $(BINDIR)/malloc_safety.o -o $@ $(EFLAGS)

.phony: check
check: build
	./$(BINDIR)/log_test
	./$(BINDIR)/crank_test
	./$(BINDIR)/block_test
	./$(BINDIR)/misc_test
  
.phony: install
install: uninstall build
	nohup ./$(TARGET) $(RUNFLG) $(IGNORE) & echo $$! > $(LOCKFILE)

.phony: uninstall
uninstall:
	-cat $(LOCKFILE) | sed -e 's#^#kill #' | sh
	-while pgrep gameserver > /dev/null; do sleep 1; done
	-$(RM) $(LOCKFILE)

.phony: clean
clean: uninstall
	$(RM) $(BINDIR) $(LOCKFILE)

