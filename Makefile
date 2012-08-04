PROGRAM = crc32
.PHONY: all clean install
FILES = main.c

CFLAGS += -Wall -Werror -ansi -pedantic
LDFLAGS +=
PREFIX ?= /usr/local

all: $(PROGRAM)

$(PROGRAM): $(FILES)
	$(CC) -o $@ $(CFLAGS) $(LDFLAGS) $(FILES)

clean:
	-rm -f $(PROGRAM)

install:
	cp $(PROGRAM) $(PREFIX)/bin

