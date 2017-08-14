/*
	Lance Umagat
	Concurrency 2
*/

#include "rdrand.h"
 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define thread_count 5

void *philosopher_func(void *id);
void think(long id);
void get_forks(long id);
void eat(long id);
void put_forks(long id);
long other_fork(long id);

sem_t forks[thread_count];
const char *philosopher[thread_count] = {"Plato", "Aristotle", "Socrates", "John Locke", "Ludwig Wittgenstein"};

int main()
{
	pthread_t threads[thread_count];
	long i;
	
	for (i = 0; i < thread_count; i++)
	{
		sem_init(&forks[i], 1, 1);
	}
	
	for (i = 0; i < thread_count; i++)
	{		
		if (pthread_create(&threads[i], NULL, philosopher_func, (void *)i))
		{
			fprintf(stderr, "Error on philosopher creation\n");
			return 1;
		}
	}
	
	for (i = 0; i < thread_count; i++)
	{
		if ( pthread_join(threads[i], NULL) )
		{
			fprintf(stderr, "Error on joining philosopher thread");
			return 2;
		}
	}
	
	return 0;
}

long other_fork(long id)
{
	if (id == 0)
		return 4;
	else if (id == 1)
		return 0;
	else if (id == 2)
		return 1;
	else if (id == 3)
		return 2;
	else if (id == 4)
		return 3;
	
	return 0;
}

void *philosopher_func(void *id)
{
	long tid = (long) id;
	printf("Philosopher %s has joined the table\n", philosopher[tid]);
	
	while (1)
	{
		think(tid);
		get_forks(tid);
		eat(tid);
		put_forks(tid);
	}
	
	pthread_exit(0);
}

void think(long id)
{
	time_t t;
	
	init_genrand(time(&t));
	long sleep_time = genrand_int32() % 20 + 1;
	printf("Philosopher %s is thinking for %ld\n\n", philosopher[id], sleep_time);
	sleep(sleep_time);
}

void get_forks(long id)
{
	sem_wait(&forks[id]);
	sem_wait(&forks[other_fork(id)]);
	printf("%s picked up forks\n", philosopher[id]);
}

void eat(long id)
{
	time_t t;
	
	init_genrand(time(&t));
	long sleep_time = genrand_int32() % 8 + 2;
	printf("Philosopher %s is eating for %ld\n\n", philosopher[id], sleep_time);
	sleep(sleep_time);
}

void put_forks(long id)
{
	sem_post(&forks[id]);
	sem_post(&forks[other_fork(id)]);
	printf("%s put down forks\n", philosopher[id]);
}


	
