#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

int client_id = 0;

void *thread_cash_withdraw()
{

    int loop_limit = 1000000;
    printf("\n\n ****From thread_cash_withdraw****");
    printf("\nClient ID %d withdrawing cash", client_id);
    client_id+=1;

    for (int i = 0; i < loop_limit; i++);

    printf("\nAccount balance updated...");


}


int main()
{
  int i, num_threads = 10;

  pthread_t threads[num_threads];

  printf("From main: Going to create Threads...\n");

    for (i = 0; i<num_threads; i++)
	pthread_create(&threads[i], NULL, thread_cash_withdraw, NULL);

    for (i = 0; i<num_threads; i++)
	pthread_join(threads[i], NULL);

   return 0;

}
