#include<stdio.h>
#include<sys/types.h>
#include<semaphore.h>
#include<pthread.h>

int table_used = 1;
int generated_item[2];
char *item[] = { "tobacco", "paper", "matches" };
sem_t table;

void *agent () {
   int item1 = 0;
   int item2 = 0;
   while (1) {
      sleep (1);
      sem_wait (&table);
      if (table_used==1) {
         item1 = rand() % 3;
         item2 = rand() % 3;
         if (item1!=item2) {
            generated_item[0] = item1;
            generated_item[1] = item2;
            printf ( "The agent offers:\n\nitem1 is : %s, item2 is : %s\n", item[item1], item[item2] );
            table_used = 0;
         }
      }
   sem_post(&table);
   }
}

void *smokerI (int i) {
   while (1) {
      sleep (1);
      sem_wait (&table);
      if (table_used==0) {
         if (generated_item[0]!=i && generated_item[1]!=i) {
	    if (i==0) {
               printf ( "smoker with only tobacco completed his smoking\n" );
	       printf ( "**********************************************\n");
            }
	    else if (i==1) {
               printf ( "smoker with only paper completed his smoking\n" );
               printf ( "**********************************************\n");
	    }
            else if (i==2) {
	       printf ( "smoker with only matches completed his smoking\n" );
               printf ( "**********************************************\n");
            }
            else
               printf ( "There is no such a smoker\n" );
            table_used=1;
         }
      }
      sem_post(&table);
   }
}

int main() {
   pthread_t smkr0, smkr1, smkr2, agnt;
   sem_init(&table, 0, 1);
   int smokerZero = 0;
   int smokerOne = 1;
   int smokerTwo = 2;
   pthread_create(&agnt, NULL, agent, NULL);
   pthread_create(&smkr0, NULL, smokerI, smokerZero);
   pthread_create(&smkr1, NULL, smokerI, smokerOne);
   pthread_create(&smkr2, NULL, smokerI, smokerTwo);
   pthread_join(agnt, NULL);
   pthread_join(smkr0, NULL);
   pthread_join(smkr1, NULL);  
   pthread_join(smkr2, NULL);
}
