/**
 * Driver.c
 *
 * Schedule is in the format
 *
 *  [name] [priority] [CPU burst]
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "task.h"
#include "list.h"
#include "schedulers.h"
#include "cpu.h"

#define SIZE    100

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		printf("please input txt file.\neg. \n./fcfs schedule.txt\n");
		return -1;
	}

	FILE *in;
	char *temp;
	char task[SIZE];

	char *name;
	int priority;
	int burst;

	in = fopen(argv[1], "r");
	if (in == NULL)
	{
		printf("the file %s is not exist\n", argv[1]);
		return -1;
	}

	int k = 1;
	struct node *head = NULL;
	while (fgets(task, SIZE, in) != NULL) {
		temp = strdup(task);
		name = strsep(&temp, ",");
		priority = atoi(strsep(&temp, ","));
		burst = atoi(strsep(&temp, ","));

		// add the task to the scheduler's list of tasks
		if (priority >= MIN_PRIORITY && priority <= MAX_PRIORITY && burst > 0)
			add(&head, name, priority, burst, k++);

		free(temp);
	}

	fclose(in);

	reverse(&head);

	printf("\n name  priority  burs\n");
	printf("--------------------------\n");
	traverse(head);

	// invoke the scheduler
	schedule(head);

	return 0;
}
