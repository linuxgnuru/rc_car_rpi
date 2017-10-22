PROGNAME = rc_web
ifeq ($(PREFIX),)
	PREFIX := /usr/local
endif

all: clean $(PROGNAME)

$(PROGNAME):$(PROGNAME).o
#gcc -O2 -Wall $(PROGNAME).c -o $(PROGNAME) libmyenum.a libardNS.a -lwiringPi
	gcc -O2 -Wall $(PROGNAME).c -o $(PROGNAME) libmyenum.a -lwiringPi -I../include

clean:
	rm -f *.o $(PROGNAME)

install: $(PROGNAME)
	sudo install -d $(PREFIX)/bin/
	sudo install -m 755 $(PROGNAME) $(PREFIX)/bin/
