#include "memory.h"
#include "seq.h"
#include "uarray.h"

static const int SEQ_SIZE = 1000;
static const int SEG_MAP_SIZE = 1000;

/* DEFINIATION OF STRUCT to store data for our SegmentBlock */
struct SegmentBlock_T {
        Seq_T unmapped_ids;
        SegmentID next_seg;
        UArray_T seg_map;
};

/* struct to store an indivudal segment */
typedef struct Segment_T {
        Word length;
        Word words[];
} * Segment;

/* === static functions headers === */

static SegmentID new_seg_id(SegmentBlock mem) {
        SegmentID seg_id;
        if(!(Seq_length(mem->unmapped_ids))) {
                seg_id = mem->next_seg;
                mem->next_seg++;
        } else {
                seg_id = (SegmentID)(uintptr_t)Seq_remlo(mem->unmapped_ids);
        }
        return seg_id;
}

static Segment allocate_segment(unsigned length) {
        return malloc(sizeof(Segment)+(sizeof(Word)*length));
}

/* === IMPLEMENTATION === */

SegmentBlock SegmentBlock_new() {
        SegmentBlock mem = malloc(sizeof(struct SegmentBlock_T));
        mem->unmapped_ids = Seq_new(SEQ_SIZE);
        mem->next_seg = 0;
        mem->seg_map = UArray_new(SEG_MAP_SIZE, sizeof(Segment));
        return mem;
}

bool SegmentBlock_free(SegmentBlock* mem) {
        Seq_free(&((*mem)->unmapped_ids));
        UArray_free(&((*mem)->seg_map));
        free(*mem);
        return true;
}

SegmentID map(UM machine, unsigned length) {
        SegmentBlock mem = machine->memory;
        SegmentID seg_id = new_seg_id(mem);
        Segment seg = UArray_at(mem->seg_map, seg_id);
        seg = allocate_segment(length);
        (void)seg;
        return seg_id;
}

void unmap(UM machine, SegmentID id) {
        UArray_T seg_map = machine->memory->seg_map;
        Segment seg = UArray_at(seg_map, id);
        free(seg);
        seg = (Segment)UM_NULL;
        (void)seg;
        Seq_addhi(machine->memory->unmapped_ids, (void *)(uintptr_t)id);
}

Word put_word(UM machine, SegmentID id, unsigned offset, Word value) {
        (void)machine;
        (void)id;
        (void)offset;
        (void)value;
        return 123;
}

Word get_word(UM machine, SegmentID id, unsigned offset) {
        (void)machine;
        (void)id;
        (void)offset;
        return 321;
}