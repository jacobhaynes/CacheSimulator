#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <execinfo.h>
#include <signal.h>
#include "FastList.h"
#include <inttypes.h>


#define DEBUG_PRINTF 0
#define PROGRESS_PRINTF 0
#define PRINT_OUTPUT 1

#define NOT_FOUND 0
#define FOUND 1

#define BYTE 8

#define ADDRESS_SPACE 32 

uint64_t loadInst = 0; 
uint64_t bracnhInst = 0; 
uint64_t storeInst = 0; 
uint64_t compInst = 0; 

uint64_t loadCyc = 0; 
uint64_t bracnhCyc = 0; 
uint64_t storeCyc = 0; 
uint64_t compCyc = 0; 

uint64_t L1I_hit = 0; 
uint64_t L1I_miss = 0; 
uint64_t L1I_evictions = 0;

uint64_t L1D_hit = 0; 
uint64_t L1D_miss = 0; 
uint64_t L1D_evictions = 0;

uint64_t L2_hit = 0; 
uint64_t L2_miss = 0; 
uint64_t L2_evictions = 0; 

uint64_t executionTime = 0; 
uint64_t instructionCount = 0;

uint32_t memoryCost = 0;



typedef enum CacheType 
{
	DirectMapped, 
	TwoWaySetAssociative, 
	FourWaySetAssociative,
	FullyAssociative
} CacheType; 

typedef struct _Blocks
{
	uint32_t address; 
	bool valid; 
	_Blocks()
	{
		valid = false; 
	}
} EvictedBlocks, LoadBlocks, StoreBlocks; 

typedef enum LoadStore
{
	LoadAddress,
	StoreAddress
} LoadStore;  


typedef struct SetAssociativeEntity
{
	bool valid; 
	bool dirty;  
	uint32_t tag; 
	SetAssociativeEntity()
	{
		valid = false;
		dirty = false; 
	}
} SetAssociativeEntity;

typedef struct SetAssociative
{
	FastList<SetAssociativeEntity> * lruTable; 
	
	uint32_t indexBits;
	uint32_t tagBits; 
	uint32_t sets; 
	uint32_t setSize; 
} SetAssociative ;

typedef struct Cache
{
	bool instructionCache; 
	
	uint32_t blockSize; 	
	uint32_t hitTime; 
	uint32_t missTime; 
	SetAssociative * setAssociative;

	bool (*store)(Cache* cache, 
					  uint32_t address, 
					  EvictedBlocks& evicted, 
					  LoadBlocks& loadNeeded);
					  
	bool (*load) (Cache* cache, 
					  uint32_t address, 
					  EvictedBlocks& evicted, 
					  LoadBlocks& loadNeeded);
} Cache;


typedef struct _Configuration 
{
	//Variable-
	CacheType L1_Icache_type; 
	uint32_t L1_Icache_size;
	CacheType L1_Dcache_type;
	uint32_t L1_Dcache_size;
	CacheType L2_Ucache_type;
	uint32_t L2_Ucache_size;
	//Static-
	uint32_t L1_block_size;
	//uint32_t L1_cache_size;
	uint32_t L1_assoc;
	uint32_t L1_hit_time;
	uint32_t L1_miss_time;
	uint32_t L2_block_size;
	//uint32_t L2_cache_size;
	uint32_t L2_assoc;
	uint32_t L2_hit_time;
	uint32_t L2_miss_time;
	uint32_t L2_trasnfer_time;
	uint32_t L2_bus_width;
	uint32_t mem_sendaddr;
	uint32_t mem_ready;
	uint32_t mem_chunktime;
	uint32_t mem_chunksize;
	uint32_t L2_block_transfer_time; 
	} Configuration; 

bool loadConfig(const char* filename, Configuration & config);
bool executeSimulator(const Configuration& config, 
						  	 Cache* L1I, 
						    Cache* L1D, 
						    Cache* L2);
						   
					  
bool setLoad(Cache* cache, 
								uint32_t address, 
								EvictedBlocks& evicted, 
								LoadBlocks& loadNeeded);

bool setStore(Cache* cache, 
					  uint32_t address, 
					  EvictedBlocks& evicted, 
					  LoadBlocks& loadNeeded); 
					  
bool setLoadStore(Cache* cache, 
									  uint32_t address, 
									  EvictedBlocks& evicted, 
									  LoadBlocks& loadNeeded, 
									  bool dirtyBitValue); 
					 
bool setInit(Cache* cache, 
				 uint32_t blockCount, 
				 uint32_t blockSize, 
				 uint32_t hitTime, 
				 uint32_t missTime, 
				 uint32_t setSize); 
				 
bool setInvalidate(Cache* cache, 
						 uint32_t address);

CacheType convertCacheType(const char* cacheString); 

uint32_t calculateActualSize(const char* inputSize);

bool setupSimulator(Configuration config, 
						  Cache* L1I, 
						  Cache* L1D, 
						  Cache* L2); 
						  
bool setupSinglCache(Cache* cache, 
							CacheType type, 
							uint32_t cacheSize,  
							uint32_t blockSize, 
							uint32_t hitTime, 
							uint32_t missTime); 
							
uint64_t executeSingleInstruction(const Configuration& config, 
						  	 			Cache* L1I, 
						    			Cache* L1D, 
						    			Cache* L2, 
						    			char op, 
						    			uint32_t address, 
						    			uint32_t execInfo);
						    			
uint64_t executeLoad(const Configuration& config, 
						  	 			Cache* L1,  
						    			Cache* L2, 
						    			Cache* otherL1, 
						    			uint32_t address);
						    			
uint64_t executeLoadOrStore(const Configuration& config, 
						  	 			Cache* L1,  
						    			Cache* L2, 
						    			Cache* otherL1, 
						    			uint32_t address, 
						    			LoadStore loadOrStore);

uint64_t executeStore(const Configuration& config, 
							 Cache* L1,  
							 Cache* L2, 
						    Cache* otherL1, 
							 uint32_t address);

uint32_t calculateL2BlockTransferTime(const Configuration& config);

uint32_t calculateMainMemoryTime(const Configuration& config,
									  uint32_t bytesToTransfer);

uint32_t calculateIndexBits(uint32_t address, 
									 uint32_t addressSize, 
									 uint32_t indexBits, 
									 uint32_t tagBits);

uint32_t calculateTagBits(uint32_t address, 
								  uint32_t addressSize, 
								  uint32_t indexBits, 
								  uint32_t tagBits);

uint32_t calculateMemoryCost(const Configuration & config,
                             const Cache L1I,
                             const Cache L1D,
                             const Cache L2);

void printOutput(); 
void printCache(Cache* cache);
void printMemory(Cache* L1I, Cache* L1D, Cache* L2); 
void printWithComma(uint64_t number);

void handler(int sig) {
  void *array[10];
  size_t size;
  
  printOutput(); 
  
  printf("--------ERROR--------\n");

  // get void*'s for all entries on the stack
  size = backtrace(array, 10);

  // print out all the frames to stderr
  fprintf(stderr, "Error: signal %d:\n", sig);
  backtrace_symbols_fd(array, size, 2);
  exit(1);
}

int main(int argc,char* argv[])
{ 
	signal(SIGSEGV, handler);

	if(isatty(STDIN_FILENO))
	{
		printf("No file"); 
		exit(1); 
	}
	#if DEBUG_PRINTF == 1
	printf("argc %d\n", argc); 
	#endif

	Configuration config;
	Cache L1I, L1D, L2;
	L1I.instructionCache = true; 
	L1D.instructionCache = false; 
	L2.instructionCache = false; 
	
  loadConfig(argv[1], config); 
	setupSimulator(config, &L1I, &L1D, &L2);
  memoryCost = calculateMemoryCost(config, L1I, L1D, L2); 
	executeSimulator(config, &L1I, &L1D, &L2);
	return 0; 
}

bool setupSimulator(Configuration config, 
						  Cache* L1I, 
						  Cache* L1D, 
						  Cache* L2)
{	
	setupSinglCache(L1I, 
						 config.L1_Icache_type, 
						 config.L1_Icache_size, 
						 config.L1_block_size,
						 config.L1_hit_time,
						 config.L1_miss_time); 
	
	setupSinglCache(L1D, 
						 config.L1_Dcache_type, 
						 config.L1_Dcache_size, 
						 config.L1_block_size,
						 config.L1_hit_time,
						 config.L1_miss_time); 
						 	
	setupSinglCache(L2, 
						 config.L2_Ucache_type, 
						 config.L2_Ucache_size, 
						 config.L2_block_size,
						 config.L2_hit_time,
						 config.L2_miss_time); 
	
	return true; 
}

bool setupSinglCache(Cache* cache, 
							CacheType type, 
							uint32_t cacheSize, 
							uint32_t blockSize, 
							uint32_t hitTime, 
							uint32_t missTime)
{
	int blockCount = ceil(double(cacheSize)/double(blockSize)); 
	if(type == DirectMapped)
	{
		return setInit(cache, blockCount, blockSize, hitTime, missTime, 1);
	}
	else if (type == TwoWaySetAssociative)
	{
		return setInit(cache, blockCount, blockSize, hitTime, missTime, 2); 
	}
	else if (type == FourWaySetAssociative)
	{
		return setInit(cache, blockCount, blockSize, hitTime, missTime, 4); 
	}
	else if (type == FullyAssociative)
	{
		return setInit(cache, blockCount, blockSize, hitTime, missTime, blockCount); 
	}
	return false; 
}

bool setInit(Cache* cache, 
					 uint32_t blockCount, 
					 uint32_t blockSize, 
					 uint32_t hitTime, 
					 uint32_t missTime,
					 uint32_t setSize) 
{
	uint32_t sets = blockCount/setSize; 
	
	cache->load = setLoad;
	cache->store = setStore; 
	cache->blockSize = blockSize; 
	cache->hitTime = hitTime; 
	cache->missTime = missTime; 
	
	cache->setAssociative = new SetAssociative();
	cache->setAssociative->sets = sets; 
	cache->setAssociative->setSize = setSize; 
	
	#if DEBUG_PRINTF == 1
	printf("Sets %u, Block Count %u, Block Size %u\n", sets, blockCount, blockSize); 
	#endif
	
	uint32_t setSizeBits = log(sets)/log(2); 
	uint32_t blockSizeBits = log(blockSize)/log(2);
	
	#if DEBUG_PRINTF == 1
	printf("setSize %u, blockSize %u\n", setSizeBits, blockSizeBits);
	#endif
	
	cache->setAssociative->tagBits = ADDRESS_SPACE - blockSizeBits - setSizeBits; 
	cache->setAssociative->indexBits = setSizeBits;  
	
	#if DEBUG_PRINTF == 1 
	printf("Tag Bits %d, Index Bits %d\n", 
				cache->setAssociative->tagBits, 
				cache->setAssociative->indexBits); 
	#endif
	
	cache->setAssociative->lruTable = new FastList<SetAssociativeEntity>[sets]();
	 
	return true; 
}

void printOutput()
{
	
	printf("Final Execution Time: %" PRIu64 "\n", executionTime); 
	printf("instructionCount: %" PRIu64 "\n", instructionCount); 
	
	printf("loadInst %" PRIu64 "\n", loadInst); 
	printf("bracnhInst %" PRIu64 "\n", bracnhInst); 
	printf("storeInst %" PRIu64 "\n", storeInst);
	printf("compInst %" PRIu64 "\n\n", compInst); 
	
	printf("loadCyc %" PRIu64 "\n", loadCyc); 
	printf("bracnhCyc %" PRIu64 "\n", bracnhCyc); 
	printf("storeCyc %" PRIu64 "\n", storeCyc); 
	printf("compCyc %" PRIu64 "\n\n", compCyc); 
	
	printf("L1I_hit %" PRIu64 "\n", L1I_hit); 
	printf("L1I_miss %" PRIu64 "\n", L1I_miss); 
	printf("L1I_evictions %" PRIu64 "\n\n", L1I_evictions); 
	
	printf("L1D_hit %" PRIu64 "\n", L1D_hit); 
	printf("L1D_miss %" PRIu64 "\n", L1D_miss); 
	printf("L1D_evictions %" PRIu64 "\n\n", L1D_evictions); 
	
	printf("L2_hit %" PRIu64 "\n", L2_hit);
	printf("L2_miss %" PRIu64 "\n", L2_miss); 
	printf("L2_evictions %" PRIu64 "\n\n", L2_evictions);
  
  printf("Total Cost %" PRIu32 "\n", memoryCost);
	return; 
}
 
void printMemory(Cache* L1I, Cache* L1D, Cache* L2)
{
	printf("=========================L1I=========================\n");
	printCache(L1I);
	printf("=========================L1D=========================\n");
	printCache(L1D);
	printf("=========================L2==========================\n");
	printCache(L2); 
}

void printWithComma(uint64_t number)
{
  uint32_t numbersSplit[16];
  int ii = 0; 
  while(number > 0)
  {
    numbersSplit[ii] = number % 1000; 
    number /= 1000;
    ii++;  
  }
  
  if(ii > 0)
  {
    ii--; 
    printf("%u", numbersSplit[ii]); 
  }
  else 
  {
    printf("0"); 
  }
  ii--;  
  while(ii >= 0)
  {
     printf(",%03u", numbersSplit[ii]); 
     ii--; 
  }
}

void printCache(Cache* cache)
{
	SetAssociative * memory = cache->setAssociative; 
	for(uint32_t ii = 0; ii < memory->sets; ii++)
	{	
		if( memory->lruTable[ii].length() > 0)
		{
		  if(memory->lruTable[ii].length() > 4)
		  {
			  printf("Index: 0x%x ", ii);
			}
			for(uint32_t jj = 1; jj <= memory->lruTable[ii].length(); jj++)
			{
				  printf("| V:%i D:%i Tag: 0x%x \t",
				 	memory->lruTable[ii].get(jj).valid, 
				 	memory->lruTable[ii].get(jj).dirty,
				 	memory->lruTable[ii].get(jj).tag); 
				 	if(memory->lruTable[ii].length() > 4)
				 	{
				 	  //if it is fully-associative
				 	  printf("\n"); 
				 	}
			}  
			
			printf("\n"); 
		}
		else
		{
			//printf("Index: 0x%x | Empty\n", ii); 
		}
				 
	}
}

bool executeSimulator(const Configuration& config, 
						  	 Cache* L1I, 
						    Cache* L1D, 
						    Cache* L2)
{
	char op; 
	uint32_t address; 
	uint32_t execInfo; 
	
	for(int ii = 0; ii < 3; ii++)
	{
		#if DEBUG_PRINTF == 1
		printf("----------------------------------------\n");
		#endif
	}
	
	char buffer[65536];//
	setvbuf(stdin, buffer, _IOFBF, sizeof(buffer)); 
	 	
	while(scanf("%c %x %x\n", &op, &address, &execInfo) == 3)
	{ 
		#if DEBUG_PRINTF == 1
		printf("===============================================\n");
		printf("%c 0x%x 0x%x\n", op, address, execInfo);
		#endif
		executionTime += executeSingleInstruction(config, L1I, L1D, L2, op, address, execInfo); 
		
		#if DEBUG_PRINTF == 1
			printf("----------------------\ninstructionCount %" PRIu64 ", executionTime %" PRIu64 "\n"
					, instructionCount, executionTime);
		#else
		#if PROGRESS_PRINTF > 0
		if(instructionCount % PROGRESS_PRINTF == 0)
		{
		  printf("instructionCount "); 
		  printWithComma(instructionCount);
		  printf(", executionTime "); 
		  printWithComma(executionTime); 
		  printf("\n"); 
		}
		#endif
		#endif 
		instructionCount++; 
	}
	#if PRINT_OUTPUT == 1
		printMemory(L1I, L1D, L2);
		printOutput(); 
	#endif
	return true;
}

uint64_t executeSingleInstruction(const Configuration& config, 
						  	 			Cache* L1I, 
						    			Cache* L1D, 
						    			Cache* L2, 
						    			char op, 
						    			uint32_t address, 
						    			uint32_t execInfo)
{	
	//Execution time is always star
	uint64_t instructionTime = 0;
	
	uint64_t executionTimeSingle = 0; 
	
	#if DEBUG_PRINTF == 1
	printf("---Load Instruction---\n"); 
	#endif 
	// Loads the Instruction from the Program Counter 
	instructionTime += executeLoad(config, L1I, L2, L1D, address);  
	if(op == 'L')
	{
		loadInst++; 
		#if DEBUG_PRINTF == 1
		printf("---Load---\n");
		#endif
		// Loads what it tells us to load
		instructionTime += executeLoad(config, L1D, L2, L1I, execInfo); 
		loadCyc += instructionTime; 
	}
	else if (op == 'S')
	{
		storeInst++; 
		#if DEBUG_PRINTF == 1
		printf("---Store---\n");
		#endif
		instructionTime += executeStore(config, L1D, L2, L1I, execInfo); 
		storeCyc += instructionTime;  
	}
	else if (op == 'B')
	{
		bracnhInst++; 
		#if DEBUG_PRINTF == 1
		printf("---Branch---\n");
		#endif
		instructionTime += 1;
		bracnhCyc += instructionTime;
		
	}
	else if (op == 'C')
	{
		compInst++; 
		#if DEBUG_PRINTF == 1
		printf("---Compute---\n");
		#endif
		instructionTime += execInfo; 
		compCyc += instructionTime;
	}
	executionTimeSingle += instructionTime; 
	#if DEBUG_PRINTF == 1
	printf("Execution time for instruction : %" PRIu64 "\n", executionTimeSingle); 
	#endif 
	return executionTimeSingle; 
}

uint64_t executeLoad(const Configuration& config, 
						  	 			Cache* L1,  
						    			Cache* L2, 
						    			Cache* otherL1, 
						    			uint32_t address)
{
	return executeLoadOrStore(config, L1, L2, otherL1, address, LoadAddress); 			
}

uint64_t executeStore(const Configuration& config, 
						  	 			Cache* L1,  
						    			Cache* L2,
						    			Cache* otherL1, 
						    			uint32_t address)
{
	return executeLoadOrStore(config, L1, L2, otherL1, address, StoreAddress); 			
}


uint64_t executeLoadOrStore(const Configuration& config, 
						  	 			Cache* L1,  
						    			Cache* L2, 
						    			Cache* otherL1, 
						    			uint32_t address, 
						    			LoadStore loadStore)
{

	uint64_t executionTimeLoad = 0; 
	EvictedBlocks evicted, L2evicted; 
	LoadBlocks loadNeeded, L2loadNeeded; 
	
	if(loadStore == LoadAddress)
	{
		#if DEBUG_PRINTF == 1
		printf("Loading\n"); 
		#endif
		L1->load(L1, address, evicted, loadNeeded); 
	}
	else 
	{
		#if DEBUG_PRINTF == 1
		printf("Storing\n"); 
		#endif
		L1->store(L1, address, evicted, loadNeeded); 
	}
	
	if(!loadNeeded.valid) 
	{
		#if DEBUG_PRINTF == 1
		printf("Hit in L1 Cache\n"); 
		#endif
		
		if(L1->instructionCache)
		{
			L1I_hit++;
		}
		else 
		{
			L1D_hit++;
		}
		//If we don't miss, we only need hit load time
		executionTimeLoad += L1->hitTime;
		return executionTimeLoad;
	}
	else
	{
		if(L1->instructionCache)
		{
			L1I_miss++;
		}
		else 
		{
			L1D_miss++;
		}
		#if DEBUG_PRINTF == 1
		printf("Miss in L1\n"); 
		#endif
		//if we missed, get the miss time
		executionTimeLoad += L1->missTime;
	}
	
	
	if(evicted.valid )
	{		
		if(L1->instructionCache)
		{
			L1I_evictions++;
		}
		else 
		{
			L1D_evictions++;
		}
		
		L2->store(L2, evicted.address, L2evicted, L2loadNeeded);	
		//Moving from L1 to L2
		executionTimeLoad += config.L2_block_transfer_time;
		// Store in L2
		executionTimeLoad += L2->hitTime; 
		
		#if DEBUG_PRINTF == 1
		printf("Evicting from L1\n"); 
		#endif
		
		if(L2loadNeeded.valid)
		{
			L2_miss++;
			//Load from main memory
			executionTimeLoad += L2->missTime ;
			executionTimeLoad += calculateMainMemoryTime(config, L2->blockSize); 
			#if DEBUG_PRINTF == 1
			printf("Hit in L1 but not L2\n"); 
			#endif
		}
		else
		{
			L2_hit++; 
		}
		
		if(L2evicted.valid)
		{
			L2_evictions++; 
			
			//Move block to Main Memory
			executionTimeLoad += calculateMainMemoryTime(config, L2->blockSize); 
			
			#if DEBUG_PRINTF == 1
			printf("Evicting from L2\n"); 
			#endif
		}
	}
	
	if(loadNeeded.valid)
	{
		L2->load(L2, loadNeeded.address, L2evicted, L2loadNeeded);
		
				#if DEBUG_PRINTF == 1
		printf("Loading from L2\n"); 
		#endif
		
		if(L2evicted.valid)
		{
			L2_evictions++;
			//Move block to Main Memory
			executionTimeLoad += calculateMainMemoryTime(config, L2->blockSize); 
			
			#if DEBUG_PRINTF == 1
			printf("Evicting from L2\n"); 
			#endif
		}
		
		if(L2loadNeeded.valid)
		{
			L2_miss++; 
			// Miss Time
			executionTimeLoad += L2->missTime; 
			// Transfer Time from Main Memory
			executionTimeLoad += calculateMainMemoryTime(config, L2->blockSize); 
			#if DEBUG_PRINTF == 1
			printf("Miss in L2\n"); 
			#endif
		}
		else 
		{
			L2_hit++; 
		}
		
		// Load block from L2
		executionTimeLoad += L2->hitTime;
		// Transfer Block
		executionTimeLoad += config.L2_block_transfer_time; 
		// Store in L1
		executionTimeLoad += L1->hitTime;		
	}
	
	#if DEBUG_PRINTF == 1
	if(loadStore == LoadAddress)
	{
		printf("Load Time: %" PRIu64 "\n", executionTimeLoad); 
	}
	else 
	{
		printf("Store Time: %" PRIu64 "\n", executionTimeLoad); 
	}

	#endif
	return executionTimeLoad; 
}

uint32_t calculateIndexBits(uint32_t address, 
									 uint32_t addressSize, 
									 uint32_t indexBits, 
									 uint32_t tagBits)
{
	if(indexBits == 0)
	{
		return 0; 
	}
	uint32_t nonIndexSize = addressSize - indexBits; 
	return (address << (tagBits) ) >> nonIndexSize;
}

uint32_t calculateTagBits(uint32_t address, 
								  uint32_t addressSize, 
								  uint32_t indexBits, 
								  uint32_t tagBits)
{
	uint32_t nonTagSize = addressSize - tagBits; 
	return address >> nonTagSize; 
}

uint32_t blockToAddress(uint32_t index,
								uint32_t tag,  
								uint32_t addressSize, 
								uint32_t indexBits, 
								uint32_t tagBits)
{
	uint32_t indexShift = addressSize - indexBits - tagBits; 
	uint32_t tagShift = addressSize - tagBits;
	
	
	uint32_t indexComponent = index << indexShift ; 
	uint32_t tagComponent = tag << tagShift; 
	#if DEBUG_PRINTF == 1
	printf("index 0x%x index bits %u tag 0x%x tagbits %u\n", index, indexBits, tag, tagBits);
	printf("indexComponent 0x%x, tagComponent 0x%x\n", indexComponent, tagComponent); 
	#endif
	return indexComponent + tagComponent; 
}

bool setLoad(Cache* cache, 
								uint32_t address, 
								EvictedBlocks& evicted, 
								LoadBlocks& loadNeeded)
{
 return setLoadStore(cache, address, evicted, loadNeeded, false);  
}

bool setStore(Cache* cache, 
					  uint32_t address, 
					  EvictedBlocks& evicted, 
					  LoadBlocks& loadNeeded)
{
	return setLoadStore(cache, address, evicted, loadNeeded, true); 
}
				  
bool setLoadStore(Cache* cache, 
						uint32_t address, 
						EvictedBlocks& evicted, 
						LoadBlocks& loadNeeded, 
						bool dirtyBitValue)
{
	loadNeeded.address = 0; 
	loadNeeded.valid = false; 	
	evicted.address = 0; 
	evicted.valid = false; 
	
	SetAssociative * memory = cache->setAssociative; 
	
	uint32_t tag = calculateTagBits(address, 
											  ADDRESS_SPACE, 
											  memory->indexBits, 
											  memory->tagBits); 
	uint32_t index = calculateIndexBits(address, 
													ADDRESS_SPACE, 
													memory->indexBits, 
													memory->tagBits);  												
	#if DEBUG_PRINTF == 1
	printf("Address 0x%x, Index 0x%x (%d bits), Tag 0x%x (%d bits)\n"
			, address, index, memory->indexBits, tag,  memory->tagBits);
	#endif
	
	if(!(memory->lruTable[index].contains(tag)))
	{
		SetAssociativeEntity newEntity; 
		
		newEntity.valid = true; 
		newEntity.dirty = dirtyBitValue; 
		newEntity.tag = tag; 
		
		loadNeeded.address = address; 
		loadNeeded.valid = true; 
			
		#if DEBUG_PRINTF == 1 
		printf("Not Found, must load: 0x%x\n",loadNeeded.address); 
		#endif
		
		// If it is full, we need to evict
		if(memory->lruTable[index].length() >= memory->setSize)
		{
			#if DEBUG_PRINTF == 1 
			printf("Full Set Must Evict\n"); 
			#endif
			SetAssociativeEntity oldEntity = memory->lruTable[index].removeLast(); 
			if(oldEntity.valid && oldEntity.dirty)
			{
				
				//What we need to evict
				evicted.address = blockToAddress(index, 
														oldEntity.tag,
														ADDRESS_SPACE,  
														memory->indexBits, 
														memory->tagBits);
				evicted.valid = true;
				#if DEBUG_PRINTF == 1 
				printf("Evicting: 0x%x\n",evicted.address); 
				#endif
			}
		}
		memory->lruTable[index].insertFirst(tag, newEntity); 
	}
	else
	{
		SetAssociativeEntity * entity = memory->lruTable[index].load(tag); 

		
		entity->valid = true; 
		
		#if DEBUG_PRINTF == 1 
		printf("Found Moving to Beginning of Array\n"); 
		#endif
		
		memory->lruTable[index].moveToFirst(tag); 
		
		#if DEBUG_PRINTF == 1 
		printf("Done moving\n"); 
		#endif
		//If we are storing make it dirty
		if(dirtyBitValue)
		{
			#if DEBUG_PRINTF == 1 
			printf("Setting Dirty Bit\n"); 
			#endif
			
			entity->dirty = dirtyBitValue; 
		}
	}
	return true;  
}

bool setInvalidate(Cache* cache, 
						 uint32_t address)
{
	SetAssociative * memory = cache->setAssociative; 
	
	uint32_t tag = calculateTagBits(address, 
											  ADDRESS_SPACE, 
											  memory->indexBits, 
											  memory->tagBits); 
	uint32_t index = calculateIndexBits(address, 
													ADDRESS_SPACE, 
													memory->indexBits, 
													memory->tagBits);  
	if(memory->lruTable[index].contains(tag))
	{
		SetAssociativeEntity * entity = memory->lruTable[index].load(tag); 
		entity-> valid = false; 
	}
	return true; 
}						 


uint32_t calculateL2BlockTransferTime(const Configuration& config)
{
	return config.L2_trasnfer_time *
			 config.L1_block_size /
			 config.L2_bus_width;
}

uint32_t calculateMainMemoryTime(const Configuration& config,
									  uint32_t bytesToTransfer)
{
	if(bytesToTransfer == 0)
	{
		// Can't use any time if we don't have any bytes to transfer
		return 0; 
	}
	
	return config.mem_sendaddr +
			 config.mem_ready +
			 bytesToTransfer *
			 config.mem_chunktime /
			 config.mem_chunksize; 
}

uint32_t calculateActualSize(const char* inputSize) 
{
	uint32_t actualSize; 
	char type[10] = "";
	sscanf(inputSize, "%d%s", &actualSize, type); 
 
 	if(strlen(type) < 1 || type == NULL)
 	{
 		//return actual size DO NOTHING
 	}
	else if(0 == strncmp(type, "KB", strlen("KB")))
	{
		actualSize  *= 1024; 
	}
	else if(0 == strncmp(type, "MB", strlen("MB")))
	{
		actualSize  *= 1024 * 1024; 
	}
	return actualSize; 
}

CacheType convertCacheType(const char* cacheString)
{
	if(0 == strncmp(cacheString, "Direct Mapped", strlen(cacheString)))
	{
		return DirectMapped;
	}
	else if(0 == strncmp(cacheString, "Two Way Set Associative", strlen(cacheString)))
	{ 
		return TwoWaySetAssociative;
	}
	else if(0 == strncmp(cacheString, "Four Way Set Associative", strlen(cacheString)))
	{
		return FourWaySetAssociative;
	}
	else if(0 == strncmp(cacheString, "Fully Associative", strlen(cacheString)))
	{
		return FullyAssociative;
	}
	else 
	{
		printf("Unknown Type %s\n", cacheString); 
		exit(1); 
	}
	return DirectMapped; 
}

bool loadConfig(const char* filename, Configuration & config)
{
	printf("filename = %s\n", filename); 
	FILE* inputFile = fopen(filename, "r"); 
	if(inputFile == NULL) 
	{
		printf("Can't open config file: %s\n", filename); 
		return false; 
	}

	char name[32];
	char value[32]; 
	while(fscanf(inputFile, "%s = %s",name, value) != EOF)
	{
		if(0 == strncmp(name, "L1_Icache_type", strlen("L1_Icache_type")))
		{
			config.L1_Icache_type = convertCacheType(value);
			#if DEBUG_PRINTF == 1
			printf("L1_Icache_type = %d\n", config.L1_Icache_type); 
			#endif
		}
		else if(0 == strncmp(name, "L1_Icache_size", strlen("L1_Icache_size")))
		{
			config.L1_Icache_size = calculateActualSize(value);
			#if DEBUG_PRINTF == 1
			printf("L1_Icache_size = %d\n", config.L1_Icache_size); 
			#endif
		}
		else if(0 == strncmp(name, "L1_Dcache_type", strlen("L1_Dcache_type")))
		{
			config.L1_Dcache_type = convertCacheType(value);
			#if DEBUG_PRINTF == 1
			printf("L1_Dcache_type = %d\n", config.L1_Dcache_type); 
			#endif
		}
		else if(0 == strncmp(name, "L1_Dcache_size", strlen("L1_Dcache_size")))
		{
			config.L1_Dcache_size = calculateActualSize(value); 
			#if DEBUG_PRINTF == 1
			printf("L1_Dcache_size = %d\n", config.L1_Dcache_size); 
			#endif
		}
		else if(0 == strncmp(name, "L2_Ucache_type", strlen("L2_Ucache_type")))
		{
			config.L2_Ucache_type = convertCacheType(value);
			#if DEBUG_PRINTF == 1
			printf("L2_Ucache_type = %d\n", config.L2_Ucache_type); 
			#endif
		}
		else if(0 == strncmp(name, "L2_Ucache_size", strlen("L2_Ucache_size")))
		{
			config.L2_Ucache_size = calculateActualSize(value); 
			#if DEBUG_PRINTF == 1
			printf("L2_Ucache_size = %d\n", config.L2_Ucache_size); 
			#endif
		}
		else if(0 == strncmp(name, "L1_block_size", strlen("L1_block_size")))
		{
			config.L1_block_size = calculateActualSize(value); 
			#if DEBUG_PRINTF == 1
			printf("L1_block_size = %d\n", config.L1_block_size); 
			#endif
		}
		else if(0 == strncmp(name, "L1_assoc", strlen("L1_assoc")))
		{
			config.L1_assoc = calculateActualSize(value); 
			#if DEBUG_PRINTF == 1
			printf("L1_assoc = %d\n", config.L1_assoc); 
			#endif
		}
		else if(0 == strncmp(name, "L1_hit_time", strlen("L1_hit_time")))
		{
			config.L1_hit_time = calculateActualSize(value); 
			#if DEBUG_PRINTF == 1
			printf("L1_hit_time = %d\n", config.L1_hit_time); 
			#endif
		}
		else if(0 == strncmp(name, "L1_miss_time", strlen("L1_miss_time")))
		{
			config.L1_miss_time = calculateActualSize(value);
			#if DEBUG_PRINTF == 1
			printf("L1_miss_time = %d\n", config.L1_miss_time); 
			#endif 
		}
		else if(0 == strncmp(name, "L2_block_size", strlen("L2_block_size")))
		{
			config.L2_block_size = calculateActualSize(value); 
			#if DEBUG_PRINTF == 1
			printf("L2_block_size = %d\n", config.L2_block_size); 
			#endif
		}
		else if(0 == strncmp(name, "L2_assoc", strlen("L2_assoc")))
		{
			config.L2_assoc = calculateActualSize(value); 
			#if DEBUG_PRINTF == 1
			printf("L2_assoc = %d\n", config.L2_assoc); 
			#endif
		}
		else if(0 == strncmp(name, "L2_hit_time", strlen("L2_hit_time")))
		{
			config.L2_hit_time = calculateActualSize(value); 
			#if DEBUG_PRINTF == 1
			printf("L2_hit_time = %d\n", config.L2_hit_time); 
			#endif
		}
		else if(0 == strncmp(name, "L2_miss_time", strlen("L2_miss_time")))
		{
			config.L2_miss_time = calculateActualSize(value); 
			#if DEBUG_PRINTF == 1
			printf("L2_miss_time = %d\n", config.L2_miss_time); 
			#endif
		}
		else if(0 == strncmp(name, "L2_trasnfer_time", strlen("L2_trasnfer_time")))
		{
			config.L2_trasnfer_time = calculateActualSize(value); 
			#if DEBUG_PRINTF == 1
			printf("L2_trasnfer_time = %d\n", config.L2_trasnfer_time); 
			#endif
		}
		else if(0 == strncmp(name, "L2_bus_width", strlen("L2_bus_width")))
		{
			config.L2_bus_width = calculateActualSize(value); 
			#if DEBUG_PRINTF == 1
			printf("L2_bus_width = %d\n", config.L2_bus_width); 
			#endif
		}	
		else if(0 == strncmp(name, "mem_sendaddr", strlen("mem_sendaddr")))
		{
			config.mem_sendaddr = calculateActualSize(value); 
			#if DEBUG_PRINTF == 1
			printf("mem_sendaddr = %d\n", config.mem_sendaddr); 
			#endif
		}
		else if(0 == strncmp(name, "mem_ready", strlen("mem_ready")))
		{
			config.mem_ready = calculateActualSize(value); 
			#if DEBUG_PRINTF == 1
			printf("mem_ready = %d\n", config.mem_ready); 
			#endif
		}
		else if(0 == strncmp(name, "mem_chunktime", strlen("mem_chunktime")))
		{
			config.mem_chunktime = calculateActualSize(value);
			#if DEBUG_PRINTF == 1
			printf("mem_chunktime = %d\n", config.mem_chunktime);  
			#endif
		}
		else if(0 == strncmp(name, "mem_chunksize", strlen("mem_chunksize")))
		{
			config.mem_chunksize = calculateActualSize(value);
			#if DEBUG_PRINTF == 1
			printf("mem_chunksize = %d\n", config.mem_chunksize); 
			#endif
		}	
	} 
	
	config.L2_block_transfer_time = calculateL2BlockTransferTime(config);
	#if DEBUG_PRINTF == 1
	printf("block_transfer_time %d\n", config.L2_block_transfer_time);
	#endif
	return true; 
}

uint32_t calculateMemoryCost(const Configuration & config, const Cache L1I, const Cache L1D, const Cache L2)
{
  uint32_t L1ICost = (config.L1_Icache_size)/calculateActualSize("4KB")*100 *
                      (log(L1I.setAssociative->setSize)/log(2) + 1);
  printf("L1ICost %"PRIu32"\n", L1ICost);
  uint32_t L1DCost = (config.L1_Dcache_size)/calculateActualSize("4KB")*100 *
                      (log(L1D.setAssociative->setSize)/log(2) + 1);
  printf("L1DCost %"PRIu32"\n", L1DCost);
  uint32_t L2Cost = (config.L2_Ucache_size)/calculateActualSize("64KB")*50 *
                      (log(L2.setAssociative->setSize)/log(2) + 1);
  printf("L2Cost %"PRIu32"\n", L2Cost);
  uint32_t MainMemoryLatencyCost = log(50.0/config.mem_ready)/log(2.0)*200 + 50; //Increases by 100 every time it goes down by 2
  printf("MainMemoryLatencyCost %"PRIu32"\n", MainMemoryLatencyCost);
  uint32_t BandwithCost = log(config.mem_chunksize/16)/log(2)*100 + 25;
  printf("BandwithCost %"PRIu32"\n", BandwithCost);
  uint32_t totalCost = L1ICost + L1DCost + L2Cost + MainMemoryLatencyCost + BandwithCost;
  printf("totalCost %"PRIu32"\n", totalCost);
  
  return totalCost; 
}
