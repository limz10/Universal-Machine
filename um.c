#include "controller.h"

int main(int argc, char *argv[]) {
        assert(argc == 2);
        UM machine = UM_new();

        FILE* program = fopen(argv[1], "r");

        UM_run(machine, program);

        fclose(program);

        UM_kill(&machine);
}