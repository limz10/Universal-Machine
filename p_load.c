#include "p_load.h"
#include "memory.h"

static unsigned wordsInFile(FILE* program);
static Word readWord(FILE* program);

bool load(UM machine, FILE* program) {
        unsigned pl = wordsInFile(program);
        SegmentID program_seg = map(machine, pl);
        assert(program_seg == 0);
        machine->program_length = pl;

        for(unsigned i = 0; i < pl; i++) {
                put_word(machine, program_seg, i, readWord(program));
        }

        return true;
        
}

static Word readWord(FILE* program) {
        Word to_return = 0;
        for(unsigned i = 0; i < sizeof(Word); i++) {
                ((unsigned char *)&to_return)[sizeof(Word)-i-1] = getc(program);
        }
        return to_return;
}

static unsigned wordsInFile(FILE* program) {
        fseek(program, 0, SEEK_END);
        unsigned fileLength = ftell(program);
        fseek(program, 0, SEEK_SET);
        return (fileLength / sizeof(Word));
}