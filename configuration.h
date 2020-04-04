#define OUTPUT_FILE_NAME "outputFile.txt"
#define STATISTICS_FILE_NAME "outputFile_processStatistics.txt"


// Size of address configurations.
#define PAGE_NUM_ADDR_SIZE 22
#define FRAME_NUM_ADDR_SIZE 16


// TLB Configurations.
#define NUM_ENTRIES_IN_L1_TLB 12
#define NUM_ENTRIES_IN_L2_TLB 24
#define ERROR_PAGE_NUM_NOT_FOUND 1


// Cache Configurations.
#define NUM_SETS_IN_L1_CACHE 32
#define NUM_SETS_IN_L2_CACHE 64
#define NUM_WAYS_IN_L1_CACHE 4
#define NUM_WAYS_IN_L2_CACHE 8
#define LOG_NUM_WAYS_IN_L2_CACHE 3
#define MAX_VALUE_IN_8_BITS 255
#define VALUE_OF_8_BITS_WITH_FIRST_BIT_SET 128
#define ERROR_WRITE_FAILED_NO_TAG_MATCH -1
#define ERROR_WRITE_FAILED_NO_PERMISSION -2
#define ERROR_CANNOT_WRITE_IN_INSTR_CACHE -3


// Time taken for every action.
// Time for L1 < time for L2 in the same category.
// Search time < write time.
// Update time > search time, update time < write time.
#define L1_TLB_SEARCH_TIME 1
#define L2_TLB_SEARCH_TIME 5

#define L1_TLB_UPDATE_TIME 3
#define L2_TLB_UPDATE_TIME 8

#define L1_CACHE_SEARCH_TIME 4
#define L2_CACHE_SEARCH_TIME 6

#define L1_CACHE_WRITE_TIME 8
#define L2_CACHE_WRITE_TIME 10 

#define L1_CACHE_UPDATE_TIME 6
#define L2_CACHE_UPDATE_TIME 8

#define MAIN_MEMORY_WRITE_TIME 50
#define MAIN_MEMORY_SEARCH_TIME 35
#define PAGE_FAULT_RECOVERY_TIME 1000