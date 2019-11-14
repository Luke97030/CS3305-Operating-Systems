/* By Olivia Toth 250965299 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

int main() {
  pid_t parentID, child_1ID, other_childID, pid;

  pid = fork();

  if (pid < 0) { //system function failed
  	printf("fork unsuccessful");
  	exit(1);
  }

  if (pid > 0) { //we are in the parent process
    parentID = getpid();
    printf("\nparent process (PID %d) created child_1 (PID %d)\n", parentID, pid);
    printf("\nparent (PID %d) is waiting for child_1 (PID %d) to complete before creating child_2\n", parentID, pid);
    wait(NULL);
  }

  if (pid == 0) { //we are in the child process
    child_1ID = getpid();
  }

  pid = fork();

  if (pid < 0) { //system function failed
    printf("fork unsuccessful");
    exit(1);
  }

  if (pid == 0) { //we are in either child_1.1 or child_2
    other_childID = getpid();
    if (other_childID == (child_1ID+1)) { //we are in child_1.1
      printf("\nchild_1 (PID %d) created child_1.1 (PID %d)\n", child_1ID, other_childID);
      printf("\nchild_1 (PID %d) is now completed\n", child_1ID);
    }
    else { //we are in child_2
      printf("\nparent (PID %d) created child_2 (PID %d)\n", parentID, other_childID);
      printf("\nchild_2 (PID %d) is calling an external program external_program.out and leaving child_2...\n", other_childID);
      execl("external_program.out", NULL);
    }
  }

}
