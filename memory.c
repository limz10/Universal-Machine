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

static Word* allocate_segment(unsigned length) {
        return calloc(sizeof(Word)*(length+1), sizeof(Word));
}

/* === IMPLEMENTATION === */

SegmentBlock SegmentBlock_new() {
        SegmentBlock mem = malloc(sizeof(struct SegmentBlock_T));
        mem->unmapped_ids = Seq_new(SEQ_SIZE);
        mem->next_seg = 0;
        mem->seg_map = UArray_new(SEG_MAP_SIZE, sizeof(uintptr_t));
        return mem;
}

bool SegmentBlock_free(SegmentBlock* mem) {
        free(*((Word**)UArray_at((*mem)->seg_map, 0)));
        Seq_free(&((*mem)->unmapped_ids));
        UArray_free(&((*mem)->seg_map));
        free(*mem);
        return true;
}

SegmentID map(UM machine, unsigned length) {
        SegmentBlock mem = machine->memory;
        UArray_T seg_map = mem->seg_map;
        SegmentID seg_id = new_seg_id(mem);
        unsigned seg_map_length = UArray_length(seg_map);
        if (seg_id >= seg_map_length) {
                UArray_resize(seg_map, (seg_map_length+SEG_MAP_SIZE));
        }
        Word** seg = UArray_at(seg_map, seg_id);
        *seg = allocate_segment(length);
        (*seg)[0] = length;
        return seg_id;
}

void unmap(UM machine, SegmentID id) {
        UArray_T seg_map = machine->memory->seg_map;
        unsigned seg_map_length = UArray_length(seg_map);

        assert(id < seg_map_length);
        Word** seg = UArray_at(seg_map, id);

        assert(seg);

        free(*seg);
        *seg = NULL;
        Seq_addhi(machine->memory->unmapped_ids, (void *)(uintptr_t)id);
}

void duplicate(UM machine, SegmentID from, SegmentID to) {
        SegmentBlock mem = machine->memory;

        Word** seg1 = UArray_at(mem->seg_map, from);
        Word** seg2 = UArray_at(mem->seg_map, to);

        *seg2 = *seg1;
}

Word put_word(UM machine, SegmentID id, unsigned offset, Word value) {
        UArray_T seg_map = machine->memory->seg_map;

        assert(id < (unsigned)UArray_length(seg_map));
        Word** seg = UArray_at(seg_map, id);

        assert(seg && *seg);
        assert(offset < (*seg)[0]);

        Word to_return = (*seg)[offset+1];
        (*seg)[offset+1] = value;
        return to_return;
}

Word get_word(UM machine, SegmentID id, unsigned offset) {
        UArray_T seg_map = machine->memory->seg_map;

        assert(id < (unsigned)UArray_length(seg_map));
        Word** seg = UArray_at(seg_map, id);

        assert(seg && *seg);
        assert(offset < (*seg)[0]);

        return (*seg)[offset+1];
}