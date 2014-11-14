#include "controller.h"
#include "memory.h"

#define NUM_REGISTERS 8

UM UM_new() {
        UM machine = malloc(sizeof(struct UM_T));
        machine->registers = malloc(sizeof(Word)*NUM_REGISTERS);
        machine->memory = SegmentBlock_new();
        machine->pc = 0;
        return machine;
}

bool UM_run(UM machine, FILE* program) {
        (void)machine;
        (void)program;
        return true;
}

bool UM_kill(UM* machine) {
        SegmentBlock_free(&((*machine)->memory));
        free((*machine)->registers);
        free(*machine);
        return true;
}
