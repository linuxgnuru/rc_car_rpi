PROGNAME = run_once
INCLUDE = -I../include
DEBUG = -O2
CFLAGS = $(DEBUG) -Wall $(INCLUDE)
LIBS = -lwiringPi libmyenum.a
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

#$(PROGNAME):$(PROGNAME).o
#	gcc -O2 -Wall -Winline -I$(INCLUDE) $(PROGNAME).c -o $(PROGNAME) -lwiringPi libmyenum.a

clean:
	rm -f *.o $(PROGNAME)

install: $(PROGNAME)
	sudo install -d $(PREFIX)/bin/
	sudo install -m 755 $(PROGNAME) $(PREFIX)/bin/
