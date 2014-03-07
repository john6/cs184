CC = g++
flags = -IEigen


scene.o: Main.cpp Main.h
	${CC} ${flags} Main.cpp -o Main ${flags}



clean :
	rm -f *.o RayTracer

