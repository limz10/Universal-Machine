#include "../io.h"
#include <stdio.h>

extern bool in_bounds(Word to_check);

int main() {
	char word[5];

	for (int i = 0; i < 5; i++) {
		word[i] = read();
	}

	for (int i = 4; i >= 0; i--) {
		write(word[i]);
	}

	write('\n');

	write(600);

	write('\n');

	printf("6123: %d | 213: %d\n", in_bounds(6123), in_bounds(213));
}