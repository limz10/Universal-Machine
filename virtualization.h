/* *************************************************
 * Virtualization Module Interface
 * by Thomas Li & Spencer Schoeben
 * November 21, 2014
 * COMP 40 HW 6
 *
 * This module maintains shared truths about the datatypes used
 * in the UM. It also allows loading and storing to registers.
 */

#ifndef VIRTUALIZATION
#define VIRTUALIZATION

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "mem.h"
#include "assert.h"


typedef uint32_t Word;
typedef Word SegmentID;
typedef Word* RegisterBank;
typedef struct SegmentBlock_T * SegmentBlock;
typedef struct UM_T {
	RegisterBank registers;
	SegmentBlock memory;
	SegmentID pc;
        unsigned program_length;
} * UM;

#define UM_NULL ((Word)-1)

/* stores value in the specified register in the machine.
 * returns previous value of the register */
Word register_store(UM machine, unsigned id, Word value);

/* loads value in the specified register in the machine
 * and returns it */
Word register_load(UM machine, unsigned id);

#endif