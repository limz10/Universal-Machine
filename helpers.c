#include "helpers.h"

bool in_bounds(Word to_check, Word bounds) {
        return ((to_check & bounds) == to_check);
}