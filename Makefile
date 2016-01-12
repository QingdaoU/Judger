CC ?= gcc
PREFIX ?= /usr/local

libexecwhitelist.so: libexecwhitelist.c
	$(CC) $^ $(CFLAGS) -fPIC -ldl -shared -lseccomp -o $@

clean:
	rm -f libexecwhitelist.so

install: libexecwhitelist.so
	install -m555 -oroot -groot -s libexecwhitelist.so $(PREFIX)/lib/libexecwhitelist.so
