PKG_CONFIG = pkg-config
CFLAGS = `$(PKG_CONFIG) --cflags gtk+-3.0`
LDFLAGS = `$(PKG_CONFIG) --libs gtk+-3.0`
$(PROGRAM): $(PROGRAM).o
	$(CC) -o $@ $@.o $(LDFLAGS)
%.o: %.c
	$(CC) $(CFLAGS) -c $<
