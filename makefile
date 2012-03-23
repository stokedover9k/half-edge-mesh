OBJS = main.o
CC = g++
CFLAGS = -c
LFLAGS = -lGL -lGLU -lglut

a.out: $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -o a.out

main.o: main.cpp input.h draw.h params.h
	$(CC) $(CFLAGS) $<

clean:
	rm -f $(OBJS) a.out *.h.gch
