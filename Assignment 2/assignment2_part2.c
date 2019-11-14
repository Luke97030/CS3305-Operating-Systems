/* By Olivia Toth 250965299 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BUFFER_MAX_LENGTH 1024
FILE *o_file;

/* Function computes the average wait time for a given array of individual process wait times
 * also prints the average to the screen and writes it out to a file */
void average_wait(int wait_times[], int index) {
  int running_total = 0;
  float average_wait_time;
  for (int j = 0; j < index; j++) {
    running_total = running_total + wait_times[j];
  }
  average_wait_time = (float) running_total / (float) index;
  printf("\nAverage waiting time = %0.1f\n", average_wait_time);
  fprintf(o_file, "\nAverage waiting time = %0.1f\n", average_wait_time);
}

/* Function prints the individual process wait times and process names for given arrays
 * also writes this out to a file */
void print_waiting_times(char *name_array[], int time_array[], int index) {
  printf("\nIndividual waiting times for each process:\n");
  fprintf(o_file, "\nIndividual waiting times for each process:\n");
  for (int j = 0; j < index; j++) {
    printf("%s = %d\n", name_array[j], time_array[j]);
    fprintf(o_file, "%s = %d\n", name_array[j], time_array[j]);
  }
}

/* Function prints the process names in the order that the CPU selects them for a certain
 * scheduling algorithm
 * also writes this out to a file */
void print_CPU_selection(char *array[], int index) {
  printf("\nOrder of selection by CPU:\n");
  fprintf(o_file, "\nOrder of selection by CPU:\n");
  for (int j = 0; j < index; j++) {
    printf("%s ", array[j]);
    fprintf(o_file, "%s ", array[j]);
  }
  printf("\n");
  fprintf(o_file, "\n");
}

int main() {
  // array for whole line
  char filestring[BUFFER_MAX_LENGTH];
  // array for burst times
  int CPU_burst_times[BUFFER_MAX_LENGTH];
  // array for process names
  char *process_names[BUFFER_MAX_LENGTH];
  // split string on white space
  char delim[] = " ";
  int queue_number, time_quantum;
  FILE *i_file;
  i_file = fopen("cpu_scheduling_input_file.txt", "r");
  o_file = fopen("cpu_scheduling_output_file.txt", "w");

  if (i_file == NULL) {
    perror("Error opening file");
    return(-1);
  }

  while (fgets(filestring, (int) sizeof(filestring), i_file)) {
    // the first word (q)
    char *ptr = strtok(filestring, delim);
    // move to the next word (the queue number)
    ptr = strtok(NULL, delim);
    // save it
    queue_number = atoi(ptr);
    // move to the next word (tq)
    ptr = strtok(NULL, delim);
    // move to the next word (the time quantum)
    ptr = strtok(NULL, delim);
    // save it
    time_quantum = atoi(ptr);
    // move to the next word (first process)
    ptr = strtok(NULL, delim);
    // very important index
    int i = 0;
    while (ptr != NULL) {
      process_names[i] = ptr;
      ptr = strtok(NULL, delim);
      CPU_burst_times[i] = atoi(ptr);
      ptr = strtok(NULL, delim);
      i++;
    }

    /* First come first serve */
    char *FCFS_names[i];
    int FCFS_burst_times[i];
    int FCFS_wait_times[i];
    // CPU selects processes in the order they arrive in the ready queue
    for (int j = 0; j < i; j++) {
      FCFS_burst_times[j] = CPU_burst_times[j];
      FCFS_names[j] = process_names[j];
    }

    FCFS_wait_times[0] = 0;
    for (int j = 1; j < i; j++) {
      FCFS_wait_times[j] = FCFS_burst_times[j - 1] + FCFS_wait_times[j - 1];
    }

    printf("Ready Queue %d Applying FCFS Scheduling:\n", queue_number);
    fprintf(o_file, "Ready Queue %d Applying FCFS Scheduling:\n", queue_number);
    print_CPU_selection(FCFS_names, i);
    print_waiting_times(FCFS_names, FCFS_wait_times, i);
    average_wait(FCFS_wait_times, i);

    /* Shortest job first */
    char *temp_names[i];
    int temp_times[i];
    char *SJF_names[i];
    int SJF_burst_times[i];
    int SJF_wait_times[i];
    // copying everything to a temporary array with the correct size (using the very important index) to work out of
    for (int j = 0; j < i; j++) {
      temp_times[j] = CPU_burst_times[j];
      temp_names[j] = process_names[j];
    }
    // puts the processes in order from shortest CPU burst time to longest CPU burst time, marking each completed process as -1 in the temporary array
    for (int q = 0; q < i; q++) {
      int index_of_smallest;
      int e = 0;
      while(temp_times[e] == -1) {
        e++;
      }
      int smallest = temp_times[e];
      index_of_smallest = e;
      for (int s = 0; s < i; s++) {
        if (temp_times[s] < smallest && temp_times[s] >= 0) {
          smallest = temp_times[s];
          index_of_smallest = s;
        }
      }
      SJF_burst_times[q] = temp_times[index_of_smallest];
      SJF_names[q] = temp_names[index_of_smallest];
      temp_times[index_of_smallest] = -1;
    }

    SJF_wait_times[0] = 0;
    for (int j = 1; j < i; j++) {
      SJF_wait_times[j] = SJF_burst_times[j - 1] + SJF_wait_times[j - 1];
    }

    printf("\nReady Queue %d Applying SJF Scheduling:\n", queue_number);
    fprintf(o_file, "\nReady Queue %d Applying SJF Scheduling:\n", queue_number);
    print_CPU_selection(SJF_names, i);
    print_waiting_times(SJF_names, SJF_wait_times, i);
    average_wait(SJF_wait_times, i);

    /* Round robin */
    int RR_turnaround_times[i];
    int RR_turnaround_times_to_print[i];
    char *RR_names[i];
    int arr_index = 0;
    // copying everything to a temporary array with the correct size (using the very important index) to work out of
    // also making an array of turnaround times and initialzing all values to 0
    for (int j = 0; j < i; j++) {
      temp_times[j] = CPU_burst_times[j];
      temp_names[j] = process_names[j];
      RR_turnaround_times[j] = 0;
    }
    printf("\nReady Queue %d Applying RR Scheduling:\n", queue_number);
    fprintf(o_file, "\nReady Queue %d Applying RR Scheduling:\n", queue_number);
    printf("\nOrder of selection by CPU:\n");
    fprintf(o_file, "\nOrder of selection by CPU:\n");
    // loop keeps executing processes until they are all executed (completed processes are marked as -1)
    int loop = 1;
    while (loop == 1) {
      for (int s = 0; s < i; s++) {
        // if the CPU burst time is less than or equal to the time quantum and not -1
        if (temp_times[s] <= time_quantum && temp_times[s] > 0) {
          printf("%s ", temp_names[s]);
          fprintf(o_file, "%s ", temp_names[s]);
          if (RR_turnaround_times[s] == 0) { //first time through the loop
            int j;
            // update the turnaround for the current process and all processes preceding it
            for (j = 0; j <= s; j++) {
              RR_turnaround_times[j] = RR_turnaround_times[j] + temp_times[s];
            }
            // this process is completed, store its name and turnaround time
            RR_turnaround_times_to_print[arr_index] = RR_turnaround_times[s];
            RR_names[arr_index] = temp_names[s];
            arr_index++;
          }
          else { // not the first time through the loop
            int j;
            // update all the turnaround times
            for (j = 0; j < i; j++) {
                RR_turnaround_times[j] = RR_turnaround_times[j] + temp_times[s];
            }
            // this process is completed, store its name and turnaround time
            RR_turnaround_times_to_print[arr_index] = RR_turnaround_times[s];
            RR_names[arr_index] = temp_names[s];
            arr_index++;
          }
          // mark the process as completed by setting the burst time to -1
          temp_times[s] = -1;
        }
        // if the  CPU burst time is greater than the time quantum
        if (temp_times[s] > time_quantum) {
          // decrement the burst time by the time quantum
          temp_times[s] = temp_times[s] - time_quantum;
          printf("%s ", temp_names[s]);
          fprintf(o_file, "%s ", temp_names[s]);
          if (RR_turnaround_times[s] == 0) { //first time through the loop
            int j;
            // update the turnaround for the current process and all processes preceding it
            for (j = 0; j <= s; j++) {
              RR_turnaround_times[j] = RR_turnaround_times[j] + time_quantum;
            }
          }
          else { // not the first time through the loop
            int j;
            // update all the turnaround times
            for (j = 0; j <= i; j++) {
                RR_turnaround_times[j] = RR_turnaround_times[j] + time_quantum;
            }
          }
        }
      }
      // check if all processes have been executed (if they are all marked -1)
      loop = 0;
      for (int s = 0; s < i; s++) {
        if (temp_times[s] > 0) {
          loop = 1;
          break;
        }
      }
    }
    printf("\n");
    fprintf(o_file, "\n");
    printf("\nTurnaround times for each process:\n");
    fprintf(o_file, "\nTurnaround times for each process:\n");
    for (int j = 0; j < i; j++) {
      printf("%s = %d\n", RR_names[j], RR_turnaround_times_to_print[j]);
      fprintf(o_file, "%s = %d\n", RR_names[j], RR_turnaround_times_to_print[j]);
    }
    printf("\n");
    fprintf(o_file, "\n");
  }
  fclose(i_file);
  fclose(o_file);
}
