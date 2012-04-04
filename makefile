OBJS = params.o io.o mesh.o mesh-loader.o main.o 
INCLUDES = headers.h cvec2t.h cvec3t.h cvec4t.h hmatrix.h
CC = g++
CFLAGS = -c
LFLAGS = -lGL -lGLU -lglut

a.out: $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -o a.out

main.o: main.cpp io.o params.o $(INCLUDES)
	$(CC) $(CFLAGS) $<

mesh-loader.o: mesh-loader.cpp mesh-loader.h $(INCLUDES)
	$(CC) $(CFLAGS) $<

params.o: params.cpp params.h
	$(CC) $(CFLAGS) $<

mesh.o: mesh.cpp mesh.h params.o $(INCLUDES)
	$(CC) $(CFLAGS) $<

io.o: io.cpp io.h mesh.o params.o $(INCLUDES)
	$(CC) $(CFLAGS) $<

clean:
	rm -f $(OBJS) a.out *.h.gch
