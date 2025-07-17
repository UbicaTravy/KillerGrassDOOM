CC = g++
CFLAGS = -O3 -Wall -Wextra
LDFLAGS = -lgdi32 -lwinmm

all: killer_grass_doom.exe

killer_grass_doom.exe: main.o game.o render.o trig_tables.o
	g++ -o killer_grass_doom.exe main.o game.o render.o trig_tables.o -lgdi32 -lwinmm

main.o: main.cpp
	g++ -O3 -Wall -Wextra -c main.cpp

game.o: game.cpp
	g++ -O3 -Wall -Wextra -c game.cpp

render.o: render.cpp
	g++ -O3 -Wall -Wextra -c render.cpp

trig_tables.o: trig_tables.cpp
	g++ -O3 -Wall -Wextra -c trig_tables.cpp

clean:
	del *.o killer_grass_doom.exe