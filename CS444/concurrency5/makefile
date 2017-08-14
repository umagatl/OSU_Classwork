all:		smokersproblem

smokersproblem:	smoke.o
	gcc smoke.o -o smoke -pthread

smokersproblem.o: smoke.c
	gcc -c smoke.c

clean:
	rm smoke.o smoke