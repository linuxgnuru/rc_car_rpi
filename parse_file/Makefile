PROGNAME = parse_file
INCLUDE = -I../include
DEBUG = -O2
CFLAGS = $(DEBUG) -Wall $(INCLUDE)
LIBS = -lwiringPi
CC = gcc
SRC = $(PROGNAME).c
OBJ =   $(SRC:.c=.o)

ifeq ($(PREFIX),)
	PREFIX := /usr/local
endif

all: clean $(PROGNAME)

$(PROGNAME): $(OBJ)
	@$(CC) -o $@ $(OBJ) $(LIBS)

.c.o:
	@$(CC) -c $(CFLAGS) $< -o $@

#sudo chown root:root $(PROGNAME)

clean:
	rm -f *.o $(PROGNAME)

install: $(PROGNAME)
	sudo install -d $(PREFIX)/bin/
#sudo chown root:root $(PROGNAME)
	sudo install -m 4755 $(PROGNAME) $(PREFIX)/bin/
