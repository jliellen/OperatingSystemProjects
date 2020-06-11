
#ifndef SCHEDULERS_H
#define SCHEDULERS_H

#include "list.h"
#include "task.h"

#define MIN_PRIORITY 1
#define MAX_PRIORITY 10

// add a task to the list 
void add(struct node **head, char *name, int priority, int burst, int tid);

// invoke the scheduler
void schedule(struct node *head);


#endif