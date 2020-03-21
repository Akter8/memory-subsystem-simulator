#define OUTPUT_FILE_NAME "outputFile.txt"


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


//---------------- Update required.
// Hit/miss times
#define TLB_SEARCH_TIME 0
#define L1_CACHE_SEARCH_TIME 0
#define L2_CACHE_SEARCH_TIME 0