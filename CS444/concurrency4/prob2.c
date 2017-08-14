/*
 * Lance Umagat
 * Barbershop Problem 2
 * 5/16/2016
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include <unistd.h>
 #include <pthread.h>
 #include <semaphore.h>
 #include <time.h>
 
 #define thread_count 25
 int chair_count = 5;
 int customer_count = 20;
 
 void * customer_func(void * id);
 void * barber_func();
 void get_hair_cut(long id);
 void cut_hair();

 sem_t waiting_room;	/* # of chairs for customers, Extra customers leaves */
 sem_t chair;			/* Barber serves one at a time */
 sem_t cutting_hair;	/* Cutting one person's hair at a time */
 sem_t sleeping;		/* Barber sleeps when no customers and wakes up when there are customers */
 int finished = 0;		/* When all customers served, barber can go home */

 int main(int argc, char *argv[])
 {
	pthread_t barber;
	pthread_t customer[thread_count];
	long i;

	sem_init(&waiting_room, 0, chair_count);
	sem_init(&chair, 0, 1);
	sem_init(&cutting_hair, 0, 0);
	sem_init(&sleeping, 0, 0);

	pthread_create(&barber, NULL, barber_func, NULL);

	for (i = 0; i < customer_count; i++) {
		pthread_create(&customer[i], NULL, customer_func, (void *)i);
	}

	for (i = 0; i < customer_count; i++) {
		pthread_join(customer[i], NULL);
	}

	finished = 1; 
	sem_post(&sleeping);
	pthread_join(barber, NULL);

	return 0;
 }

 void * customer_func(void *id)
 {
	long tid = (long) id;

	srand(time(NULL) + tid);
	int sleep_time = rand() % 2 + 2;
	sleep(sleep_time);
	
	printf("Customer %lu entered\n", tid);

	get_hair_cut(tid);

	return 0;
 }

 void * barber_func()
 {
	while (!finished) {
		printf("Barber sleeping\n");
		sem_wait(&sleeping);

		cut_hair();
	}

	return 0;
 }

 void get_hair_cut(long tid)
 {
	/* Customer waits if there are no more chairs in the waiting room */
	sem_wait(&waiting_room);
	printf("\tCustomer %lu in waiting room\n", tid);

	/* Wait for the barber's only chair, like only one barber here */
	sem_wait(&chair);

	/* Release a semaphore in the waiting_room */
	sem_post(&waiting_room);

	/* 
	 * Release sleeping semaphore to wake up the barber
	 * At this point call cut_hair
	 */
	printf("\tcustomer %lu waking up the barber\n", tid);
	sem_post(&sleeping);

	/* Customer and barber finishes at the same time, wait for barber to finish */
	sem_wait(&cutting_hair);

	/* Finished haircut, release semaphore to allow others to get a haircut */
	sem_post(&chair);
	printf("customer %lu finished, exiting\n", tid);
 }

 void cut_hair()
 {
	if (!finished) {
		printf("barber cutting hair\n");
		
		srand(time(NULL));
		int sleep_time = rand() % 2 + 2;
		sleep(sleep_time);
		
		printf("barber finished cutting hair\n");

		// allow the customer to get up and leave
		sem_post(&cutting_hair);
	}
	else {
		printf("Barber is done, he is going to go home now, GOODBYE!\n");
	}
 }
 