/* By Olivia Toth 250965299 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

int main(int argc, char *argv[]) {
  pid_t parentID, childID, pid;
  int fd[2];
  int x, y;
  char y_string[50];

  if (pipe(fd)< 0) { //system function failed
    perror("pipe error");
    exit(0);
  }

  pid = fork();

  if (pid < 0) { //system function failed
    perror("fork error");
    exit(0);
  }

  if (pid > 0) { //we are in the parent process
    x = atoi(argv[1]); //convert the first arugement to an integer
    parentID = getpid();
    printf("\nA pipe is created for communication between parent (PID %d) and child\n", parentID);
    printf("\nparent (PID %d) created a child (PID %d)\n", parentID, pid);
    printf("\nparent (PID %d) reading X = %d from the user\n", parentID, x);
    wait(NULL);
    read(fd[0], y_string, sizeof(y_string)); //read the second arguement from the pipe
    printf("\nparent (PID %d) reading Y from the pipe (Y = %s)\n", parentID, y_string);
    y = atoi(y_string); //convert the second arguement to an integer
    printf("\nparent (PID %d) adding X + Y = %d\n", parentID, x + y); //add the two integers
  }

  if (pid == 0) { //we are in the child process
    strcpy(y_string, argv[2]); //copy the second arguement into a variable
    parentID = getppid();
    childID = getpid();
    printf("\nchild (PID %d) reading Y = %s from the user\n", childID, y_string);
    printf("\nchild (PID %d) writing Y into the pipe\n", childID);
    write(fd[1], y_string, sizeof(y_string)); //write the second argument into the pipe
  }
}
