#ifndef VIRTUALIZATION
#define VIRTUALIZATION

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

typedef uint32_t Word;
typedef Word SegmentID;
typedef Word* RegisterBank;
typedef Word* Segment;
typedef struct SegmentBlock_T* SegmentBlock;
typedef struct UM_T {
	RegisterBank registers;
	SegmentBlock memory;
	SegmentID pc;
} * UM;

Word register_store(UM machine, int id, Word value);
Word register_load(UM machine, int id);


#endif