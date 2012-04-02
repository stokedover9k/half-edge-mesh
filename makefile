OBJS = params.o io.o mesh.o mesh-loader.o main.o 
CC = g++
CFLAGS = -c
LFLAGS = -lGL -lGLU -lglut

a.out: $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -o a.out

main.o: main.cpp io.o params.o
	$(CC) $(CFLAGS) $<

mesh-loader.o: mesh-loader.cpp mesh-loader.h
	$(CC) $(CFLAGS) $<

params.o: params.cpp params.h
	$(CC) $(CFLAGS) $<

mesh.o: mesh.cpp mesh.h params.o
	$(CC) $(CFLAGS) $<

io.o: io.cpp io.h params.o
	$(CC) $(CFLAGS) $<

clean:
	rm -f $(OBJS) a.out *.h.gch
