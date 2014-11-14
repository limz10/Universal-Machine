#include "helpers.h"

Word register_store(UM machine, int id, Word value) {
	if (!in_bounds(id, 7)) {
		fprintf(stderr, "Register %d does not exist. Must be in [0, 7]!\n", id);
                exit(1);
	}
	Word to_return = register_load(machine, id);

	machine->registers[id] = value;

	return to_return;
}

Word register_load(UM machine, int id) {
	if (!in_bounds(id, 7)) {
		fprintf(stderr, "Register %d does not exist. Must be in [0, 7]!\n", id);
                exit(1);
	}
	return machine->registers[id];
}
