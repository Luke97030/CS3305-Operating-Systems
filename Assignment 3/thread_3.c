#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

int client_id = 0;
pthread_mutex_t lock;

void *thread_cash_withdraw() {
  int loop_limit = 10000;
  printf("\n\n ****From thread_cash_withdraw****");

  pthread_mutex_lock(&lock);  // ENTRY REGION
  printf("\nClient ID %d withdrawing cash", client_id);  //CRITICAL STARTS
  client_id+=1;
  for (int i = 0; i < loop_limit; i++)
    printf("\nAccount balanced updated..."); // CRITICAL ENDS
  pthread_mutex_unlock(&lock); // EXIT REGION
}

int main() {
  int i, num_threads = 10;
  pthread_t threads[num_threads];
  if (pthread_mutex_init(&lock, NULL) != 0) {
      printf("\n mutex init failed\n");
      return 1;
  }

  printf("From main: Going to create Threads...\n");
  for (i = 0; i<num_threads; i++)
	 pthread_create(&threads[i], NULL, thread_cash_withdraw, NULL);
  for (i = 0; i<num_threads; i++)
	 pthread_join(threads[i], NULL);
  pthread_mutex_destroy(&lock);
  return 0;
}
