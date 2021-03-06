CC     = g++
NEWDIR = mkdir
SRCDIR = src
TSTDIR = test
BINDIR = bin
SKDIR  = ../socket.io-client-cpp/build/lib/Release
RM     = rm -rf
TARGET = $(BINDIR)/gameserver
HEADER = $(wildcard $(SRCDIR)/*.hpp)
IP     = localhost
RUNFLG = -db $(IP) -w 10 -log log.l -q
IGNORE = >/dev/null 2>&1
KILLCOM = /bin/kill -9
EFLAGS = -lssl -lcrypto
OS := $(shell uname)
ifeq ($(OS), Darwin)
	CFLAGS = -Wall -Werror -I $(SRCDIR)/ -std=c++11
else
	CFLAGS = -Wall -Werror -I $(SRCDIR)/ -std=c++11 -pthread
endif
TFLAGS = 
OBJECT = $(BINDIR)/init.o $(BINDIR)/block.o $(BINDIR)/malloc_safety.o $(BINDIR)/database.o $(BINDIR)/game.o $(BINDIR)/server_ui.o $(BINDIR)/log.o $(BINDIR)/crank.o $(BINDIR)/server.o $(BINDIR)/math.o $(BINDIR)/strings.o
TESTS  = $(BINDIR)/log_test $(BINDIR)/crank_test $(BINDIR)/block_test $(BINDIR)/misc_test
LIBS = $(wildcard $(SKDIR)/*.a) # /usr/lib/x86_64-linux-gnu/libcrypto.a

.phony: all
all: installdirs $(TARGET) $(TESTS)

.phony: installdirs
installdirs: $(BINDIR)

$(BINDIR):
	$(NEWDIR) $@

$(TARGET): $(OBJECT)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@ $(EFLAGS)

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

$(BINDIR)/crank.o: $(SRCDIR)/crank.cpp $(HEADER)
	$(CC) $(CFLAGS) -c $< -o $@ $(EFLAGS)

$(BINDIR)/server.o: $(SRCDIR)/server.cpp $(HEADER)
	$(CC) $(CFLAGS) -c $< -o $@ $(EFLAGS)

$(BINDIR)/math.o: $(SRCDIR)/math.cpp $(HEADER)
	$(CC) $(CFLAGS) -c $< -o $@ $(EFLAGS)

$(BINDIR)/strings.o: $(SRCDIR)/strings.cpp $(HEADER)
	$(CC) $(CFLAGS) -c $< -o $@ $(EFLAGS)

$(BINDIR)/log_test: $(TSTDIR)/log_test.cpp $(BINDIR)/log.o $(TSTDIR)/log_test.hpp $(HEADER)
	$(CC) $(CFLAGS) $(TFLAGS) $< $(BINDIR)/log.o -o $@ $(EFLAGS)

$(BINDIR)/crank_test: $(TSTDIR)/crank_test.cpp $(BINDIR)/block.o $(BINDIR)/crank.o $(BINDIR)/malloc_safety.o $(TSTDIR)/crank_test.hpp $(HEADER)
	$(CC) $(CFLAGS) $(TFLAGS) $<  $(BINDIR)/block.o $(BINDIR)/crank.o $(BINDIR)/malloc_safety.o -o $@ $(EFLAGS)

$(BINDIR)/block_test: $(TSTDIR)/block_test.cpp $(BINDIR)/block.o $(BINDIR)/malloc_safety.o $(TSTDIR)/block_test.hpp $(HEADER)
	$(CC) $(CFLAGS) $(TFLAGS) $< $(BINDIR)/block.o $(BINDIR)/malloc_safety.o -o $@ $(EFLAGS)

$(BINDIR)/misc_test: $(TSTDIR)/misc_test.cpp $(BINDIR)/math.o $(BINDIR)/strings.o $(BINDIR)/malloc_safety.o $(TSTDIR)/misc_test.hpp $(HEADER)
	$(CC) $(CFLAGS) $(TFLAGS) $< $(BINDIR)/math.o $(BINDIR)/strings.o $(BINDIR)/malloc_safety.o -o $@ $(EFLAGS)

.phony: check
check: all
	./$(BINDIR)/log_test
	./$(BINDIR)/crank_test
	./$(BINDIR)/block_test
	./$(BINDIR)/misc_test
  
.phony: install
install: all uninstall
	nohup ./$(TARGET) $(RUNFLG) $(IGNORE) &

.phony: uninstall
uninstall:
	-ps -A | grep gameserver | awk '{print "echo " $$1 ";pgrep -P " $$1}' | sh | sed -e 's#^#/bin/kill -2 #' | sh
	-while pgrep gameserver > /dev/null; do sleep 1; done
	-sleep 60

.phony: clean
clean:
	$(RM) $(BINDIR)
