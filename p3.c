#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/time.h>
#include <assert.h>

double GetTime() {
  struct timeval t;
  int rc = gettimeofday(&t, NULL);
  assert(rc == 0);
  return (double)t.tv_sec + (double)t.tv_usec/1e6;
}

void Spin(int howlong) {
  double t = GetTime();
  while ((GetTime() - t) < (double)howlong) {
    ;} // do nothing in loop
}

// *******************************************

sem_t availableAssts;
sem_t customersWaiting;

struct customer_data {
  int cid;
  int arrival_time;
  int service_time;
};

#define Q_SIZE 4

typedef struct {
  int cid;
  int arrival_time;
  int service_time;
} CustomerData;

typedef struct {
  CustomerData* customers[Q_SIZE];
  int head;
  int tail;
} CustomerQueue;

void init_queue(CustomerQueue* q) {
  q->head = 0;
  q->tail = 0;
}

int is_queue_empty(CustomerQueue* q) {
  return q->head == q->tail;
}

int is_queue_full(CustomerQueue* q) {
  return (q->tail + 1) % Q_SIZE == q->head;
}

void enqueue(CustomerQueue* q, CustomerData* customerData) {
  if (is_queue_full(q)) {
    printf("Queue is full\n");
    return;
  }
  q->customers[q->tail] = customerData;
  q->tail = (q->tail + 1) % Q_SIZE;
}

CustomerData* dequeue(CustomerQueue* q) {
  if (is_queue_empty(q)) {
    printf("Queue is empty\n");
    return NULL;
  }
  CustomerData* customer = q->customers[q->head];
  q->head = (q->head + 1) % Q_SIZE;
  return customer;
}

void service() {
  while (1) {
    sem_wait(&customersWaiting); // sleep until customers arrive
    // mutex lock
    // customer = customerQueue.dequeue()
    // mutex unlock
    // printf("Time %d: Customer %d starts\n", how to calculate time when customer starts?
    // Spin(customer->service_time);
    // printf("Time %d: Customer %d leaves\n", how to calculate time when customer left?
    sem_post(&availableAssts); // indicate an asst is available
  }
}

void enqueueCustomer(struct customer_data* cust_data) {
  // mutex lock
  int customersWaitingSemVal;
  sem_getvalue(&customersWaiting, &customersWaitingSemVal);
  if (customersWaitingSemVal >= 4) {
    printf("Time %d: Customer %d leaves\n", cust_data->arrival_time, cust_data->cid);
    // mutex unlock
    return;
  }
  // customerQueue.enqueue(cust_data)
  // mutex unlock
  // sem_post(&customersWaiting); // increment customersWaiting, wake any sleeping assts
  // sem_wait(&availableAssts); // sleep until an asst is available
}

void createCustomer(void* custData) {
  struct customer_data* cust_data = (struct customer_data*) custData;
  usleep(cust_data->arrival_time * 1000000); // wait x seconds
  printf("Time %d: Customer %d arrives\n", cust_data->arrival_time, cust_data->cid);
  // enqueue customer
  enqueueCustomer(cust_data);
}

int main(void) {
    
  sem_init(&availableAssts, 0, 2);
  sem_init(&customersWaiting, 0, 0);

  pthread_t* asst1 = malloc(sizeof(*asst1));
  pthread_t* asst2 = malloc(sizeof(*asst2));
  pthread_create(asst1, NULL, (void*)service, NULL);
  pthread_create(asst2, NULL, (void*)service, NULL);

  pthread_t* cust1 = malloc(sizeof(*cust1));
  struct customer_data cust1Data;
  cust1Data.cid = 1;
  cust1Data.arrival_time = 3;
  cust1Data.service_time = 15;
  
  pthread_create(cust1, NULL, (void*)createCustomer, (void*)&cust1Data);
  pthread_join(*cust1, NULL);
  // pthread_t* cust2 = malloc(sizeof(*cust2));
  // pthread_t* cust3 = malloc(sizeof(*cust3));
  // pthread_t* cust4 = malloc(sizeof(*cust4));
  // pthread_t* cust5 = malloc(sizeof(*cust5));
  // pthread_t* cust6 = malloc(sizeof(*cust6));
  // pthread_t* cust7 = malloc(sizeof(*cust7));
  // pthread_t* cust8 = malloc(sizeof(*cust8));
  // pthread_t* cust9 = malloc(sizeof(*cust9));
  // pthread_t* cust10 = malloc(sizeof(*cust10));
  // pthread_t* cust11 = malloc(sizeof(*cust11));
  // pthread_t* cust12 = malloc(sizeof(*cust12));
    
  
  return 0;
}