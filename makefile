build:
	mpicc -o static_gcd static_gcd.c gcdNumbers.c
	mpicc -o dynamic_gcd dynamic_gcd.c gcdNumbers.c

clean:
	rm -rf *.o static_gcd dynamic_gcd