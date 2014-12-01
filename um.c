#include "virtualization.h"

/* UM CYCLE */

static inline void run();

/* PROGRAM */
static Word program_counter = 0;
static Word program_length = 0;

static inline void load(FILE* program);

/* EXECUTION */

static inline void execute();

/* MEMORY */
static inline void mem_init();
static inline void mem_free();
static inline SegmentID map(unsigned size);
static inline void unmap(SegmentID id);
static inline void duplicate(SegmentID from);
static inline Word put_word(SegmentID id, unsigned offset, Word value);
static inline Word get_word(SegmentID id, unsigned offset);

struct {
        struct {
                unsigned head;
                unsigned length;
                struct {
                        size_t size;
                        unsigned length;
                        Word* values;
                } array;
        } unmapped_ids;
        SegmentID next_seg;
        struct {
                size_t size;
                unsigned length;
                char* values;
        } seg_map;
} memory;

/* REGISTERS */
static Word registers[8] = { 0 };
static inline void register_store(unsigned id, Word value);
static inline Word register_load(unsigned id);

/* IO */
static inline Word write(Word to_write);
static inline Word read();

/* QUEUE */

#define SCALEFACTOR 2

static inline void Queue_new(unsigned hint, size_t size);
static inline Word Queue_pop();
static inline void Queue_push(Word value);
static inline void Queue_free();
static inline void Queue_resize();

/* ARRAY */

static inline void Array_new(unsigned hint, size_t size);
static inline void* Array_at(unsigned pos);
static inline void Array_resize();
static inline void Array_free();

int main(int argc, char *argv[]) {
        FILE* program;
        if (argc == 2) {
                program = fopen(argv[1], "r");
        } else {
                program = NULL;
        }
        if (!program) {
                fprintf(stderr, "usage: ./um <file in um binary format>\n");
                exit(1);
        }

        mem_init();
        load(program);

        run();

        fclose(program);
        mem_free();
}

/* UM CYCLE */

static inline void run() {
        while(program_counter < program_length) {
                execute();
                program_counter++;
        }
}

/* PROGRAM LOAD */

static inline Word readWord(FILE* program) {
        Word to_return = 0;
        for(unsigned i = 0; i < sizeof(Word); i++) {
                ((unsigned char *)&to_return)[sizeof(Word)-i-1] = getc(program);
        }
        return to_return;
}

static inline unsigned wordsInFile(FILE* program) {
        fseek(program, 0, SEEK_END);
        unsigned fileLength = ftell(program);
        fseek(program, 0, SEEK_SET);
        return (fileLength / sizeof(Word));
}

static inline void load(FILE* program) {
        program_length = wordsInFile(program);
        map(program_length);

        for(unsigned i = 0; i < program_length; i++) {
                put_word(0, i, readWord(program));
        }        
}


/* MEMORY */

static const int SEQ_SIZE = 1000;
static const int SEG_MAP_SIZE = 1000;

static unsigned seq_length;
static unsigned seg_map_length;

static inline SegmentID new_seg_id();
static inline Word* allocate_segment(unsigned length);
static inline void* get_segment(int seg_id);

/* === IMPLEMENTATION === */

static inline void mem_init() {
        Queue_new(SEQ_SIZE, sizeof(Word));
        seq_length = 0;
        Array_new(SEG_MAP_SIZE, sizeof(uintptr_t));
        seg_map_length = SEG_MAP_SIZE;
        memory.next_seg = 0;
}

static inline void mem_free() {
        unmap(0);
        Queue_free();
        Array_free();
        seq_length = 0;
        seg_map_length = 0;
}

static inline SegmentID map(unsigned length) {
        SegmentID seg_id = new_seg_id();
        if (seg_id == memory.seg_map.length) {
                Array_resize();
        }
        Word** seg = get_segment(seg_id);
        if(!((*seg) = allocate_segment(length))) {
                fprintf(stderr, "Machine is out of memory!\n");
                exit(1);
        }
        (*seg)[0] = length;
        return seg_id;
}

static inline void unmap(SegmentID id) {
        Word** seg = get_segment(id);
        free(*seg);
        *seg = NULL;
        if (id) {
                Queue_push(id);
                seq_length++;
        } 
}

static inline void duplicate(SegmentID from) {
        Word** seg1 = get_segment(from);
        unsigned seg1_length = (*seg1)[0];
        Word** seg2 = get_segment(0);
        free(*seg2);
        *seg2 = allocate_segment(seg1_length);
        for(unsigned i = 0; i <= seg1_length; i++) {
                (*seg2)[i] = (*seg1)[i];
        }
        program_length = (*seg2)[0];
}

static inline Word put_word(SegmentID id, unsigned offset, Word value) {
        Word** seg = get_segment(id);
        Word to_return = (*seg)[offset+1];
        (*seg)[offset+1] = value;
        return to_return;
}

static inline Word get_word(SegmentID id, unsigned offset) {
        Word** seg = get_segment(id);
        return (*seg)[offset+1];
}

static inline SegmentID new_seg_id() {
        SegmentID seg_id;
        if(seq_length) {
                seg_id = Queue_pop();
                seq_length--;
        } else {
                seg_id = memory.next_seg;
                memory.next_seg++;
        }
        return seg_id;
}

static inline Word* allocate_segment(unsigned length) {
        return calloc(sizeof(Word)*(length+1), sizeof(Word));
}

static inline void* get_segment(int seg_id) {
        return memory.seg_map.values + seg_id * memory.seg_map.size;
}

/* REGISTERS */

static inline void register_store(unsigned id, Word value) {
        if (id > 7) {
                fprintf(stderr, "Register %d does not exist."
                        " Must be in [0, 7]!\n", id);
                exit(1);
        }
        registers[id] = value;
}

static inline Word register_load(unsigned id) {
        if (id > 7) {
                fprintf(stderr, "Register %d does not exist. Must be in"
                " [0, 7]!\n", id);
                exit(1);
        }
        return registers[id];
}

/* IO */

static inline Word read() {
        Word to_return = getchar();

        if(to_return != (Word)EOF && (to_return > 255)) {
                fprintf(stderr, "You cannot read value %u!\n",
                (unsigned)to_return);
                exit(1);
        }

        return to_return;
}

static inline Word write(Word to_write) {
        if(to_write > 255) {
                fprintf(stderr, "You cannot write value %u!\n",
                (unsigned)to_write);
                exit(1);
        }

        Word to_return = putchar((char)to_write);

        return to_return;
}

/* EXECUTION */

typedef struct {
        Word opcode;
        Word rA;
        Word rB;
        Word rC;
        Word value; 
} UM_Instruction;

static inline UM_Instruction unpack(Word packed_instruction);
static inline Word unpack_bits(Word bitword, unsigned width, unsigned lsb);

static inline void exe_move(UM_Instruction i);
static inline void exe_segload(UM_Instruction i);
static inline void exe_segstore(UM_Instruction i);
static inline void exe_add(UM_Instruction i);
static inline void exe_multiply(UM_Instruction i);
static inline void exe_divide(UM_Instruction i);
static inline void exe_nand(UM_Instruction i);
static inline void exe_halt(UM_Instruction i);
static inline void exe_map(UM_Instruction i);
static inline void exe_unmap(UM_Instruction i);
static inline void exe_output(UM_Instruction i);
static inline void exe_input(UM_Instruction i);
static inline void exe_jump(UM_Instruction i);
static inline void exe_loadval(UM_Instruction i);

static inline void execute() {
        Word packed = get_word(0, program_counter);
        UM_Instruction i = unpack(packed);
        switch(i.opcode) {
                case 0:
                        exe_move(i);
                        break;
                case 1:
                        exe_segload(i);
                        break;
                case 2:
                        exe_segstore(i);
                        break;
                case 3:
                        exe_add(i);
                        break;
                case 4:
                        exe_multiply(i);
                        break;
                case 5:
                        exe_divide(i);
                        break;
                case 6:
                        exe_nand(i);
                        break;
                case 7:
                        exe_halt(i);
                        break;
                case 8:
                        exe_map(i);
                        break;
                case 9:
                        exe_unmap(i);
                        break;
                case 10:
                        exe_output(i);
                        break;
                case 11:
                        exe_input(i);
                        break;
                case 12:
                        exe_jump(i);
                        break;
                case 13:
                        exe_loadval(i);
                        break;
        }
}

static inline UM_Instruction unpack(Word packed) {
        UM_Instruction i;

        i.opcode = unpack_bits(packed, 4, 28);

        if (i.opcode < 13) {
                i.rA = unpack_bits(packed, 3, 6);
                i.rB = unpack_bits(packed, 3, 3);
                i.rC = unpack_bits(packed, 3, 0);
        } else {
                i.rA = unpack_bits(packed, 3, 25);
                i.value = unpack_bits(packed, 25, 0);
        }

        return i;
}

static inline Word unpack_bits(Word bitword, unsigned width, unsigned lsb) {
        return bitword << (32 - (width + lsb)) >> (32 - width);
}

static inline void exe_move(UM_Instruction i) {
        Word rBv = register_load(i.rB);
        Word rCv = register_load(i.rC);
        if (rCv != 0) {
                register_store(i.rA, rBv);
        }
}
static inline void exe_segload(UM_Instruction i) {
        Word rBv = register_load(i.rB);
        Word rCv = register_load(i.rC);
        register_store(i.rA, get_word(rBv, rCv));
}
static inline void exe_segstore(UM_Instruction i) {
        Word rAv = register_load(i.rA);
        Word rBv = register_load(i.rB);
        Word rCv = register_load(i.rC);
        put_word(rAv, rBv, rCv);
}
static inline void exe_add(UM_Instruction i) {
        Word rBv = register_load(i.rB);
        Word rCv = register_load(i.rC);
        register_store(i.rA, (rBv+rCv));
}
static inline void exe_multiply(UM_Instruction i) {
        Word rBv = register_load(i.rB);
        Word rCv = register_load(i.rC);
        register_store(i.rA, (rBv*rCv));
}
static inline void exe_divide(UM_Instruction i) {
        Word rBv = register_load(i.rB);
        Word rCv = register_load(i.rC);
        register_store(i.rA, (rBv/rCv));
}
static inline void exe_nand(UM_Instruction i) {
        Word rBv = register_load(i.rB);
        Word rCv = register_load(i.rC);
        register_store(i.rA, ~(rBv & rCv));
}
static inline void exe_halt(UM_Instruction i) {
        (void)i;
        program_counter = program_length;
}
static inline void exe_map(UM_Instruction i) {
        Word rCv = register_load(i.rC);
        SegmentID seg = map(rCv);
        register_store(i.rB, seg);
}
static inline void exe_unmap(UM_Instruction i) {
        Word rCv = register_load(i.rC);
        unmap(rCv);
}
static inline void exe_output(UM_Instruction i) {
        Word rCv = register_load(i.rC);
        write(rCv);
}
static inline void exe_input(UM_Instruction i) {
        register_store(i.rC, read());
}
static inline void exe_jump(UM_Instruction i) {
        Word rBv = register_load(i.rB);
        Word rCv = register_load(i.rC);
        program_counter = rCv - 1;
        if (rBv) {
                duplicate(rBv);
        }
}
static inline void exe_loadval(UM_Instruction i) {
        register_store(i.rA, i.value);
}

/* QUEUE */

static inline void Queue_new(unsigned hint, size_t size) {
        memory.unmapped_ids.head = 0;
        memory.unmapped_ids.length = 0;
        memory.unmapped_ids.array.length = hint;
        memory.unmapped_ids.array.values = malloc(size * hint);
        memory.unmapped_ids.array.size = size;
}

static inline Word Queue_pop() {
        if (memory.unmapped_ids.length == 0) return UM_NULL;
        Word x = memory.unmapped_ids.array.values[(memory.unmapped_ids.head%memory.unmapped_ids.array.length)];
        memory.unmapped_ids.head = (memory.unmapped_ids.head+1)%memory.unmapped_ids.array.length;
        --memory.unmapped_ids.length;
        return x;
}

static inline void Queue_push(Word value) {
        unsigned i;
        if (memory.unmapped_ids.length == memory.unmapped_ids.array.length) {
                Queue_resize();
        }
        i = memory.unmapped_ids.length++;
        memory.unmapped_ids.array.values[((memory.unmapped_ids.head+i)%memory.unmapped_ids.array.length)] = value;
}


static inline void Queue_resize() {
        unsigned n = memory.unmapped_ids.array.length;
        memory.unmapped_ids.array.length *= SCALEFACTOR;
        memory.unmapped_ids.array.values = realloc(memory.unmapped_ids.array.values, memory.unmapped_ids.array.size*memory.unmapped_ids.array.length);
        if (memory.unmapped_ids.head > 0) {
                Word* old = &(memory.unmapped_ids.array.values)[memory.unmapped_ids.head];
                memcpy(old+n, old, (n - memory.unmapped_ids.head)*memory.unmapped_ids.array.size);
                memory.unmapped_ids.head += n;
        }
}

static inline void Queue_free() {
        free(memory.unmapped_ids.array.values);
}

/* ARRAY */

static inline void Array_new(unsigned hint, size_t size) {
        memory.seg_map.values = malloc(size * hint);
        memory.seg_map.length = hint;
        memory.seg_map.size = size;
}

static inline void* Array_at(unsigned pos) {
        return memory.seg_map.values + (pos * memory.seg_map.size);
}

static inline void Array_resize() {
        memory.seg_map.length *= SCALEFACTOR;
        memory.seg_map.values = realloc(memory.seg_map.values, memory.seg_map.size*memory.seg_map.length);
}

static inline void Array_free() {
        free(memory.seg_map.values);
}
