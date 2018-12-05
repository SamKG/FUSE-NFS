BINDIR=bin
SRCDIR=src
INCDIR=$(SRCDIR)/include
ODIR=$(SRCDIR)/obj

CC=gcc
CFLAGS=-I $(INCDIR) -Wfatal-errors -Wall -g -std=c11
DEPS=$(ODIR)/processpool.o $(ODIR)/queue.o $(ODIR)/sharedmem.o $(ODIR)/threadpool.o $(ODIR)/filehandler.o $(ODIR)/mapred_def.o $(ODIR)/helper.o

LIBS=-lpthread -lrt

$(ODIR)/%.o: $(SRCDIR)/%.c 
	$(CC) -c -o $@ $< $(CFLAGS) $(LIBS)

mapred: $(ODIR)/mapred.o $(DEPS)
	$(CC) $^ -o $(BINDIR)/$@ $(CFLAGS) $(LIBS)

test: $(ODIR)/test.o $(DEPS)
	$(CC) $^ -o $(BINDIR)/$@ $(CFLAGS) $(LIBS)
	
.PHONY: clean
clean:
	rm -f $(ODIR)/*.o $(BINDIR)/*
