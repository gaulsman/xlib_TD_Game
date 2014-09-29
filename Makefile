
SRC = $(wildcard *.cpp)

run:	Tower
	~/Desktop/Tower

Tower: $(SRC)
	g++ -o ~/Desktop/Tower $(SRC) -L/usr/X11R6/lib -lX11 -lstdc++
