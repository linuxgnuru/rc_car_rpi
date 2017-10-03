PROGNAME = send_command

all: clean $(PROGNAME)

$(PROGNAME):$(PROGNAME).o
	gcc -O2 -Wall $(PROGNAME).c -o $(PROGNAME) -lwiringPi

clean:
	rm -f *.o $(PROGNAME)
