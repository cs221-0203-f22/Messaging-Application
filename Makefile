PROG = project05
OBJS = project05.o

%.o: %.c
	gcc -c -g -o $@ $<

all: $(PROG)

$(PROG): project05.h $(OBJS)
	gcc -g -o project05 project05.c

clean:
	rm -rf $ (PROG) $ (OBJS)
