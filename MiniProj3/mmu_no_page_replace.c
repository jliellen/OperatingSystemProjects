#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void resolve(int logical, int *page, int *offset) {
	*page = (logical & 0XFF00) >> 8;
	*offset = (logical & 0XFF);
}

int readContentIntoMemory(FILE *bin, int page, int frameUsedCount, char *phyMemory) {
	char content[256];
	fseek(bin, page * 256, SEEK_SET);
	fread(content, 256, 1, bin);
	memcpy(phyMemory + frameUsedCount * 256, content, 256);
	return frameUsedCount;
}

int main(int argc, char *argv[]) {
	int pageTable[256];
	int tlb[16][2];
	char phyMemory[65536];
	int i;
	int logical;
	int page, offset;
	int frame;
	int frameUsedCount = 0;
	int hit, fault;
	int fault_count = 0;
	int hit_count = 0;
	float fault_rate;
	float hit_rate;
	int count = 0;
	FILE *bin, *fp;
	FILE *out;

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

	out = fopen("output_no_page_replace.csv", "w");
	if (out == NULL) {
		fprintf(stderr, "open the file \"output_no_page_replace.csv\"failed.\n");
		exit(EXIT_FAILURE);
	}

	for (i = 0; i < 256; ++i)
		pageTable[i] = -1;
	for (i = 0; i < 16; ++i)
		tlb[i][0] = tlb[i][1] = -1;


	while (!feof(fp)) {
		if (fscanf(fp, "%d", &logical) == 1) {
			resolve(logical, &page, &offset);

			hit = fault = 0;

			frame = -1;
			for (i = 0; i < 16; ++i) {
				if (tlb[i][0] == page) {
					frame = tlb[i][1];
					hit = 1;
					break;
				}
			}

			if (frame == -1) {
				frame = pageTable[page];
				if (frame == -1) {
					fault = 1;
					frame = readContentIntoMemory(bin, page, frameUsedCount, phyMemory);
					pageTable[page] = frame;
					++frameUsedCount;

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

	fault_rate = (1.0*fault_count/count) ;
	hit_rate = (1.0*hit_count/count);
	printf("Page Faults = %d\n", fault_count);
	printf("Page Faults Rate = %.4f\n", fault_rate);
	printf("TLB Hits = %d\n", hit_count);
	printf("TLB Hits Rate= %.4f\n", hit_rate);

	return 0;
}

