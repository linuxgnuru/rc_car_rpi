PROGNAME = enum_2_char
INCLUDE = -I../include
DEBUG = -O2
CFLAGS = $(DEBUG) -Wall $(INCLUDE)
LIBS = -lwiringPi
CC = gcc
SRC = $(PROGNAME).c
OBJ =   $(SRC:.c=.o)

all: clean $(PROGNAME)

$(PROGNAME): $(OBJ)
	@$(CC) -o $@ $(OBJ) $(LIBS)

.c.o:
	@$(CC) -c $(CFLAGS) $< -o $@

#sudo chown root:root $(PROGNAME)
#	sudo chmod 4755 $(PROGNAME)

clean:
	rm -f *.o $(PROGNAME)

