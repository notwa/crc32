PROGRAM = crc32
.PHONY: all clean install

all: $(PROGRAM)

$(PROGRAM): main.c
	$(CC) -o $@ -Wall -Winline -std=gnu11 $(CFLAGS) $(LDFLAGS) $^

clean:
	-rm -f $(PROGRAM)

install:
	install -m 0755 -d $(PREFIX)/bin
	install -m 0755 $(PROGRAM) $(PREFIX)/bin
