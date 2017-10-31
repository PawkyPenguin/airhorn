LIBS = $(shell pkg-config --cflags --libs x11-xcb x11 xcb-aux xcb-image)
CFLAGS= -I.
DEPS = airhorn.h crosshair.c
OBJ = airhorn.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)
airhorn.exe: $(OBJ)
	$(CC) $(LIBS) -o  $@ $^ $(CFLAGS)

