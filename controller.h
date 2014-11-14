#ifndef CONTROLLER
#define CONTROLLER

#include "helpers.h"

UM UM_new();

bool UM_run(UM machine, FILE* program);

bool UM_kill(UM* machine);

#endif