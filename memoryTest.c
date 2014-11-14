/* a series of unit tests to test our memory management module */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "assert.h"
#include "memory.h"

extern UM UM_new();
extern void UM_run(UM machine, FILE* program);
extern void UM_kill(UM machine);

void cannot_access_unmapped_segment_ids() {
        UM machine = UM_new();
        // should throw an error and exit since 123123 is not
        // a segment ID of a mapped segment
        get_word(machine, 123123, 0);
        UM_kill(machine);
}

void should_fail_on_out_of_bounds() {
        UM machine = UM_new();
        SegmentID seg = map(machine, 10);
        // 15 is out of bounds on a segment of size 10
        get_word(machine, seg, 15);
        // 10 should also be out of bounds because it is 0 indexed
        get_word(machine, seg, 10);
        UM_kill(machine);
}

void run_tests() {
        should_fail_on_out_of_bounds();
        cannot_access_unmapped_segment_ids();
}

int main() {
        run_tests();
        printf("Passed!!!\n");
}