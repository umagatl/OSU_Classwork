//Christopher Nguyen
//rdrand test

#include "rdrand.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>


int main()
{
	unsigned long num;
	int i;
	time_t t;
	
	init_genrand(time(&t));
	for (i = 0; i < 5; i++)
	{
		num = genrand_int32() % 5;
		printf("Number is %lu\n", num);
	}
	
	return 0;
}