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


#### Tasks
- [ ] Page table frames not to be loaded into memory.
- [x] Add comments: Splitting of physical addr into cache numbers.
- [ ] Complete the README.
- [ ] Convert README.md to README.txt
- [ ] Make folder name 6.tar.gz before submission
- [ ] Modularise entire code base so that the driver function has at max 20 lines of code.
- [ ] Pre-condition and post-condition for every function.


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
* There is also a statistics file which gives an idea about the processes that just ran.


#### How to compile
* Make the necessary changes in input.txt (number of processes and input files corresponding to them).
* Make the changes (if required) in configuration.h as per one's requirement.
* Run "make" on the terminal in the working directory of the program.
* Run the executable file name on the terminal to run the compiled program.
* To clean the compilation, run "make clean" on the terminal.


#### Description of structure of program
* The program first initialises the various hardware being simulated.(i.e. Cache, TLBs etc)
* Then it creates the necessary inputs for the segmenation table. (i.e. it extracts the segmentNumber from the input files and stores it in a different file with prefix "Segment_")
* Next it initializes Frame Table, Global Descriptor Tables etc.
* Then the program enqueues all processes in READY queue, it also initializes the PCB, which inturn initializes the SegmentTable, for each segment the Page Tables are also initalized and pre-paging is done. 
* Then it starts reading input from one process file at a time (till there is a context switch due to preemption or due to a page-fault).
* Each memory access is put across two levels of TLB (while managing the misses in both levels).
* Once the L1-TLB is updated to have the required page number, we start checking for the data in the cache using the specified methods (look-through, look-aside, write-through and write-back).
* If there is a miss in the caches, we update the various tables pertaining to the main memory. If that is also a miss, then we do a repair the page-fault and in the mean-time carry on with another process.
* There are a few other steps not pertaining directly to the required data, but indirectly through data required to get the required output (data in the various tables-- Page table, frame table and segmentation table) which will happen routinely whenever it is required.
* Once all the processes have been successfully completed, the simluation program terminates.


#### Limitations
* Since this is a simulation, we have not actually implemented parallel searches, etc. We have just assumed them to be parallel and taken the total time to be not the added sum, but happening at the same time.
* We assume that the input given to us is the linear address data directly using which we create segmentation input data, so in essense segmentation + paging is not really being simulated in the best possible manner.
* We assume that the number of memory accesses a process will request at once before it gets preempted is constant. That does not usually happen in practical systems as the amount of time for memory accesses will vary and hence so will the number of memory accesses a process can request before preemption.
* The program also assumes that the page fault is serviced before a process gets back into running state in the cpu (which is not really practical).
* The simulation assumes that there are only two segments at play-- one in GDT and another in LDT.


#### Assumptions
1. Virtual Memory contains the complete program. Hard disk is big enough to hold all the programs.
2. First 2 blocks of the process (assume the page size and frame size same and is 1KB) will be pre-paged into main memory before a process starts its execution.
3. All other pages are loaded on demand [Assume the system supports dynamic loading. Also assume that the system has no dynamic linking support].
4. Main memory follows Global replacement. Lower limit number of pages and upper limit number of pages per process should be strictly maintained.
5. Page tables of all the processes reside in main memory and will not be loaded into cache memory levels.


#### Bugs


#### What could have been added
* A minimal scheduler to check if a process has finished servicing the page fault before it might get a turn in the CPU.
* For implementing search through the pageTable and pageFault handling, we have used the fact that we have all the frames in a level of pageTable sequentially accessible in our ADT. However it would have been better if we had implemented it the conventional way. (Assuming Frames not sequentially accessible for a level of pageTable)
