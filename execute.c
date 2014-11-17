#include "execute.h"
#include "memory.h"
#include "bitpack.h"
#include "io.h"
#include "um-dis.h"

typedef enum { MOVE, SEGLOAD, SEGSTORE, ADD, 
        MULT, DIV, NAND,  HALT, MAP, UNMAP, 
        OUTPUT, INPUT, JUMP, LOADVAL } OPCODE;

typedef enum { R0, R1, R2, R3, R4,
        R5, R6, R7 } REGISTER;

typedef struct {
        OPCODE opcode;
        REGISTER rA;
        REGISTER rB;
        REGISTER rC;
        Word rAv;
        Word rBv;
        Word rCv;
        Word value; 
} UM_Instruction;

typedef void (*funPtr)(UM, UM_Instruction);

static UM_Instruction unpack(UM machine, Word packed_instruction);

static funPtr* getFunctions();

static void exe_move(UM machine, UM_Instruction i);
static void exe_segload(UM machine, UM_Instruction i);
static void exe_segstore(UM machine, UM_Instruction i);
static void exe_add(UM machine, UM_Instruction i);
static void exe_multiply(UM machine, UM_Instruction i);
static void exe_divide(UM machine, UM_Instruction i);
static void exe_nand(UM machine, UM_Instruction i);
static void exe_halt(UM machine, UM_Instruction i);
static void exe_map(UM machine, UM_Instruction i);
static void exe_unmap(UM machine, UM_Instruction i);
static void exe_output(UM machine, UM_Instruction i);
static void exe_input(UM machine, UM_Instruction i);
static void exe_jump(UM machine, UM_Instruction i);
static void exe_loadval(UM machine, UM_Instruction i);

bool execute(UM machine) {
        Word packed = get_word(machine, 0, machine->pc);
        UM_Instruction i = unpack(machine, packed);
        funPtr* functions = getFunctions();
        if (i.opcode > 13) return false;
        (*(functions[i.opcode]))(machine, i);
        free(functions);
        return true;
}

static UM_Instruction unpack(UM machine, Word packed) {
        UM_Instruction i;

        i.opcode = Bitpack_getu(packed, 4, 28);

        if (i.opcode < 13) {
                i.rA = Bitpack_getu(packed, 3, 6);
                i.rAv = register_load(machine, i.rA);
                i.rB = Bitpack_getu(packed, 3, 3);
                i.rBv = register_load(machine, i.rB);
                i.rC = Bitpack_getu(packed, 3, 0);
                i.rCv = register_load(machine, i.rC);
        } else {
                i.rA = Bitpack_getu(packed, 3, 25);
                i.rAv = register_load(machine, i.rA);
                i.value = Bitpack_getu(packed, 25, 0);
        }

        return i;
}

static funPtr* getFunctions() {
        funPtr* array = malloc(sizeof(funPtr)*14);
        array[0] = &exe_move;
        array[1] = &exe_segload;
        array[2] = &exe_segstore;
        array[3] = &exe_add;
        array[4] = &exe_multiply;
        array[5] = &exe_divide;
        array[6] = &exe_nand;
        array[7] = &exe_halt;
        array[8] = &exe_map;
        array[9] = &exe_unmap;
        array[10] = &exe_output;
        array[11] = &exe_input;
        array[12] = &exe_jump;
        array[13] = &exe_loadval;
        return array;     
}

static void exe_move(UM machine, UM_Instruction i) {
        if (i.rCv != 0) {
                register_store(machine, i.rA, i.rBv);
        }
}
static void exe_segload(UM machine, UM_Instruction i) {
        register_store(machine, i.rA, get_word(machine, i.rBv, i.rCv));
}
static void exe_segstore(UM machine, UM_Instruction i) {
        put_word(machine, i.rAv, i.rBv, i.rCv);
}
static void exe_add(UM machine, UM_Instruction i) {
        register_store(machine, i.rA, (i.rBv+i.rCv));
}
static void exe_multiply(UM machine, UM_Instruction i) {
        register_store(machine, i.rA, (i.rBv*i.rCv));
}
static void exe_divide(UM machine, UM_Instruction i) {
        register_store(machine, i.rA, (i.rBv/i.rCv));
}
static void exe_nand(UM machine, UM_Instruction i) {
        register_store(machine, i.rA, ~(i.rBv & i.rCv));
}
static void exe_halt(UM machine, UM_Instruction i) {
        (void)i;
        machine->pc = machine->program_length;
}
static void exe_map(UM machine, UM_Instruction i) {
        SegmentID seg = map(machine, i.rCv);
        register_store(machine, i.rB, seg);
}
static void exe_unmap(UM machine, UM_Instruction i) {
        unmap(machine, i.rCv);
}
static void exe_output(UM machine, UM_Instruction i) {
        (void)machine;
        write(i.rCv);
}
static void exe_input(UM machine, UM_Instruction i) {
        register_store(machine, i.rC, read());
}
static void exe_jump(UM machine, UM_Instruction i) {
        machine->pc = i.rCv - 1;
        if (i.rBv) {
                duplicate(machine, i.rBv, 0);
        }
}
static void exe_loadval(UM machine, UM_Instruction i) {
        register_store(machine, i.rA, i.value);
}
