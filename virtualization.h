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
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "mem.h"
#include "assert.h"

typedef uint32_t Word;
typedef Word SegmentID;
#define UM_NULL 0xFFFF
#endif