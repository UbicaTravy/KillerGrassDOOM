CC = g++
CFLAGS = -O3 -Wall -Wextra
LDFLAGS = -lgdi32 -lwinmm

all: killer_grass_doom.exe

killer_grass_doom.exe: main.o game.o render.o
	$(CC) -o $@ $^ $(LDFLAGS)

main.o: main.cpp game.h render.h trig_tables.h
	$(CC) $(CFLAGS) -c main.cpp

game.o: game.cpp game.h trig_tables.h
	$(CC) $(CFLAGS) -c game.cpp

render.o: render.cpp render.h trig_tables.h
	$(CC) $(CFLAGS) -c render.cpp

clean:
	del /F /Q *.o killer_grass_doom.exe 2>nul