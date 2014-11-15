#include "controller.h"
#include "io.h"
#include "memory.h"
#include "helpers.h"

int main() {
        UM machine = UM_new();

        SegmentID segs[100];

        for(int i = 0; i < 100; i++) {
                SegmentID seg = map(machine, 10);
                fprintf(stderr, "%u\n", seg);
                segs[i] = seg;
                for (int j = 0; j < 10; j++) {
                        put_word(machine, seg, j, (Word)rand());
                }
        }

        for(int i = 0; i < 100; i++) {
                segs[i]
        }

        for(int i = 0; i < 100; i++) {
                if (!segs[i]) {
                        unmap(machine, segs[i]);
                }
        }

        UM_kill(&machine);

}