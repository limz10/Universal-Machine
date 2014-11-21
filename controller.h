/* *************************************************
 * Controller Module Interface
 * by Thomas Li & Spencer Schoeben
 * November 21, 2014
 * COMP 40 HW 6
 *
 * This module allows the UM to be controlled and interfaced with in a
 * very abstract way.
 */

#ifndef CONTROLLER
#define CONTROLLER

#include "virtualization.h"

/* creates a new UM and returns it. */
UM UM_new();

/* runs the program specified on the UM specified.
 * if nothing bad happens, returns true.
 */
bool UM_run(UM machine, FILE* program);

/* kills the machine, freeing its data */
bool UM_kill(UM* machine);

#endif