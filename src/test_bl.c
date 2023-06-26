#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "sqids.h"

#ifndef lengthof
#define lengthof(x) ((size_t)(sizeof(x) / sizeof(x[0])))
#endif

struct sqids_bl_test_s {
    char *bad_words[16];
    char *str;
    int exp;
    int line;
};
typedef struct sqids_bl_test_s sqids_bl_test_t;

sqids_bl_test_t sqids_bl_tests[] = {
    {{"sexy", NULL}, "sexy", 1, __LINE__},
    {{"sexy", NULL}, "1sexy", 1, __LINE__},
    {{"sexy", NULL}, "sexy2", 1, __LINE__},
    {{"sexy", NULL}, "se1xy", 0, __LINE__},
    {{NULL}, NULL, 0, 0},
};

char *sqids_bl_failures[lengthof(sqids_bl_tests)] = {};

int
main(int argc, char **argv)
{
    int i, j, k, r;
    sqids_bl_test_t *test;
    sqids_bl_list_t *bl;
    char *err;

    for (i = 0, j = 0, k = 0;; ++i) {
        test = &sqids_bl_tests[i];

        if (!test->str || !test->exp || !test->line) {
            break;
        }

        if (i && i % 72 == 0) {
            fputs("\n", stdout);
        }

        bl = sqids_bl_list_new(sqids_bl_match_func);

        for (j = 0; test->bad_words[j]; ++j) {
            sqids_bl_add_tail(bl, test->bad_words[j]);
        }

        r = sqids_bl_find(bl, test->str) != NULL;

        if (r == test->exp) {
            fputc('.', stdout);
        } else {
            fputc('F', stdout);

            (void)asprintf(
                &err,
                "%s:%d: "
                "sqids_bl_find(\"%s\")\n"
                "  expected: \"%d\",\n"
                "       got: \"%d\"\n",
                __FILE__,
                test->line,
                test->str,
                test->exp,
                r);
            sqids_bl_failures[k++] = err;
        }

        sqids_bl_list_free(bl);
    }

    fputs("\n", stdout);

    if (k) {
        fputs("\n", stdout);
    }

    for (i = 0;; ++i) {
        if (!sqids_bl_failures[i]) {
            break;
        }

        fputs(sqids_bl_failures[i], stderr);
        free(sqids_bl_failures[i]);
    }

    return k;
}
