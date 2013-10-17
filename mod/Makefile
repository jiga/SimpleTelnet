WARN := -W -Wall -Wstrict-prototypes -Wmissing-prototypes
KINC := -I/lib/modules/`uname -r`/build/include
CFLAGS := -O2 -DMODULE -D__KERNEL__ ${KINC}
CC := gcc

all: tmod.o 

tmod.o :  tmod.c tmod.h 
	${CC} ${CFLAGS} -c tmod.c -o tmod.o
	
clean:
	rm -f *.o  

i:
	insmod ./tmod.o

u:
	rmmod tmod
