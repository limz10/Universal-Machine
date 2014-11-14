#include "io.h"

int main() {
	char word[5];

	for (int i = 0; i < 5; i++) {
		word[i] = read();
	}

	for (int i = 4; i >= 0; i--) {
		write(word[i]);
	}

	write('\n');

	// write(600);

	write('\n');

	printf("6123: %d | 213: %d\n", in_bounds(61231,255), in_bounds(213,255));
}