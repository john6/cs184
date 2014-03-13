CC = g++
flags = -I./Eigen -I/usr/X11/include
Lflags = -L/usr/X11/lib
libs = -lx11

%.o:%.cpp
	${CC} $(flags) ${flags} -c $< 

main: Main.o 
	${CC} ${flags} ${Lflags} ${libs} $@.o -o $@ 



clean :
	rm -f *.o RayTracer

