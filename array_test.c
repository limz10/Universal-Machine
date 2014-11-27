#include "dyn_array.h"
#include "seq.h"

int main(int argc, char const *argv[]) {
        (void)argv;
        unsigned num = 10000;
        if (argc == 2) {
                Seq_T seq = Seq_new(100);
                for(unsigned i = 0; i < num; i++) {
                        Seq_addhi(seq, (void *)(uintptr_t)i);
                }

                for(unsigned j = 0; j < num; j++) {
                        printf("%u\n", (unsigned)(uintptr_t)Seq_remlo(seq));
                }
                Seq_free(&seq);
                fprintf(stderr, "SEQ\n");
        } else {
                Queue my_queue = Queue_new(100, 4);
                for(unsigned i = 0; i < num; i++) {
                        Queue_push(my_queue, i);
                }

                for(unsigned j = 0; j < num; j++) {
                        printf("%u\n", Queue_pop(my_queue));
                }

                Queue_free(&my_queue);
                fprintf(stderr, "QUEUE\n");
        }
}