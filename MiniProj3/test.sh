#!/bin/bash -e
echo "Compiling the first project without page replacement"
gcc mmu_no_page_replace.c -o mmu_no_page_replace
echo "Compiling the second project with page replacement"
gcc mmu_with_page_replace.c -o mmu_with_page_replace

echo "--------------------------------------------------------------------------"
echo "Running the first project (without page replacement)"
./mmu_no_page_replace BACKING_STORE.bin addresses.txt

echo "--------------------------------------------------------------------------"
echo "Running the second project (page replacement)"
./mmu_with_page_replace BACKING_STORE.bin addresses.txt

echo " "