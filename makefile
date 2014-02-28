CC = g++
flags = -I/opt/X11/include

RayTracer: camera.o
	${CC} -o RayTracer camera.o ${flags}
camera.o: camera.cpp camera.h
	${CC} -c camera.cpp 
clean :
	rm -f *.o RayTracer

