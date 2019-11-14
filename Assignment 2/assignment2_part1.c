/* By Olivia Toth 250965299 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

int temp_sum = 0;

void *thread_adds_numbers(void *number) {
  int *number_to_be_added = (int*) number;

  if (*number_to_be_added == 10)
    temp_sum += *number_to_be_added;
  else if (*number_to_be_added == 20)
    temp_sum += *number_to_be_added;
  else
    *number_to_be_added = temp_sum;

  return 0;
}

int main() {
  int x = 10, y = 20, z = 0; //x, y, z are local to the parent program
  pid_t pid, parentID, childID;
  pthread_t pthread0, pthread1, pthread2;

  pid = fork();

  if (pid < 0) { //fork failed
    printf ("fork unsuccessful");
    exit(1);
  }

  if (pid > 0) { //we are in the parent process
    parentID = getpid();
    printf("\nparent process (PID %d) created a child process (PID %d)\n", parentID, pid);
    printf("\nparent (PID %d) is waiting for child (PID %d) to complete before proceeding\n", parentID, pid);
    wait(NULL);
    printf("\nthe value of z from the parent process (PID %d) is %d\n", parentID, z);
    printf("\nparent process (PID %d) is now creating a thread\n", parentID);
    printf("\nparent (PID %d) is waiting for thread to complete before proceeding\n", parentID);
    pthread_create(&pthread0, NULL, thread_adds_numbers, &x);
    pthread_join(pthread0, NULL);
    pthread_create(&pthread1, NULL, thread_adds_numbers, &y);
    pthread_join(pthread1, NULL);
    pthread_create(&pthread2, NULL, thread_adds_numbers, &z);
    pthread_join(pthread2, NULL);
    printf("\nthe value of z from the parent process (PID %d) is %d\n\n", parentID, z);
  }

  if (pid == 0) { //we are in the child process
    childID = getpid();
    printf("\nchild (PID %d) is adding x and y and storing the result in z\n", childID);
    z = x + y;
    printf("\nchild (PID %d) is now complete\n", childID);
  }

  return 0;
}
