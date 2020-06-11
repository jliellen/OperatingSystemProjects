
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "schedulers.h"
#include "cpu.h"

// add a task to the list 
void add(struct node **head, char *name, int priority, int burst, int tid)
{
	struct task *newTask = malloc(sizeof(struct task));
	newTask->name = name;
	newTask->priority = priority;
	newTask->burst = burst;
	newTask->tid = tid;
	insert(head, newTask);
}

// invoke the scheduler
void schedule(struct node *head)
{
	int n = 0;
	struct node *temp = head;
	while (temp != NULL)
	{
		temp = temp->next;
		++n;
	}

	if (n < 1)
		return;

	int* taskid = (int*)malloc(n * sizeof(int));
	int* priority = (int*)malloc(n * sizeof(int));
	int* burs = (int*)malloc(n * sizeof(int));

	int* wait = (int*)malloc(n * sizeof(int));
	int* turnaround = (int*)malloc(n * sizeof(int));
	int* response = (int*)malloc(n * sizeof(int));

	int i = 0;
	temp = head;
	while (temp != NULL)
	{
		taskid[i] = temp->task->tid;
		priority[i] = temp->task->priority;
		burs[i] = temp->task->burst;

		temp = temp->next;
		++i;
	}


	// invoke the scheduler
	int pos, temp2, j;
	for (i = 0; i < n; i++)
	{
		pos = i;
		for (j = i + 1; j < n; j++)
		{
			if (burs[j] < burs[pos])
			{
				pos = j;
			}
		}
		temp2 = burs[i];
		burs[i] = burs[pos];
		burs[pos] = temp2;

		temp2 = taskid[i];
		taskid[i] = taskid[pos];
		taskid[pos] = temp2;
	}

	wait[0] = 0;
	turnaround[0] = burs[0];
	response[0] = 0;
	for (i = 1; i < n; i++)
	{
		//waiting time[p] = waiting time[p-1] + Burst Time[p-1]
		wait[i] = wait[i - 1] + burs[i - 1];
		//Turnaround Time = Waiting Time + Burst Time
		turnaround[i] = wait[i] + burs[i];
		//response time[p] = waiting time[p-1] + Burst Time[p-1]
		response[i] = wait[i - 1] + burs[i - 1];
	}


	//print result
	printf("\n");
	int k = 1;
	for (i = 0; i < n; ++i)
	{
		temp = head;
		while (temp != NULL)
		{
			if (temp->task->tid == taskid[i])
			{
				run(temp->task, temp->task->burst);
				++k;
				break;
			}

			temp = temp->next;
		}
	}

	//print average
	float avgwt = 0, avgtat = 0, avgres = 0;
	for (i = 0; i < n; i++)
	{
		avgwt += wait[i];
		avgtat += turnaround[i];
		avgres += response[i];
	}
	avgwt = avgwt / n;
	avgtat = avgtat / n;
	avgres = avgres / n;


	printf("\nAverage Waiting Time = %f \n", avgwt);
	printf("Average Turnaround Time = %f \n", avgtat);
	printf("Average Response Time = %f \n", avgres);

	if (taskid != NULL)
	{
		free(taskid);
		taskid = NULL;
	}
	if (priority != NULL)
	{
		free(priority);
		priority = NULL;
	}
	if (burs != NULL)
	{
		free(burs);
		burs = NULL;
	}
	if (wait != NULL)
	{
		free(wait);
		wait = NULL;
	}
	if (turnaround != NULL)
	{
		free(turnaround);
		turnaround = NULL;
	}
	if (response != NULL)
	{
		free(response);
		response = NULL;
	}
}
