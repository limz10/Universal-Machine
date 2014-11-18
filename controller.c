#include "controller.h"
#include "memory.h"
#include "p_load.h"
#include "execute.h"

#define NUM_REGISTERS 8

UM UM_new() {
        UM machine = malloc(sizeof(struct UM_T));
        machine->registers = calloc(sizeof(Word)*NUM_REGISTERS, sizeof(Word));
        machine->memory = SegmentBlock_new();
        machine->pc = 0;
        machine->program_length = 0;
        return machine;
}

bool UM_run(UM machine, FILE* program) {
        load(machine, program);
        while(machine->pc < machine->program_length) {
                execute(machine);
                machine->pc++;
        }
        return true;
}

bool UM_kill(UM* machine) {
        SegmentBlock_free(&((*machine)->memory));
        free((*machine)->registers);
        free(*machine);
        return true;
}
