CC=gcc
# CFLAGS=-Wall -Wextra -L. -lftd2xx -Wl,-rpath /usr/local/lib

# take of Wextra too much
CFLAGS=-Wall -L. -lftd2xx -Wl,-rpath /usr/local/lib


APP = example

all: $(APP)

$(APP): main.c	
	$(CC) main.c -o $(APP) $(CFLAGS)

clean:
	rm -f *.o ; rm $(APP)
