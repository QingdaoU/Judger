CC ?= gcc
PREFIX ?= /usr/local

sandbox.so: sandbox.c
	$(CC) $^ $(CFLAGS) -fPIC -ldl -shared -lseccomp -o $@

clean:
	rm -f sandbox.so
