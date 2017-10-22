PROGNAME = man_rc

all: clean $(PROGNAME)

$(PROGNAME):$(PROGNAME).o
#gcc -O2 -Wall $(PROGNAME).c -o $(PROGNAME) libmyenum.a libardNS.a -lwiringPi
	gcc -O2 -Wall $(PROGNAME).c -o $(PROGNAME) -lwiringPi -I../include
	sudo chown root:root $(PROGNAME)
	sudo chmod 4755 $(PROGNAME)

clean:
	rm -f *.o $(PROGNAME)
