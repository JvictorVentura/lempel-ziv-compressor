CFLAGS=-Wall -Wextra
DEBUG=-g

make:
	gcc *.c $(CFLAGS) -o lempel-ziv

debug:
	gcc *.c $(DEBUG) $(CFLAGS) -o lempel-ziv_debug



