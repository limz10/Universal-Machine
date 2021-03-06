#include "io.h"

Word read() {
        Word to_return = getchar();

        if(to_return != (Word)EOF && (to_return > 255)) {
                fprintf(stderr, "You cannot read value %u!\n",
                (unsigned)to_return);
                exit(1);
        }

        return to_return;
}

Word write(Word to_write) {
        if(to_write > 255) {
                fprintf(stderr, "You cannot write value %u!\n",
                (unsigned)to_write);
                exit(1);
        }

        Word to_return = putchar((char)to_write);

        return to_return;
}

