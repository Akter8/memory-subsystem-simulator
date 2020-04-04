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


#### Tasks
- [ ] Add comments: Splitting of physical addr into cache numbers.
- [ ] Complete the README.
- [ ] Convert README.md to README.txt
- [ ] Make folder name 6.tar.gz before submission
- [ ] Modularise entire code base so that the driver function has at max 20 lines of code.


#### Description of files
* configuration.h - Contains the configurations of static data required for this program.
* dataTypes.h - The data types made and used by use.
* tlb.h - Contains the ADT of both the TLBs.
* tlb.c - Contains the implementation of the functions related to both levels of the TLB ADT.
* cache.h - Contains the ADT of both the levels of cache.
* cache.c - Contains the implementation of the functions related to both levels of the cache ADT.
* pcb.h - Contains the ADT of the Process Control Block.
* pcb.c - Contains the implementation of the functions related to the PCB.
* frameTable.h - Contains the ADT of the frame table and its entries.
* frameTable.c - Containts the function related to the frame table ADT. It also contains the functions related to modelling the MM.
* pageTable.h - Contains the ADT of the page table and its entries.
* pageTable.c - Contains the implementation of the ADT of the page table.
* segmentTable.h - Contains the ADT of the segment table and its entries.
* segmentTable.c - Contains the implementation of the segment table and its entries.
* utility.h - Contains the header files for utility functions and macros.
* utilityFunctions.c - Contains a few function definitions of utility functions.
* mainDriver.c - Contains just a small main function.
* kernel.c - Contains the driver() function which can be seen as the medium between the simulated h/w and the requested data.


##### Input Files
* Input files must contain linear addresses.
* A number of processes and the list of the input files must be writen in the main input file.
* For every process input file, there will be a corresponding file created with the same name but with a prefix of Segment_. This file will contain the segment table input and also a psuedo randomly generated bit which differentiates between a read and a write memory access.
* configuration.h also acts as an input file for various static parameters in the program.

##### Output Files
* Once the program has been run, all the output goes into the output file specified in configuration.h


#### How to compile
* Make the necessary changes in input.txt (number of processes and input files corresponding to them).
* Make the changes (if required) in configuration.h as per one's requirement.
* Run "make" on the terminal in the working directory of the program.
* Run the executable file name on the terminal to run the compiled program.
* To clean the compilation, run "make clean" on the terminal.


#### Description of structure of program
* The program first initialises the various hardware being simulated.
* Then it creates the necessary inputs for the segmenation table.
* Then it starts reading input from one process file at a time (till there is a context switch due to preemption or due to a page-fault).
* Each memory access is put across two levels of TLB (while managing the misses in both levels).
* Once the L1-TLB is updated to have the required page number, we start checking for the data in the cache using the specified methods (look-through, look-aside, write-through and write-back).
* If there is a miss in the caches, we update the various tables pertaining to the main memory. If that is also a miss, then we do a repair the page-fault and in the mean-time carry on with another process.
* There are a few other steps not pertaining directly to the required data, but indirectly through data required to get the required output (data in the various tables-- Page table, frame table and segmentation table) which will happen routinely whenever it is required.
* Once all the processes have been successfully completed, the simluation program terminates.


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
