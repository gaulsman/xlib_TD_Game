
SRC = $(wildcard *.cpp)

run:	Tower
	./Tower

Tower: $(SRC)
	g++ -o ./Tower $(SRC) -L/usr/X11R6/lib -lX11 -lstdc++
