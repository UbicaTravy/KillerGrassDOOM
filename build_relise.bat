@echo off
windres resources.rc -o resources.o
g++ -O3 -Wall -Wextra -static -mwindows -o killer_grass_doom.exe main.o game.o render.o trig_tables.o resources.o -lgdi32 -lwinmm
pause