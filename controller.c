#include "controller.h"
#include "memory.h"

#define NUM_REGISTERS 8

UM UM_new() {
        UM machine = (UM)malloc(sizeof(struct UM_T));
        machine->registers = (RegisterBank)malloc(sizeof(Word)*NUM_REGISTERS);
        initialize_memory(machine);
        machine->pc = 0;
}

bool UM_run(UM machine, FILE* program) {

}

bool UM_kill(UM* machine) {
        free((*machine)->registers);
        free(*machine);
}
