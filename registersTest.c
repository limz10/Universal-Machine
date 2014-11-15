#include "controller.h"
#include "io.h"
#include "helpers.h"

int main() {
	UM machine = UM_new();

	// for (int i = 0; i < 10; i++) {
	// 	register_store(machine, i%8, read());
	// }

	Word input = read();

	int i = 0;

	// UM_NULL signifies EOF
	while (input!=UM_NULL) {
		register_store(machine, i%8, input);
		input = read();
		i++;
	}

	write('\n');
	write('\n');
	for (i = 7; i >= 0; i--) {
		write(register_load(machine, i%8));
	}

	write('\n');

	UM_kill(&machine);

}