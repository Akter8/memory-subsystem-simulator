# DSTN Assignment-1
Group #6, Questions #5.

### Authors
* Akhil Tarikere, 2017A7PS1916G
* Chiplunkar Chinmay Vidyadhar, 2017A7PS0097G
* Prabhu Divya Umesh, 2017A7PS0001G
* Prakarsh Parashar, 2016B4A70776G

#### Question
* The memory subsystem [with TLBs, L1 Cache, L2 Cache and Main Memory] has following configuration:
* TLBs: L1 TLB with 12 entries and L2 TLB with 24 entries. Invalidation / Flush takes place at each preemption point.
* L1 Cache: 8KB, 32B, 4 Way set associative cache. The cache follows Write through and Look aside. It follows LRU Square matrix as replacement policy.
* L2 Cache: 32KB, 64B, 8 Way set associative cache. The cache follows Write back and look through. It follows LRU Counter as replacement policy.
* Main Memory with Memory Management: 64MB Main memory with LFU [with aging] as replacement policy. The memory management scheme used is Segmentation + Paging.


#### Doubts
* min(t1, t2) or max(t1, t2) in look aside?
* Size of physical and virtual addresses.
* Should the user enter number of processes as an input to our program? Or should we be able to handle  any number of processes arriving at any time?


#### Programming Tasks
- [ ] TLB
- [ ] Cache
- [ ] MM: Segmentation
- [ ] MM: Paging


#### Tasks
- [ ] Create a Makefile.
- [ ] Complete the README.
- [ ] Convert README.md to README.txt
- [ ] Make folder name 6.tar.gz before submission
- [ ] Modularise entire code base so that the driver function has at max 20 lines of code.


#### Description of files
* Inputs (folder) - Contains the input files. Every file has an equivalent (Segment_File.txt) which contains the segment table address and whether the memory access is a read or write.
* dataTypes.h - The data types made and used by use.
* tlb.h - Contains the ADT of both the TLBs.
* tlb.c - Contains the implementation of the functions related to both levels of the TLB ADT.
* cache.h - Contains the ADT of both the levels of cache.
* cache.c - Contains the implementation of the functions related to both levels of the cache ADT.
* pcb.h - Contains the ADT of the Process Control Block.
* frameTable.h - Contains the ADT of the frame table and its entries.
* pageTable.h - Contains the ADT of the page table and its entries.
* segmentTable.h - Contains the ADT of the frame table and its entries.

#### How to compile


#### Description of structure of program


#### Things to keep in mind
1. https://www.kernel.org/doc/html/v4.10/process/coding-style.html,
2. https://developer.gnome.org/programming-guidelines/stable/c-coding-style.html.en,
3. Pre-condition,
4. Post-condition and a
5. Loop invariant for every loop.


#### Bugs
- [ ] PageTableBaseAddress entry in pcb.h is not required
- [ ] In SegmentTable.h limit is 26 bits. It should be 32 bits. Change to be made and related code, dependent on it, to be fixed.

#### Assumptions
1. Virtual Memory contains the complete program. Hard disk is big enough to hold all the programs.
2. First 2 blocks of the process (assume the page size and frame size same and is 1KB) will be pre-paged into main memory before a process starts its execution.
3. All other pages are loaded on demand [Assume the system supports dynamic loading. Also assume that the system has no dynamic linking support].
4. Main memory follows Global replacement. Lower limit number of pages and upper limit number of pages per process should be strictly maintained.
5. Page tables of all the processes reside in main memory and will not be loaded into cache memory levels.
