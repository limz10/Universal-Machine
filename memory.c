#include "memory.h"

void initialize_memory(UM machine) {

}

SegmentID map(UM machine, unsigned size) {
        return 5;
}

void unmap(UM machine, SegmentID id) {

}

Word put_word(UM machine, SegmentID id, unsigned offset, Word value) {
        return 123;
}

Word get_word(UM machine, SegmentID id, unsigned offset) {
        return 321;
}