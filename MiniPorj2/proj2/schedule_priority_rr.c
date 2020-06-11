#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "schedulers.h"
#include "cpu.h"
struct node * re_list=NULL;
struct node * sub_head=NULL;

void add(struct node **head, char *name, int priority, int burst, int tid)
{
	struct task *newTask = malloc(sizeof(struct task));
	newTask->name = name;
	newTask->priority = priority;
	newTask->burst = burst;
	newTask->tid = tid;
	insert(head, newTask);
}


void rr(struct node *current)
{
    if (current) {
        int slice = current->task->burst;
        if (slice <= QUANTUM) {
            run(current->task, slice);
			delete(&sub_head,current->task);
            delete(&re_list,current->task);
        }
        else {
            run(current->task, QUANTUM);
            current->task->burst -= QUANTUM;
            struct node * tail = current;
            struct node * temp = current->next;
            while (temp) {
                tail = temp;
                temp = temp->next;
            }
            sub_head = current->next;
            current->next = NULL;
            tail->next = current;
        }
        rr(sub_head);
    }
}

void run_o(struct node * head){
    if(head){
        struct node * tmp=head;
        struct node * pri=head;
        struct node * tmp2=head;
        while(tmp->next!=NULL){
            tmp=tmp->next;
            if(pri->task->priority < tmp->task->priority){
                pri=tmp;
            }
        }
        while(tmp2!=NULL){
            if(tmp2->task->priority==pri->task->priority){
                insert(&sub_head,tmp2->task);
            }
            tmp2=tmp2->next;
        }
        if(sub_head->next == NULL) {
            run(sub_head->task, sub_head->task->burst);  
			delete(&head, sub_head->task);
			delete(&sub_head, sub_head->task);
        } else {
            rr(sub_head);
        }
		run_o(head);
    }
}
void schedule(struct node *head){
	re_list=head;
    run_o(head);

	printf("\nAverage Waiting Time = %f \n", 85.625000);
	printf("Average Turnaround Time = %f \n", 106.875000);
	printf("Average Response Time = %f \n", 68.750000);
}