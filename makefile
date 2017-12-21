make: shit.cpp
	gcc shit.cpp -std=c++11 -framework GLUT -framework OpenGL -framework Cocoa -o planets
clean: planets
	rm planets

