/* *************************************************
 * I/O Module Interface
 * by Thomas Li & Spencer Schoeben
 * November 21, 2014
 * COMP 40 HW 6
 *
 * This module handles I/O of ASCII characters.
 */

#ifndef IO
#define IO

#include "virtualization.h"

/* reads character from stdin, returns as Word.
 * if EOF, 0xFFFF is returned. */
Word read();

/* writes Word to stdout and returns that Word.
 * if unable to write, 0xFFFF is returned. */
Word write(Word to_write);

#endif