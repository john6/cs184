CC = g++
flags = -I./Eigen -I/opt/X11/include
Lflags = -L/opt/X11/lib
libs = -lx11

%.o:%.cpp
	${CC} $(flags) ${flags} -c $< 

Main: Main.o 
	${CC} ${flags} ${Lflags} ${libs} $@.o -o $@ 



clean :
	rm -f *.o RayTracer

