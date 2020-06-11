#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

struct disk {
    int init;
    int total;
};

static int* duplicate(const int seq[], int len) {
    int* sequence = malloc(len * sizeof(int));
    for (int i = 0; i < len; ++i) {
        sequence[i] = seq[i];
    }
    return sequence;
}

static int cmp(const void *x, const void *y) { 
    return *(int*)x - *(int*)y; 
}


void CSCAN(struct disk *disk, int seq[], int len, FILE *out) {
    fprintf(out, "C_SCAN Algorithm\n"
           "Initial head position = %d\n"
           "Seek Sequence is:\n", disk->init);

    int* sequence = duplicate(seq, len);

    qsort(sequence, len, sizeof(int), cmp);
    int mid = 0;
    for (mid = 0; mid < len; ++mid) {
        if (sequence[mid] >= disk->init) {
            mid--;
            break;
        }
    }
    if (mid >= len) {
        mid = len - 1;
    }
    int* left = sequence;
    int  leftLen = mid + 1;

    int* right = sequence + (mid + 1);
    int  rightLen = len - leftLen;

    int movement = 0,  current = disk->init;

    for (int i = 0; i < rightLen; ++i) {
        fprintf(out, "%d\n", right[i]);
        movement += abs(right[i] - current);
        current = right[i];
    }

    if (leftLen != 0) {
        movement += (disk->total - 1) - current;
        movement += disk->total - 1;

        current = 0;
        for (int i = 0 ; i < leftLen; ++i) {
            fprintf(out, "%d\n", left[i]);
            movement += abs(current - left[i]);
            current = left[i];
        }
    }

    fprintf(out, "Total head movement for C_SCAN = %d", movement);
    free(sequence);
}


void SCAN(struct disk *disk, const int seq[], int len, FILE *out) {

    fprintf(out,
            "SCAN Algorithm\n"
           "Initial head position = %d\n"
           "Seek Sequence is:\n", disk->init);

    int* sequence = duplicate(seq, len);

    qsort(sequence, len, sizeof(int), cmp);
    int mid = 0;
    for (mid = 0; mid < len; ++mid) {
        if (sequence[mid] > disk->init) {
            mid--;
            break;
        }
        if (sequence[mid] == disk->init) {
            break;
        }
    }
    if (mid >= len) {
        mid = len - 1;
    }

    int* left = sequence;
    int  leftLen = mid + 1;

    int* right = sequence + (mid + 1);
    int  rightLen = len - leftLen;


    int movement = 0, current = disk->init;

    for (int i = leftLen - 1; i >= 0; --i) {
        fprintf(out, "%d\n", left[i]);
        movement += abs(current - left[i]);
        current = left[i];
    }

    if (rightLen != 0) {
        movement += current;

        current = 0;
        for (int i = 0; i < rightLen; ++i) {
            fprintf(out, "%d\n", right[i]);
            movement += abs(right[i] - current);
            current = right[i];
        }
    }

    fprintf(out, "Total head movement for SCAN = %d", movement);
    free(sequence);
}

void FCFS(struct disk *disk, int sequence[], int len, FILE *out) {

    int movement = 0, current = disk->init;

    fprintf(out,
            "FCFS Algorithm\n"
           "Initial head position = %d\n"
           "Seek Sequence is:\n", disk->init);

    for (int i = 0; i < len; ++i) {
        fprintf(out, "%d\n", sequence[i]);
        movement += abs(current - sequence[i]);
        current = sequence[i];
    }

    fprintf(out, "Total head movement for FCFS = %d", movement);
}

int main(int argc, const char* argv[]) {
#define SequenceLength 25

    const char* DefaultInputFile = "input.txt";

    if (argc >= 2) { DefaultInputFile = argv[1]; }

    FILE* input = fopen(DefaultInputFile, "r");
    if(input == NULL) {
        return -1;
    }

    int sequence[SequenceLength];

    for (int i = 0; i < SequenceLength; ++i) {
        fscanf(input, "%d", &sequence[i]);
    }
    fclose(input);


    FILE* output = fopen("output.txt", "w");
    if (output == NULL) {
        return -1;
    }

    struct disk disk;
    disk.init = 33;
    disk.total = 100;

    fprintf(output, "\n");

    FCFS(&disk, sequence, SequenceLength, output);
    fprintf(output, "\n\n");

    SCAN(&disk, sequence, SequenceLength, output);
    fprintf(output, "\n\n");

    CSCAN(&disk, sequence, SequenceLength, output);

    fflush(output);
    fclose(output);
}



