#include "controller.h"
#include "io.h"
#include "memory.h"
#include "helpers.h"

int main() {
        UM machine = UM_new();

        fprintf(stderr, "HELLLOOOO\n");

        UM_kill(&machine);

}