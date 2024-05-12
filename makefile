PROG = ApacheLogToCSV
LIB = ApacheLogToCSV.a
DEF = /usr/local/include
PRG = /usr/local/bin/$(PROG)

FILES = \
	$(LIB)(ApacheLogToCSV.o)

.SILENT:

$(PRG): $(LIB) $(XLIB)
	echo "using gcc to load $(PRG)"
	gcc -o $(PRG) $(LIB) $(XLIB)

$(LIB): $(FILES)

clean:
	rm -f $(LIB)

all:
	make clean
	make

.PRECIOUS: $(LIB)

