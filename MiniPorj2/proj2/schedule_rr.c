
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
	int tempn = n;
	int terminaltime = 0;
	int initialtime;
	int flag = 0;
	int qt = QUANTUM;
	int* tempbt = (int*)malloc(n * sizeof(int));

	for (i = 0; i < n; i++)
	{
		tempbt[i] = burs[i];
		terminaltime += burs[i];
	}

	for (i = 0; i < n; ++i)
		response[i] = i * qt;

	wait[0] = 0;

	int count;
	printf("\n taskid    initial time    terminal time \n");
	for (terminaltime = 0, count = 0; tempn != 0;)
	{
		initialtime = terminaltime;
		if (tempbt[count] <= qt && tempbt[count] > 0)
		{
			terminaltime += tempbt[count];
			tempbt[count] = 0;
			wait[count] = terminaltime - burs[count];
			turnaround[count] = wait[count] + burs[count];
			flag = 1;
		}
		else if (tempbt[count] > qt)
		{
			tempbt[count] -= qt;
			terminaltime += qt;
		}
		if (tempbt[count] == 0 && flag == 1)
		{
			tempn--;
			flag = 0;
		}
		if (initialtime != terminaltime)
		{
			temp = head;
			while (temp != NULL)
			{
				if (temp->task->tid == taskid[count])
				{
					printf("   %s\t\t %d\t\t %d\n", temp->task->name, initialtime, terminaltime);
					break;
				}
				temp = temp->next;
			}
			//printf("   T%d\t\t %d\t\t %d\n", taskid[count], initialtime, terminaltime);
		}
		if (count == n - 1)
			count = 0;
		else
			++count;
	}

	if (tempbt != NULL)
	{
		free(tempbt);
		tempbt = NULL;
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
				run(temp->task, k);
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
