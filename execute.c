#include "execute.h"
#include "memory.h"
#include "bitpack.h"
#include "io.h"

typedef struct {
        Word opcode;
        Word rA;
        Word rB;
        Word rC;
        Word rAv;
        Word rBv;
        Word rCv;
        Word value; 
} UM_Instruction;

typedef void (*funPtr)(UM, UM_Instruction);

static inline UM_Instruction unpack(UM machine, Word packed_instruction);
static inline Word unpack_bits(Word bitword, unsigned width, unsigned lsb);

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

static funPtr functions[14] = { 
        &exe_move,
        &exe_segload,
        &exe_segstore,
        &exe_add,
        &exe_multiply,
        &exe_divide,
        &exe_nand,
        &exe_halt,
        &exe_map,
        &exe_unmap,
        &exe_output,
        &exe_input,
        &exe_jump,
        &exe_loadval
};

bool execute(UM machine) {
        Word packed = get_word(machine, 0, machine->pc);
        UM_Instruction i = unpack(machine, packed);
        if (i.opcode > 13) return false;
        (*(functions[i.opcode]))(machine, i);
        return true;
}

static inline UM_Instruction unpack(UM machine, Word packed) {
        UM_Instruction i;

        i.opcode = unpack_bits(packed, 4, 28);

        if (i.opcode < 13) {
                i.rA = unpack_bits(packed, 3, 6);
                i.rAv = register_load(machine, i.rA);
                i.rB = unpack_bits(packed, 3, 3);
                i.rBv = register_load(machine, i.rB);
                i.rC = unpack_bits(packed, 3, 0);
                i.rCv = register_load(machine, i.rC);
        } else {
                i.rA = unpack_bits(packed, 3, 25);
                i.rAv = register_load(machine, i.rA);
                i.value = unpack_bits(packed, 25, 0);
        }

        return i;
}

static inline Word unpack_bits(Word bitword, unsigned width, unsigned lsb) {
        return bitword << (32 - (width + lsb)) >> (32 - width);
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
