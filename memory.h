/* *************************************************
 * Memory Managemnt Module Interface
 * by Thomas Li & Spencer Schoeben
 * November 7, 2014
 * COMP 40 HW 6
 *
 * This module handles memory managemnt for our Universal Machine.
 * The interface allows an instance of memory to be initialized 
 * for a given machine and allows mapping and unmapping of segments.
 *
 * Access to data within segments is done through functions provided
 * by the interface in order to maintain security.
 */

#ifndef MEMORY
#define MEMORY

#include "virtualization.h"

/* sets up the memory in the machine by initializing all necessary values.
 * this must be called before segments can be mapped.
 * if this function is called more than once on a machine, subsequent
 * calls to the function will be ignored as need only be initialized once.
 */
SegmentBlock SegmentBlock_new();

/* unitializes the memory module. */
bool SegmentBlock_free(SegmentBlock* mem);

/* maps a segment in the UM of size number of words and returns a 
 * unique segment id for referering to the segment in future calls */
SegmentID map(UM machine, unsigned size);

/* unmaps a segment in the UM by a given segment id */
void unmap(UM machine, SegmentID id);

/* duplicates a segment into another segment */
void duplicate(UM machine, SegmentID from, SegmentID to);

/* puts a word specified by a value into a segment in the UM specified
 * by a segment id and offset. returns the word that was previously
 * in this space */
Word put_word(UM machine, SegmentID id, unsigned offset, Word value);

/* returns the word from a segment in the UM specified by a segment id
 * and offset */
Word get_word(UM machine, SegmentID id, unsigned offset);

#endif