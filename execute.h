/* *************************************************
 * Execution Module Interface
 * by Thomas Li & Spencer Schoeben
 * November 21, 2014
 * COMP 40 HW 6
 *
 * This module handles execution of instructions from the UM.
 */

#ifndef EXECUTE
#define EXECUTE

#include "virtualization.h"

/* gets the program_counter and executes the instruction at the
 * location of the program_counter
 */
bool execute(UM machine);

#endif