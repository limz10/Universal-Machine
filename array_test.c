#include "dyn_array.h"

int main() {
        Queue my_queue = Queue_new(100);
        for(unsigned i = 0; i < 10000; i++) {
                Queue_push(my_queue, i);
        }

        for(unsigned j = 0; j < 10000; j++) {
                printf("%u\n", Queue_pop(my_queue));
        }

        Queue_free(&my_queue);
}