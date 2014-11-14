#include "memory.h"
#include "seq.h"
#include "uarray.h"

static const int SEQ_SIZE = 1000;
static const int SEG_MAP_SIZE = 1000;

struct SegmentBlock_T {
        Seq_T unmapped_ids;
        SegmentID next_seg;
        UArray_T seg_map;
};

SegmentBlock SegmentBlock_new() {
        SegmentBlock mem;
        mem = malloc(sizeof(struct SegmentBlock_T));
        mem->unmapped_ids = Seq_new(SEQ_SIZE);
        mem->next_seg = 0;
        mem->seg_map = UArray_new(SEG_MAP_SIZE, sizeof(uintptr_t));
        return mem;
}

bool SegmentBlock_free(SegmentBlock* mem) {
        Seq_free(&((*mem)->unmapped_ids));
        UArray_free(&((*mem)->seg_map));
        free(*mem);
        return true;
}

SegmentID map(UM machine, unsigned size) {
        (void)machine;
        (void)size;
        return 5;
}

void unmap(UM machine, SegmentID id) {
        (void)machine;
        (void)id;
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