# DSTN Assignment-1
Group #6, Questions #5.

#### Authors
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


#### Programming Tasks
- [ ] TLB
- [ ] Cache
- [ ] MM: Segmentation
- [ ] MM: Paging


#### Tasks
- [ ] Complete the README.
- [ ] Convert README.md to README.txt
- [ ] Make folder name 6.tar.gz before submission


#### Description of files


#### How to compile


#### Description of structure of program


#### Bugs


#### Assumptions
1. Virtual Memory contains the complete program. Hard disk is big enough to hold all the programs.
2. First 2 blocks of the process (assume the page size and frame size same and is 1KB) will be pre-paged into main memory before a process starts its execution.
3. All other pages are loaded on demand [Assume the system supports dynamic loading. Also assume that the system has no dynamic linking support].
4. Main memory follows Global replacement. Lower limit number of pages and upper limit number of pages per process should be strictly maintained.
5. Page tables of all the processes reside in main memory and will not be loaded into cache memory levels.