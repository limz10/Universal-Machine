#ifndef TESTS
#define TESTS

#include "virtualization.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "assert.h"
#include "fmt.h"
#include "seq.h"
#include "bitpack.h"

typedef Word Um_instruction;
typedef enum { MOVE, SEGLOAD, SEGSTORE, ADD,
        MULTIPLY, DIVIDE, NAND, HALT, MAP, UNMAP,
        OUTPUT, INPUT, JUMP, LOADVAL  } Um_opcode;
typedef enum { r0 = 0, r1, r2, r3, r4, r5, r6, r7 } Um_reg;

Um_instruction get_inst(Seq_T stream, int i);
void put_inst(Seq_T stream, int i, Um_instruction inst);


#endif