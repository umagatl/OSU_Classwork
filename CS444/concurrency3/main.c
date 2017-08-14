/* Lance Umagat
 * 5/3/2016
 * Concurrency 3
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include <unistd.h>
 #include <pthread.h>
 #include <semaphore.h>
 #include <time.h>
 
 #define thread_count 5
 
 void *search_func(void *id);
 void *insert_func(void *id);
 void *delete_func(void *id);
 
 sem_t sem_s[thread_count];
 sem_t sem_i;
 sem_t sem_d;
 
 typedef struct node {
	int data;
	struct node * next;
 } node_t;

 node_t *list = NULL;
 int list_count = 0;
 
 int main()
 {
	pthread_t searchers[thread_count];
	pthread_t inserters[thread_count];
	pthread_t deleters[thread_count];
	
	long i;
	
	sem_init(&sem_i, 0, 1);
	sem_init(&sem_d, 0, 1);
	
	srand(time(NULL));
	
	for ( i = 0; i < thread_count; i++ )
	{
		sem_init(&sem_s[i], 0, 1);
	}
	
	for ( i = 0; i < thread_count; i++ )
	{
		if ( pthread_create(&searchers[i], NULL, search_func, (void *)i) )
		{
			fprintf(stderr, "Error on searcher thread creation\n");
			return 1;
		}
	}
	
	for ( i = 0; i < thread_count; i++ ) 
	{
		if ( pthread_create(&inserters[i], NULL, insert_func, (void *)i) )
		{
			fprintf(stderr, "Error on inserter thread creation\n");
			return 2;
		}
	}
	
	for ( i = 0; i < thread_count; i++ ) 
	{
		if ( pthread_create(&deleters[i], NULL, delete_func, (void *)i) )
		{
			fprintf(stderr, "Error on deleter thread creation\n");
			return 3;
		}
	}
	
	for ( i = 0; i < thread_count; i++ )
	{
		if ( pthread_join(searchers[i], NULL) )
		{
			fprintf(stderr, "Error on joining searcher creation\n");
			return 4;
		}
		
		if ( pthread_join(inserters[i], NULL) )
		{
			fprintf(stderr, "Error on joining inserter thread");
			return 5;
		}
		
		if ( pthread_join(deleters[i], NULL) )
		{
			fprintf(stderr, "Error on joining deleter thread\n");
			return 6;
		}
	}
	
	sem_destroy(&sem_i);
	sem_destroy(&sem_d);
	
	for ( i = 0; i < thread_count; i++ )
	{
		sem_destroy(&sem_s[i]);
	}
	
	return 0;
 }
 
 void print(node_t *list)
 {
	node_t *curr = list;
	
	printf("List: ");
    while (curr != NULL) {
        printf("[%d]", curr->data);
        curr = curr->next;
    }
    printf("\n");
 }
 
 void *search_func(void *id)
 {
	long tid = (long) id;
	srand(time(NULL) + tid);
	
	while(1)
	{	
		sem_wait(&sem_s[tid]);
		int value = rand() % 10;
		
		node_t * temp = list;
		int valid = 0;
		
		//Search for the number
		while(temp != NULL) {
			if (temp->data == value) {
			   valid = 1;
			   break;
			} else {
				temp = temp->next;
			}
		}
		
		printf("Searcher #%lu for number: %d, result: %d\n", tid, value, valid);
	
		int sleep_time = rand() % 2 + 1;
		printf("Searching for %d seconds\n\n", sleep_time);
		sleep(sleep_time);
		
		sem_post(&sem_s[tid]);
		sleep(1);
	}
 
	pthread_exit(0);
 }
 
 void *insert_func(void *id)
 {
	long tid = (long) id;
	srand(time(NULL) + tid);
	
	int value = rand() % 10;
	
	while(1)
	{
		sem_wait(&sem_i);
		
		//Insert Node
		if (list == NULL) {
			node_t * new = malloc(sizeof(node_t));
			
			if(new == NULL) {
				printf("Node creation failed\n");
			}  
			
			new->data = value;
			new->next = NULL;
			
			list = new;
			
			list_count++;
		}
		else {
			node_t * temp = list;
			
			while (temp->next != NULL) {
				temp = temp->next;
			}
			
			node_t * new = malloc(sizeof(node_t));

			if(new == NULL) {
				printf("Node creation failed\n");
			}  
			
			new->data = value;
			new->next = NULL;
			
			temp->next = new;
			
			list_count++;
		}
		
		printf("Inserter #%lu inserting %d into list\n", tid, value);
		print(list);
		
		int sleep_time = rand() % 2 + 2;
		sleep(sleep_time);
		
		sem_post(&sem_i);
		sleep(1);
	}
	
	pthread_exit(0);
 }
 
 void *delete_func(void *id)
 {
	long tid = (long)id;
	int i;
	
	while(1)
	{	
		sem_wait(&sem_d);	
		sem_wait(&sem_i);
		for ( i = 0; i < thread_count; i++ ) {
			sem_wait(&sem_s[i]);
		}
		if (list == NULL) {	//Empty List
			printf("Removed nothing, Empty list\n");
		}
		else if (list->next == NULL) { //One node in list
			printf("Deleter #%lu removing %d from list\n", tid, list->data);
			print(list);
			free(list);
			
			//Reinitialize list since it is empty
			list = NULL;
			
			list_count--;
		}
		else {
			int remove_node_idx = rand() % list_count;
			node_t *curr = list;
			node_t *temp = NULL;
			
			for ( i = 0; i < remove_node_idx - 1; i++ ) {
				curr = curr->next;
			}
			
			temp = curr->next;
			curr->next = temp->next;
			printf("Deleter #%lu removing %d from list\n", tid, temp->data);
			print(list);
			free(temp);
			
			list_count--;
		}
		
		for ( i = 0; i < thread_count; i++ ) {
			sem_post(&sem_s[i]);
		}
		
		sem_post(&sem_i);	

		int sleep_time = rand() % 2 + 2;
		printf("Deleting for %d seconds\n\n", sleep_time);
		sleep(sleep_time);
		
		sem_post(&sem_d);
				
		sleep(1);
	}
	
	pthread_exit(0);
 }
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
