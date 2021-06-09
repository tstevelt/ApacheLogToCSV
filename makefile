
CFLAGS = -Wall 

PROG = ApacheLogToCSV

PRG = /usr/local/bin/$(PROG)
HEADERS = 
OBJECTS = $(PROG).o

default: $(PRG)

%.o: %.c $(HEADERS)
	gcc -c $< -o $@

$(PRG): $(OBJECTS)
	gcc $(OBJECTS) -o $@
	strip $(PRG)
	ls -l $(PRG)

.SILENT:

clean:
	-rm -f $(OBJECTS)
	-rm -f $(PRG)

all:
	make clean
	make

