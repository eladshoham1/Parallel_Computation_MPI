build:
	mpicc -o static_gcd static_gcd.c gcdFunctions.c
	mpicc -o dynamic_gcd dynamic_gcd.c gcdFunctions.c

clean:
	rm -rf *.o static_gcd dynamic_gcd

static:
	mpiexec -n 4 ./static_gcd < input.txt

dynamic:
	mpiexec -n 4 ./dynamic_gcd < input.txt