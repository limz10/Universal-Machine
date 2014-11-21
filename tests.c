#include "tests.h"

/* EXPOSED HELPERS */

Um_instruction get_inst(Seq_T stream, int i) {
	assert(sizeof(Um_instruction) <= sizeof(uintptr_t));
	return (Um_instruction) (uintptr_t) (Seq_get(stream, i));
}

void put_inst(Seq_T stream, int i, Um_instruction inst) {
	assert(sizeof(inst) <= sizeof(uintptr_t));
	Seq_put(stream, i, (void *)(uintptr_t) inst);
}

/* STATIC HELPERS */

static Um_instruction three_register(Um_opcode op, Um_reg ra, 
	Um_reg rb, Um_reg rc) {
	Um_instruction inst = 0;

        inst = Bitpack_newu(inst, 4, 28, (unsigned)op);
        inst = Bitpack_newu(inst, 3, 6, (unsigned)ra);
        inst = Bitpack_newu(inst, 3, 3, (unsigned)rb);
        inst = Bitpack_newu(inst, 3, 0, (unsigned)rc);

        return inst;
}

static inline Um_instruction loadval(Um_reg ra, Word val) {
	Um_instruction inst = 0;

        inst = Bitpack_newu(inst, 4, 28, (unsigned)LOADVAL);
        inst = Bitpack_newu(inst, 3, 25, (unsigned)ra);
        inst = Bitpack_newu(inst, 25, 0, (unsigned)val);
 
	return inst;
}

static inline void emit(Seq_T stream, Um_instruction inst)
{
	assert(sizeof(inst) <= sizeof(uintptr_t));
	Seq_addhi(stream, (void *)(uintptr_t) inst);
}


static void add_label(Seq_T stream, int location_to_patch, int label_value)
{
	Um_instruction inst = get_inst(stream, location_to_patch);
	unsigned k = Bitpack_getu(inst, 25, 0);
	inst = Bitpack_newu(inst, 25, 0, label_value + k);
	put_inst(stream, location_to_patch, inst);
}

/* INSTRUCTIONS */

static inline Um_instruction move(Um_reg ra, Um_reg rb, Um_reg rc) {
	return three_register(MOVE, ra, rb, rc);
}
static inline Um_instruction segload(Um_reg ra, Um_reg rb, Um_reg rc) {
	return three_register(SEGLOAD, ra, rb, rc);
}
static inline Um_instruction segstore(Um_reg ra, Um_reg rb, Um_reg rc) {
	return three_register(SEGSTORE, ra, rb, rc);
}
static inline Um_instruction add(Um_reg ra, Um_reg rb, Um_reg rc) {
	return three_register(ADD, ra, rb, rc);
}
static inline Um_instruction multiply(Um_reg ra, Um_reg rb, Um_reg rc) {
	return three_register(MULTIPLY, ra, rb, rc);
}
static inline Um_instruction divide(Um_reg ra, Um_reg rb, Um_reg rc) {
	return three_register(DIVIDE, ra, rb, rc);
}
static inline Um_instruction nand(Um_reg ra, Um_reg rb, Um_reg rc) {
	return three_register(NAND, ra, rb, rc);
}
static inline Um_instruction halt(void) {
	return three_register(HALT, 0, 0, 0);
}
static inline Um_instruction map(Um_reg rb, Um_reg rc) {
	return three_register(MAP, 0, rb, rc);
}
static inline Um_instruction unmap(Um_reg rc) {
	return three_register(UNMAP, 0, 0, rc);
}
static inline Um_instruction output(Um_reg rc) {
	return three_register(OUTPUT, 0, 0, rc);
}
static inline Um_instruction input(Um_reg rc) {
	return three_register(INPUT, 0, 0, rc);
}
static inline Um_instruction jump(Um_reg rb, Um_reg rc) {
	return three_register(JUMP, 0, rb, rc);
}
// loadval function defined above

static void emit_out_string(Seq_T stream, const char *s, Um_reg aux_reg) {
	unsigned str_length = strlen(s);
	for(unsigned i = 0; i < str_length; i++) {
		emit(stream, loadval(aux_reg, s[i]));
		emit(stream, output(aux_reg));
	}
}

// // emit virtual instructions
// static void emit_subtract(Um_reg ra, Um_reg rb, Um_reg rc) {
// 	emit(stream, nand(rc, rc, rc));
// 	emit(stream, add(rb, rb, rc));
// 	emit(stream, loadval(ra, 1));
// 	emit(stream, add(ra, rb, ra));
// }

/* EMIT TESTS */

void emit_halt_test(Seq_T stream) {
	emit(stream, halt());
	emit(stream, loadval(r1, 'B'));
	emit(stream, output(r1));
	emit(stream, loadval(r1, 'a'));
	emit(stream, output(r1));
	emit(stream, loadval(r1, 'd'));
	emit(stream, output(r1));
	emit(stream, loadval(r1, '!'));
	emit(stream, output(r1));
	emit(stream, loadval(r1, '\n'));
	emit(stream, output(r1));
}

void emit_goto_test(Seq_T stream) {
	int patch_L = Seq_length(stream);
	emit(stream, loadval(r7, 0));	     /* will be patched to 'r7 := L' */
	emit(stream, jump(r0, r7));   /* should goto label L          */
	emit_out_string(stream, "GOTO failed.\n", r1);
	emit(stream, halt());
	/* define 'L' to be here */
	add_label(stream, patch_L, Seq_length(stream));	
	emit_out_string(stream, "GOTO passed.\n", r1);
	emit(stream, halt());
}

void emit_arith_test(Seq_T stream) {
	// expects as input: 5, 2, 3
	// (r1 + r2 - r2) * r3 / r3 = r1
	// ( 5 + 2 - 2 ) * 3 / 3 = 5
	emit(stream, loadval(r1, 5));
	emit(stream, loadval(r2, 2));
	emit(stream, loadval(r3, 3));
	// r0 = r1 + r2
	emit(stream, add(r0, r1, r2));
	// r0 = r0 - r2
	// r4 = -r2
	// r0 = r0 + r4
	emit(stream, nand(r4, r2, r2));
	emit(stream, loadval(r7, 1));
	emit(stream, add(r2, r4, r7));
	emit(stream, add(r7, r0, r2));
	// r7 = r0 * r3
	emit(stream, multiply(r4, r7, r3));
	// r0 = r7 / r3
	emit(stream, divide(r0, r4, r3));
	// r1 = 48 (offset to turn a value into its digit)
	emit(stream, loadval(r1, 48));
	// get r3 ready to print the answer
	emit(stream, add(r3, r0, r1));
	emit(stream, output(r3));
}

void emit_map_memory_test(Seq_T stream) {
	emit(stream, loadval(r1, 10000));
	emit(stream, loadval(r7, 100));
	emit(stream, loadval(r6, 1));
	int mapJump = Seq_length(stream);
	emit(stream, map(r0, r1));
	emit_out_string(stream, "Mapped segment.\n", r2);
	emit(stream, nand(r4, r5, r5));
	emit(stream, add(r4, r7, r4));
	int patch_End = Seq_length(stream);
	emit(stream, loadval(r0, 0));
	emit(stream, loadval(r2, mapJump));
	emit(stream, add(r5, r5, r6)); // r5++
	emit(stream, move(r0, r2, r4));
	emit(stream, jump(r3, r0));
	add_label(stream, patch_End, Seq_length(stream));
	emit(stream, halt());
}