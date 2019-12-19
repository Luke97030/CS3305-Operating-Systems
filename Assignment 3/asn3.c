/* By Olivia Toth 250965299 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define BUFFER_MAX_LENGTH 1024
#define OVERDRAFT_LIMIT -5000
#define ARRAY_SIZE 100

// arrays for the account information
char account_names[ARRAY_SIZE][5];
char account_types[ARRAY_SIZE][9];
int deposit_fees[ARRAY_SIZE];
int withdrawl_fees[ARRAY_SIZE];
int transfer_fees[ARRAY_SIZE];
int free_transactions[ARRAY_SIZE];
int transaction_fees[ARRAY_SIZE];
int overdraft_fees[ARRAY_SIZE];
int balance[ARRAY_SIZE];

pthread_mutex_t lock;
int overdraft_fee = 0;

void *process_thread(void *string);
void deposit(char account[], int amount);
int withdraw(char account[], int amount);
void transfer(char account1[], char account2[], int amount);

void *process_thread(void *string) {
  // split string on white space
  char delim[] = " ";
  char *ptr = strtok((char *)string, delim);
  ptr = strtok(NULL, delim);
  // go through the line word by word and process transactions by calling the banking functions with the values given in the line
  while (ptr != NULL) {
    if (*ptr == 'd') {
      char acc_name[5];
      int amt;
      ptr = strtok(NULL, delim);
      strcpy(acc_name, ptr);

      ptr = strtok(NULL, delim);
      amt = atoi(ptr);
      pthread_mutex_lock(&lock);
      deposit(acc_name, amt);
      pthread_mutex_unlock(&lock);
    }
    if (*ptr == 'w') {
      char acc_name[5];
      int amt;
      ptr = strtok(NULL, delim);
      strcpy(acc_name, ptr);

      ptr = strtok(NULL, delim);
      amt = atoi(ptr);
      pthread_mutex_lock(&lock);
      int withdraw_result = withdraw(acc_name, amt);
      pthread_mutex_unlock(&lock);
    }
    if (*ptr == 't') {
      char acc1_name[5];
      char acc2_name[5];
      int amt;
      ptr = strtok(NULL, delim);
      strcpy(acc1_name, ptr);

      ptr = strtok(NULL, delim);
      strcpy(acc2_name, ptr);

      ptr = strtok(NULL, delim);
      amt = atoi(ptr);
      pthread_mutex_lock(&lock);
      transfer(acc1_name, acc2_name, amt);
      pthread_mutex_unlock(&lock);
    }
    ptr = strtok(NULL, delim);
  }
  pthread_exit(NULL);
}


void deposit(char account[], int amount) {
  // do a linear search to find the index of the account
  int index;
  for (int i = 0; i < ARRAY_SIZE; i++) {
    if (strcmp(account, account_names[i]) == 0) {
      index = i;
      break;
    }
  }
  // change the account balance to reflect the deposit and add any relevant fees
  balance[index] = balance[index] + amount - deposit_fees[index];
  free_transactions[index] = free_transactions[index] - 1;
  if (free_transactions[index] < 0) {
    balance[index] = balance[index] - transaction_fees[index];
  }
}

int withdraw(char account[], int amount) {
  // do a linear search to find the index of the account
  int index;
  for (int i = 0; i < ARRAY_SIZE; i++) {
    if (strcmp(account, account_names[i]) == 0) {
      index = i;
      break;
    }
  }
  // first check if we can even do the withdrawl
  // see what the new account balance would be, including if there is a fee for the transaction
  int new_balance = balance[index] - amount - withdrawl_fees[index];
  free_transactions[index] = free_transactions[index] - 1;
  if (free_transactions[index] < 0) {
    new_balance = new_balance - transaction_fees[index];
  }
  // if either of the next two statements is true, we can't do the withdrawl, so exit without changing anything
  if (new_balance < 0 && overdraft_fees[index] == 0) {
    free_transactions[index] = free_transactions[index] + 1;
    return -1;
  }
  if (new_balance < OVERDRAFT_LIMIT) {
    free_transactions[index] = free_transactions[index] + 1;
    return -1;
  }
  // check if we'd have to apply the overdraft fee, and if so, decrement it
  if (new_balance < overdraft_fee) {
    new_balance = new_balance - overdraft_fees[index];
    overdraft_fee = overdraft_fee - 500;
  }
  // change the account balance to reflect the withdrawl
  balance[index] = new_balance;
  return 1;
}

void transfer(char account1[], char account2[], int amount) {
  // do a linear search to find the index of the first account
  int index1;
  for (int i = 0; i < ARRAY_SIZE; i++) {
    if (strcmp(account1, account_names[i]) == 0) {
      index1 = i;
      break;
    }
  }
  // do a linear search to find the index of the second account
  int index2;
  for (int i = 0; i < ARRAY_SIZE; i++) {
    if (strcmp(account2, account_names[i]) == 0) {
      index2 = i;
      break;
    }
  }
  // withdraw the money from the first account, and if that was successful (returned 1), deposit the money into the second account
  int withdraw_result = withdraw(account1, amount);
  if (withdraw_result == 1) {
    deposit(account2, amount);
    // refund the fees that withdrawing and depositing might have taken and apply the fees for transferring
    balance[index1] = balance[index1] + withdrawl_fees[index1] - transfer_fees[index1];
    balance[index2] = balance[index2] + withdrawl_fees[index2] - transfer_fees[index2];
  }
}

int main(void) {
  // array for the whole line
  char filestring[BUFFER_MAX_LENGTH];
  // split string on white space
  char delim[] = " ";
  // open input and output files
  FILE *i_file;
  i_file = fopen("assignment_3_input_file.txt", "r");
  FILE *o_file;
  o_file = fopen("assignment_3_output_file.txt", "w");

  if (i_file == NULL || o_file == NULL) {
    perror("Error opening file");
    return(-1);
  }

  // setting up the indexes for the accounts, clients, and depositors
  int number_of_accounts = 0;
  int number_of_depositors = 0;
  int number_of_clients = 0;

  while (fgets(filestring, (int) sizeof(filestring), i_file)) {
    char firstchar = filestring[0];
    if (firstchar == 'a') {
      number_of_accounts = number_of_accounts + 1;
    }
    if (firstchar == 'd') {
      number_of_depositors = number_of_depositors + 1;
    }
    if (firstchar == 'c') {
      number_of_clients = number_of_clients + 1;
    }
  }

  // closing the input file and reopening it so I can start from the top
  fclose(i_file);
  i_file = fopen("assignment_3_input_file.txt", "r");
  if (i_file == NULL) {
    perror("Error opening file");
    return(-1);
  }

  // fill the arrays for the accounts with the relevant info
  for (int i = 0; i < number_of_accounts; i++) {
    fgets(filestring, (int) sizeof(filestring), i_file);
    char *ptr = strtok(filestring, delim);
    strcpy(account_names[i], ptr);
    ptr = strtok(NULL, delim);
    ptr = strtok(NULL, delim);
    strcpy(account_types[i], ptr);
    ptr = strtok(NULL, delim);
    ptr = strtok(NULL, delim);
    deposit_fees[i] = atoi(ptr);
    ptr = strtok(NULL, delim);
    ptr = strtok(NULL, delim);
    withdrawl_fees[i] = atoi(ptr);
    ptr = strtok(NULL, delim);
    ptr = strtok(NULL, delim);
    transfer_fees[i] = atoi(ptr);
    ptr = strtok(NULL, delim);
    ptr = strtok(NULL, delim);
    free_transactions[i] = atoi(ptr);
    ptr = strtok(NULL, delim);
    transaction_fees[i] = atoi(ptr);
    ptr = strtok(NULL, delim);
    ptr = strtok(NULL, delim);
    if (*ptr == 'N') {
      overdraft_fees[i] = 0;
    }
    else {
      ptr = strtok(NULL, delim);
      overdraft_fees[i] = atoi(ptr);
    }
    balance[i] = 0;
  }

  int err_thread;
  pthread_t depositor_threads[number_of_depositors];
  pthread_t client_threads[number_of_clients];

  if (pthread_mutex_init(&lock, NULL) != 0) {
    printf("\nMutex init failed.\n");
    return 1;
  }

  // processing the depositor threads so they go through before the clients
  for (int i = 0; i < number_of_depositors; i++) {
    char depositor_string[BUFFER_MAX_LENGTH];
    fgets(depositor_string, (int) sizeof(depositor_string), i_file);
    err_thread = pthread_create(&depositor_threads[i], NULL, &process_thread, depositor_string);
    if (err_thread != 0) {
      printf("\nError creating depositor %d.", i + 1);
    }
    pthread_join(depositor_threads[i], NULL);
  }

  /*for (int i = 0; i < number_of_depositors; i++) {
    pthread_join(depositor_threads[i], NULL);
  }*/

  // processing the client threads
  for (int i = 0; i < number_of_clients; i++) {
    char client_string[BUFFER_MAX_LENGTH];
    fgets(client_string, (int) sizeof(client_string), i_file);
    err_thread = pthread_create(&client_threads[i], NULL, &process_thread, client_string);
    if (err_thread != 0) {
      printf("\nError creating client %d.", i + 1);
    }
    pthread_join(client_threads[i], NULL);
  }

  /*for (i = 0; i < number_of_clients; i++) {
    pthread_join(client_threads[i], NULL);
  } */

  // output the final account information to the screen and to a file
  for (int i = 0; i < number_of_accounts; i++) {
    printf("%s type %s %d\n", account_names[i], account_types[i], balance[i]);
    fprintf(o_file, "%s type %s %d\n", account_names[i], account_types[i], balance[i]);
  }

  pthread_mutex_destroy(&lock);
  return 0;
}
