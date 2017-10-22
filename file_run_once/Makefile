PROGNAME = file_run_once

all: clean $(PROGNAME)

$(PROGNAME):$(PROGNAME).o
	gcc -O2 -Wall $(PROGNAME).c -o $(PROGNAME) -lwiringPi -I../include

clean:
	rm -f *.o $(PROGNAME)
