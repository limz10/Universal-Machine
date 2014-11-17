#include "p_load.h"
#include "controller.h"
#include "io.h"
#include "memory.h"
#include "helpers.h"
#include "um-dis.h"

int main(int argc, char *argv[]) {
        (void)argc;
        UM machine = UM_new();

        FILE* program = fopen(argv[1], "r");

        load(machine, program);

        for(unsigned i = 0; i < machine->program_length; i++) {
                Word instruction = get_word(machine, 0, i);
                fprintf(stdout, "%s\n", Um_disassemble(instruction));
        }

        fclose(program);
        UM_kill(&machine);

}