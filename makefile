
CFLAGS = -Wall 

PROG = ApacheLogToCSV
DIR = /usr/local/bin
HEADERS = 
OBJECTS = $(PROG).o

default: $(PROG)

%.o: %.c $(HEADERS)
	gcc -c $< -o $@

$(PROG): $(OBJECTS)
	gcc $(OBJECTS) -o $@
	strip $(PROG)
	ls -l $(PROG)

.SILENT:

install:
	mv $(PROG) $(DIR)/$(PROG)

clean:
	-rm -f $(OBJECTS)
	-rm -f $(PROG)

all:
	make clean
	make
