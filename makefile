CC = g++
flags = -IEigen


scene.o: Main.cpp Main.h
	${CC} -c Main.cpp ${flags}


clean :
	rm -f *.o RayTracer

