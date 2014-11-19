#include "controller.h"

int main(int argc, char *argv[]) {
        FILE* program;
        if (argc == 2) {
                program = fopen(argv[1], "r");
        } else {
                program = NULL;
        }

        if (!program) {
                fprintf(stderr, "usage: ./um <file in um binary format>\n");
                exit(1);
        }

        UM machine = UM_new();

        UM_run(machine, program);

        fclose(program);

        UM_kill(&machine);
}