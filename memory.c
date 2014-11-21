#include "memory.h"
#include "seq.h"
#include "uarray.h"

struct UArray_T {
        int   length;
        int   size;
        char *elems;
};

static const int SEQ_SIZE = 1000;
static const int SEG_MAP_SIZE = 1000;

static unsigned seq_length;
static unsigned seg_map_length;

/* DEFINIATION OF STRUCT to store data for our SegmentBlock */
struct SegmentBlock_T {
        Seq_T unmapped_ids;
        SegmentID next_seg;
        UArray_T seg_map;
};

/* === static functions headers === */

/* returns a unique SegmentID which is currently not in use. */
static inline SegmentID new_seg_id(SegmentBlock mem);

/* allocates memory for a new segmend and returns the pointer to it */
static inline Word* allocate_segment(unsigned length);

/* gets the segment from our seg_map */
static inline void* get_segment(UArray_T seg_map, int seg_id);

/* === IMPLEMENTATION === */

SegmentBlock SegmentBlock_new() {
        SegmentBlock mem = malloc(sizeof(struct SegmentBlock_T));
        mem->unmapped_ids = Seq_new(SEQ_SIZE);
        seq_length = 0;
        mem->next_seg = 0;
        mem->seg_map = UArray_new(SEG_MAP_SIZE, sizeof(uintptr_t));
        seg_map_length = SEG_MAP_SIZE;
        return mem;
}

bool SegmentBlock_free(SegmentBlock* mem) {
        free(*((Word**)get_segment((*mem)->seg_map, 0)));
        Seq_free(&((*mem)->unmapped_ids));
        UArray_free(&((*mem)->seg_map));
        seq_length = 0;
        seg_map_length = 0;
        free(*mem);
        return true;
}

SegmentID map(UM machine, unsigned length) {
        SegmentBlock mem = machine->memory;
        UArray_T seg_map = mem->seg_map;
        SegmentID seg_id = new_seg_id(mem);
        if (seg_id >= seg_map_length) {
                seg_map_length = seg_map_length + SEG_MAP_SIZE;
                UArray_resize(seg_map, seg_map_length);
        }
        Word** seg = get_segment(seg_map, seg_id);
        if(!((*seg) = allocate_segment(length))) {
                fprintf(stderr, "Machine is out of memory!\n");
                exit(1);
        }
        (*seg)[0] = length;
        return seg_id;
}

void unmap(UM machine, SegmentID id) {
        UArray_T seg_map = machine->memory->seg_map;

        assert(id < seg_map_length);
        Word** seg = get_segment(seg_map, id);

        assert(seg);

        free(*seg);
        *seg = NULL;
        Seq_addhi(machine->memory->unmapped_ids, (void *)(uintptr_t)id);
        seq_length++;
}

void duplicate(UM machine, SegmentID from, SegmentID to) {
        SegmentBlock mem = machine->memory;

        Word** seg1 = get_segment(mem->seg_map, from);
        unsigned seg1_length = (*seg1)[0];
        Word** seg2 = get_segment(mem->seg_map, to);
        free(*seg2);
        *seg2 = allocate_segment(seg1_length);
        for(unsigned i = 0; i <= seg1_length; i++) {
                (*seg2)[i] = (*seg1)[i];
        }
        if (to == 0) {
                machine->program_length = (*seg2)[0];
        }
}

Word put_word(UM machine, SegmentID id, unsigned offset, Word value) {
        UArray_T seg_map = machine->memory->seg_map;

        assert(id < seg_map_length);
        Word** seg = get_segment(seg_map, id);

        assert(seg && *seg);
        assert(offset < (*seg)[0]);

        Word to_return = (*seg)[offset+1];
        (*seg)[offset+1] = value;
        return to_return;
}

Word get_word(UM machine, SegmentID id, unsigned offset) {
        UArray_T seg_map = machine->memory->seg_map;

        assert(id < seg_map_length);
        Word** seg = get_segment(seg_map, id);

        assert(seg && *seg);
        assert(offset < (*seg)[0]);

        return (*seg)[offset+1];
}

/* STATIC INLINE */

static inline SegmentID new_seg_id(SegmentBlock mem) {
        SegmentID seg_id;
        if(!seq_length) {
                seg_id = mem->next_seg;
                mem->next_seg++;
        } else {
                seg_id = (SegmentID)(uintptr_t)Seq_remlo(mem->unmapped_ids);
                seq_length--;
        }
        return seg_id;
}

static inline Word* allocate_segment(unsigned length) {
        return calloc(sizeof(Word)*(length+1), sizeof(Word));
}

static inline void* get_segment(UArray_T seg_map, int seg_id) {
        return seg_map->elems + seg_id * seg_map->size;
}