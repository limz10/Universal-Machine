/* *************************************************
 * Program Loading Module Interface
 * by Thomas Li & Spencer Schoeben
 * November 21, 2014
 * COMP 40 HW 6
 *
 * This module loads a program from file into the machine's memory.
 */

#ifndef P_LOAD
#define P_LOAD

#include "virtualization.h"

/* loads the UM program from file into the memory of the UM.
 * returns true if successful! */
bool load(UM machine, FILE* program);

#endif