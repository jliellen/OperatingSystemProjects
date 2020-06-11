#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//shift and masking 
void resolve(int logical, int *page, int *offset) {
	*page = (logical & 0XFF00) >> 8;
	*offset = (logical & 0XFF);
}

void readContentIntoMemory(FILE *bin, int page, int frame, char *phyMemory) {
	char content[256];
	fseek(bin, page * 256, SEEK_SET);
	fread(content, 256, 1, bin);
	memcpy(phyMemory + frame * 256, content, 256);
}

int main(int argc, char *argv[]) {
	int pageTable[128];
	int lru[128];
	int tlb[16][2];
	char phyMemory[32768];
	int i;
	int logical;
	int page, offset;
	int frame;
	int hit, fault;
	int fault_count = 0;
	int hit_count = 0;
	float fault_rate;
	float hit_rate;
	int count = 0;
	int curTime;
	FILE *bin, *fp;
	FILE *out;

	//when users don't give bin or address file
	if (argc < 3) {
		fprintf(stderr, "usage: %s filenameoflogicaladdresses\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	bin = fopen(argv[1], "rb");
	if (bin == NULL) {
		fprintf(stderr, "open the file \"%s\"failed.\n", argv[1]);
		exit(EXIT_FAILURE);
	}

	fp = fopen(argv[2], "r");
	if (fp == NULL) {
		fprintf(stderr, "open the file \"%s\"failed.\n", argv[2]);
		exit(EXIT_FAILURE);
	}

	out = fopen("output_with_page_replace.csv", "w");
	if (out == NULL) {
		fprintf(stderr, "open the file \"output_with_page_replace.csv\"failed.\n");
		exit(EXIT_FAILURE);
	}

	//initialize tlb and page table and lru
	for (i = 0; i < 128; ++i)
		pageTable[i] = (lru[i] = -1);
	for (i = 0; i < 16; ++i)
		tlb[i][0] = (tlb[i][1] = -1);

	curTime = 0;
	while (!feof(fp)) {
		if (fscanf(fp, "%d", &logical) == 1) {
			resolve(logical, &page, &offset);
			
			//lru[i] decrement by one every round
			hit = fault = 0;
			for (i = 0; i < 128; ++i) {
				lru[i]--;
			}

			//looking for it in tlb
			frame = -1;
			for (i = 0; i < 16; ++i) {
				if (tlb[i][0] == page) {
					frame = tlb[i][1];
					hit = 1;
					lru[frame] = curTime; // if found in TLB, make it to 0
					break;
				}
			}

			//looking for it in pageTable
			if (frame == -1) {
				for (i = 0; i < 128; ++i) {
					if (pageTable[i] == page) {
						frame = i;
						lru[frame] = curTime; // if found in pageTable, make it to 0
						break;
					}
				}

				if (frame == -1) {
					fault = 1;

					for (i = 0; i < 128; ++i) {
						if (pageTable[i] == -1) {
							frame = i;
							pageTable[i] = page;
							lru[frame] = curTime;
							break;
						}
					} 

					//find the index of the min lru, and swap it
					if (frame == -1) {
						int minIdx = 0;
						int prevPage;
						for (i = 1; i < 128; ++i) {
							if (lru[minIdx] > lru[i])
								minIdx = i;
						}
						frame = minIdx;
						lru[minIdx] = curTime;
						prevPage = pageTable[minIdx];
						pageTable[minIdx] = page;


						for(i=0;i<16;++i)
							if (tlb[i][0] == prevPage) {
								tlb[i][0] = -1;
								break;
							}
					}

					readContentIntoMemory(bin, page, frame, phyMemory);

					//update tlb 
					for (i = 0; i < 16; ++i) {
						if (tlb[i][0] == -1) {
							tlb[i][0] = page;
							tlb[i][1] = frame;
							break;
						}
					}
					if (i == 16) {
						for (i = 1; i < 16; ++i) {
							tlb[i - 1][0] = tlb[i][0];
							tlb[i - 1][1] = tlb[i][1];
						}
						tlb[15][0] = page;
						tlb[15][1] = frame;
					}
				}

			}

			int temp = phyMemory[frame * 256 + offset];

			++count;
			fault_count += fault;
			hit_count += hit;

			fprintf(out, "%d,%d,%d\n", logical, frame * 256 + offset, temp);
		}
	}
	fclose(fp);
	fclose(bin);
	fclose(out);

	fault_rate = (1.0*fault_count/count) ;
	hit_rate = (1.0*hit_count/count);
	printf("Page Faults = %d\n", fault_count);
	printf("Page Faults Rate = %.4f\n", fault_rate);
	printf("TLB Hits = %d\n", hit_count);
	printf("TLB Hits Rate= %.4f\n", hit_rate);

	return 0;
}

