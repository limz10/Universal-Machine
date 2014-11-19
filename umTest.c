#include "tests.h"

/* TESTS TO RUN */

extern void emit_halt_test(Seq_T asm);
extern void emit_goto_test(Seq_T asm);
extern void emit_map_memory_test(Seq_T asm);
extern void emit_arith_test(Seq_T asm);


static struct test_info {
        const char *name;
        const char *test_input;            /* NULL means no input needed */
        const char *expected_output;
        /* writes instructions into sequence */
        void (*emit_test) (Seq_T stream);
} tests[] = {
        {"halt", NULL, "", emit_halt_test},
        {"goto", NULL, "GOTO passed.\n", emit_goto_test},
        {"arith", "", "5", emit_arith_test},
        {"mem", NULL, "", emit_map_memory_test}
};

#define NTESTS (sizeof(tests)/sizeof(tests[0]))

/* HELPERS FOR WRITING */

static void writeWord(FILE* output, Um_instruction inst) {
        unsigned char * inst_str = (unsigned char *)&inst;
        for(unsigned i = 0; i < sizeof(Um_instruction); i++) {
            fputc(inst_str[sizeof(Um_instruction)-i-1], output);
        }
}

void Um_write_sequence(FILE *output, Seq_T stream) {
    unsigned seq_length = Seq_length(stream);
    for(unsigned i = 0; i < seq_length; i++) {
        writeWord(output, get_inst(stream, i));
    }
}

static void write_or_remove_file(char *path, const char *contents)
{
        if (contents == NULL || *contents == '\0') {
                remove(path);
        } else {
                FILE *input = fopen(path, "wb");
                assert(input);
                fputs(contents, input);
                fclose(input);
        }
        free(path);
}

static FILE *open_and_free_pathname(char *path)
{
        FILE *fp = fopen(path, "wb");
        assert(fp != NULL);

        free(path);
        return fp;
}

static void write_test_files(struct test_info *test)
{
        FILE *binary = open_and_free_pathname(Fmt_string("%s.um", test->name));
        Seq_T asm = Seq_new(0);
        test->emit_test(asm);
        Um_write_sequence(binary, asm);
        Seq_free(&asm);
        fclose(binary);

        write_or_remove_file(Fmt_string("%s.0", test->name), test->test_input);
        write_or_remove_file(Fmt_string("%s.1", test->name),
                             test->expected_output);
}

/* MAIN */

int main(int argc, char *argv[])
{
        bool failed = false;
        if (argc == 1)
                for (unsigned i = 0; i < NTESTS; i++) {
                        printf("***** Writing test '%s'.\n", tests[i].name);
                        write_test_files(&tests[i]);
        } else
                for (int j = 1; j < argc; j++) {
                        bool tested = false;
                        for (unsigned i = 0; i < NTESTS; i++)
                                if (!strcmp(tests[i].name, argv[j])) {
                                        tested = true;
                                        write_test_files(&tests[i]);
                                }
                        if (!tested) {
                                failed = true;
                                fprintf(stderr,
                                        "***** No test named %s *****\n",
                                        argv[j]);
                        }
                }
        return failed;   /* failed nonzero == exit nonzero == failure */
}
