# ---------- Project Configuration ----------
CC       := gcc
CFLAGS   := -Wall -Wextra -O2
TARGETS  := test test2

OBJS_COMMON := utils.o connect.o usbredir.o

# ---------- Build Rules ----------
all: $(TARGETS)

test: test.o $(OBJS_COMMON)
	$(CC) $(CFLAGS) -o $@ $^

test2: test2.o $(OBJS_COMMON)
	$(CC) $(CFLAGS) -o $@ $^

# Compile .c → .o
%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@

# ---------- Utility ----------
clean:
	rm -f *.o $(TARGETS) libutils.a libconnect.a libusbredir.a *.tmp*

libutils.a: utils.o 
	ar rcs $@ $^

libconnect.a: connect.o
	ar rcs $@ $^

libusbredir.a: usbredir.o
	ar rcs $@ $^

rebuild: clean all

.PHONY: all clean rebuild
