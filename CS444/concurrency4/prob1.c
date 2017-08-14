/* 
 *  Lance Umagat
 *  Barbershop Problem 1
 *	5/15/2016
 */ 
 
 #include <stdio.h>
 #include <stdlib.h>
 #include <unistd.h>
 #include <pthread.h>
 #include <semaphore.h>
 #include <time.h>
 
 #define thread_count 500
 
 void *process_func();
 
 sem_t sem;
 pthread_barrier_t barrier; /* Used to release all three threads at once */
 pthread_cond_t cond;       /* Used to block when three threads are using the resource*/
 pthread_mutex_t lock;
 int active = 0;	    /* Used to figure out # of active threads */
 int full = 0;		    /* Used to check if all active threads are in used */
 
 int main()
 {
	pthread_t process[thread_count];
	
	long i;
	
	sem_init(&sem, 0, 3);
	pthread_barrier_init(&barrier, NULL, 3);
	pthread_cond_init(&cond, NULL);
    	pthread_mutex_init(&lock, NULL);
	srand(time(NULL));
	
	for ( i = 0; i < thread_count; i++ )
	{
		pthread_mutex_lock(&lock);

		/* Blocks until all three resources are released */
		while (full || active >= 3) {
			pthread_cond_wait(&cond, &lock);
		}
		pthread_mutex_unlock(&lock);

		if ( pthread_create(&process[i], NULL, process_func, (void *)i) )
		{
			fprintf(stderr, "Error on process thread creation\n");
			return 1;
		}
	}
	
	for ( i = 0; i < thread_count; i++ )
	{
		if ( pthread_join(process[i], NULL) )
		{
			fprintf(stderr, "Error on joining process creation\n");
			return 2;
		}
	}
	
	sem_destroy(&sem);
	
	return 0;
 }
 
 void *process_func(void *id)
 {
	long tid = (long) id;
	srand(time(NULL) + tid);
		
	printf("thread %lu: running\n", tid);
		
	sem_wait(&sem);
		
	printf("\tthread %lu: obtained semaphore\n", tid);
		
	pthread_mutex_lock(&lock);
	active++;
	
	/* There are three active, start blocking */		
	if (active >= 3) {
		full = 1;
	}
		
	pthread_cond_signal(&cond);
	pthread_mutex_unlock(&lock);
		
	printf("\t\tthread %lu: using the resource\n", tid);
		
	int sleep_time = rand() % 2 + 2;
	//printf("thread %lu: working for %d seconds\n\n", sleep_time);
	sleep(sleep_time);
		
	printf("\t\tthread %lu: released resource\n", tid);
	
	/* Barrier blocks until all three processes are completed then release them all */	
	pthread_barrier_wait(&barrier);
	//printf("\t\tthread %lu: passed barrier\n", tid);
		
	pthread_mutex_lock(&lock);
	active--;

	/* When the last process is completed release the block */
	if (active == 0) {
		full = 0;
	}

	/* Release the condition for all threads that are waiting */
	pthread_cond_broadcast(&cond);
	pthread_mutex_unlock(&lock);  
		
	printf("\tthread %lu: released semaphore\n", tid);
	sem_post(&sem);
	
	printf("thread %lu: exiting\n", tid);	
	pthread_exit(0);
 }
		
 
 
