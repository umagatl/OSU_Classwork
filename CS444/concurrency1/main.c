/* Christopher Nguyen
 * 10/7/2015
 * Concurrency 1
 */
 #include "rdrand.h"
 
 #include <stdio.h>
 #include <stdlib.h>
 #include <unistd.h>
 #include <pthread.h>
 
 #define thread_count 50
 void *producer_function(void *buf);
 void *consumer_function(void *buf);
 
 pthread_mutex_t mutex;
 pthread_cond_t cond_p;
 pthread_cond_t cond_c;
 
 typedef struct 
 {
	 int value;
	 int wait_sec;
 } item;
 
 typedef struct
 {
	item array[32];
	int item_limit;
	int item_count;
 } buffer;
 
 int main()
 {
	buffer buf;
	buf.item_limit = 32;
	buf.item_count = 0;
	
	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&cond_p, NULL);
	pthread_cond_init(&cond_c, NULL);
	
	pthread_t producer[thread_count];
	pthread_t consumer[thread_count];
	
	int i;
	
	for ( i = 0; i < thread_count; i++ )
	{
		if ( pthread_create(&producer[i], NULL, producer_function, &buf) )
		{
			fprintf(stderr, "Error on producer thread creation\n");
			return 1;
		}
		
		if ( pthread_create(&consumer[i], NULL, consumer_function, &buf) )
		{
			fprintf(stderr, "Error on consumer thread creation\n");
			return 2;
		}
	}
	
	for ( i = 0; i < thread_count; i++ )
	{
		if ( pthread_join(producer[i], NULL) )
		{
			fprintf(stderr, "Error on joining producer creation\n");
			return 3;
		}
		
		if ( pthread_join(consumer[i], NULL) )
		{
			fprintf(stderr, "Error on joining consumer thread");
			return 4;
		}
	}
	
	//Print buffer to check
	if (buf.item_count == 0)
	{
		printf("\nThe buffer is empty\n");
	} 
	else
	{
		int i;
		
		for (i = 0; i < buf.item_count; i++)
		{
			printf("\nValue: %d\tWait Time: %d", buf.array[i].value, buf.array[i].wait_sec);
		}
	}
	
	// Cleans up all of the mutex and condition variables
	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&cond_p);
	pthread_cond_destroy(&cond_c);
	
	return 0;
 }
 
 void *consumer_function(void *buf)
 {
	buffer *b = (buffer *)buf;
	time_t t;
	
	init_genrand(time(&t));
			
	pthread_mutex_lock(&mutex);
	
	while (b->item_count == 0)
	{
		pthread_cond_wait(&cond_c, &mutex);
	}
	
	if (b->item_count != 0)
	{
		printf("Consumed\tValue: %d\tWait Time: %d\n\n", b->array[b->item_count-1].value, b->array[b->item_count-1].wait_sec);
		//printf("Consumers sleeping for %d seconds\n", b->array[b->item_count-1].wait_sec);
		sleep(b->array[b->item_count-1].wait_sec);
		//printf("Consumers consumed value %d\n", b->array[b->item_count-1].value);
		b->array[b->item_count-1].value = 0;
		b->array[b->item_count-1].wait_sec = 0;
		b->item_count--;
	}
	
	pthread_cond_signal(&cond_p);
	pthread_mutex_unlock(&mutex);
	
	pthread_exit(0);
 }
 
 void *producer_function(void *buf)
 {
	buffer *b = (buffer *)buf;
	time_t t;
	
	init_genrand(time(&t));
			
	pthread_mutex_lock(&mutex);
	
	while (b->item_count == b->item_limit)
	{
		pthread_cond_wait(&cond_p, &mutex);
	}
	
	if (b->item_count < b->item_limit)
	{
		b->item_count++;
		b->array[b->item_count-1].value = genrand_int32() % 10 + 1;
		b->array[b->item_count-1].wait_sec = genrand_int32() % 8 + 2;
		printf("Produced\tValue: %d\tWait Time: %d\n", b->array[b->item_count-1].value, b->array[b->item_count-1].wait_sec);
		//printf("Producer sleeping for %d seconds\n", b->array[b->item_count-1].wait_sec);
		int sleep_time = genrand_int32() % 5 + 3;
		printf("Producer sleeps for %d\n\n", sleep_time);
		sleep(sleep_time);
	}
	
	pthread_cond_signal(&cond_c);
	pthread_mutex_unlock(&mutex);
	
	pthread_exit(0);
 }
 
