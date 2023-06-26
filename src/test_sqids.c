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

struct sqids_sqids_test_s {
    char *alphabet;
    int min_len;
    int num_cnt;
    unsigned long long nums[128];
    char *exp;
    int line;
};
typedef struct sqids_sqids_test_s sqids_sqids_test_t;

sqids_sqids_test_t sqids_sqids_tests[] = {
    {SQIDS_DEFAULT_ALPHABET, 0, 1, {0}, "bM", __LINE__},
    {SQIDS_DEFAULT_ALPHABET, 0, 1, {1}, "Uk", __LINE__},
    {SQIDS_DEFAULT_ALPHABET, 0, 1, {2}, "gb", __LINE__},
    {SQIDS_DEFAULT_ALPHABET, 0, 1, {3}, "Ef", __LINE__},
    {SQIDS_DEFAULT_ALPHABET, 0, 1, {4}, "Vq", __LINE__},
    {SQIDS_DEFAULT_ALPHABET, 0, 1, {5}, "uw", __LINE__},
    {SQIDS_DEFAULT_ALPHABET, 0, 1, {6}, "OI", __LINE__},
    {SQIDS_DEFAULT_ALPHABET, 0, 1, {7}, "AX", __LINE__},
    {SQIDS_DEFAULT_ALPHABET, 0, 1, {8}, "p6", __LINE__},
    {SQIDS_DEFAULT_ALPHABET, 0, 1, {9}, "nJ", __LINE__},

    {SQIDS_DEFAULT_ALPHABET, 0, 2, {0, 0}, "SvIz", __LINE__},
    {SQIDS_DEFAULT_ALPHABET, 0, 2, {0, 1}, "n3qa", __LINE__},
    {SQIDS_DEFAULT_ALPHABET, 0, 2, {0, 2}, "tryF", __LINE__},
    {SQIDS_DEFAULT_ALPHABET, 0, 2, {0, 3}, "eg6q", __LINE__},
    {SQIDS_DEFAULT_ALPHABET, 0, 2, {0, 4}, "rSCF", __LINE__},
    {SQIDS_DEFAULT_ALPHABET, 0, 2, {0, 5}, "sR8x", __LINE__},
    {SQIDS_DEFAULT_ALPHABET, 0, 2, {0, 6}, "uY2M", __LINE__},
    {SQIDS_DEFAULT_ALPHABET, 0, 2, {0, 7}, "74dI", __LINE__},
    {SQIDS_DEFAULT_ALPHABET, 0, 2, {0, 8}, "30WX", __LINE__},
    {SQIDS_DEFAULT_ALPHABET, 0, 2, {0, 9}, "moxr", __LINE__},

    {SQIDS_DEFAULT_ALPHABET, 0, 2, {0, 0}, "SvIz", __LINE__},
    {SQIDS_DEFAULT_ALPHABET, 0, 2, {1, 0}, "nWqP", __LINE__},
    {SQIDS_DEFAULT_ALPHABET, 0, 2, {2, 0}, "tSyw", __LINE__},
    {SQIDS_DEFAULT_ALPHABET, 0, 2, {3, 0}, "eX68", __LINE__},
    {SQIDS_DEFAULT_ALPHABET, 0, 2, {4, 0}, "rxCY", __LINE__},
    {SQIDS_DEFAULT_ALPHABET, 0, 2, {5, 0}, "sV8a", __LINE__},
    {SQIDS_DEFAULT_ALPHABET, 0, 2, {6, 0}, "uf2K", __LINE__},
    {SQIDS_DEFAULT_ALPHABET, 0, 2, {7, 0}, "7Cdk", __LINE__},
    {SQIDS_DEFAULT_ALPHABET, 0, 2, {8, 0}, "3aWP", __LINE__},
    {SQIDS_DEFAULT_ALPHABET, 0, 2, {9, 0}, "m2xn", __LINE__},

    {SQIDS_DEFAULT_ALPHABET, 0, 100, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
        13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30,
        31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48,
        49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66,
        67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84,
        85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99},
        "iPDjfx9tCly2b0HKAAmOrwkl7RucWXsIGZ68AslVAwxdGB6ua5Nw60t3zxVjXAnJ1NgwX"
        "ABfs9AVtV9XW5OJQxuHg2VXmFYr5k48OMuGyesD3UETwUsqbtGXCJp6yCssOsAgMDpf0Z"
        "wUgx9JEczPzMrb9PIGcLPjwmH7xaWOpdQmk1zHw4XMRX6MOh1s96qWVxdb9fdEvat4KzH"
        "INW7VMoglaKdCy3z5bnEsVYEQxl4ICjo", __LINE__},

#if defined(SQIDS_DEFAULT_BLOCKLIST) && SQIDS_DEFAULT_BLOCKLIST == 1
    {SQIDS_DEFAULT_ALPHABET, 0, 1, {4572721}, "JExTR", __LINE__},
#endif

    {NULL, 0, 0, {}, NULL, 0},
};

char *sqids_sqids_failures[lengthof(sqids_sqids_tests) + 1] = {};

int
main(int argc, char **argv)
{
    int i, j;
    sqids_sqids_test_t *test;
    sqids_bl_list_t *bl;
    sqids_t *sqids;
    char *enc, *err;

    for (i = 0, j = 0;; ++i) {
        test = &sqids_sqids_tests[i];

        if (!test->num_cnt || !test->exp || !test->line) {
            break;
        }

        if (i && i % 72 == 0) {
            fputs("\n", stdout);
        }

        sqids = sqids_new(test->alphabet, test->min_len,
            sqids_bl_list_all(NULL));
        enc = sqids_encode(sqids, test->num_cnt, test->nums);

        if (strcmp(enc, test->exp) == 0) {
            fputc('.', stdout);
        } else {
            fputc('F', stdout);

            (void)asprintf(
                &err,
                "%s:%d: "
                "sqids_encode(...)\n"
                "  expected: \"%s\",\n"
                "       got: \"%s\"\n",
                __FILE__,
                test->line,
                test->exp,
                enc);
            sqids_sqids_failures[j++] = err;
        }

        sqids_mem_free(enc);
        sqids_free(sqids);
    }

    /* test edge case where all the possibilities are blocked */
    bl = sqids_bl_list_new(sqids_bl_match_func);
    sqids_bl_add_tail(bl, "abc");
    sqids_bl_add_tail(bl, "bca");
    sqids_bl_add_tail(bl, "cab");
    sqids = sqids_new("abc", 3, bl);
    enc = sqids_vencode(sqids, 1, 0);
    if (!enc) {
        fputc('.', stdout);
    } else {
        fputc('F', stdout);

        (void)asprintf(
            &err,
            "%s:%d: "
            "sqids_encode(...)\n"
            "  expected: NULL,\n"
            "       got: \"%s\"\n",
            __FILE__,
            __LINE__,
            enc);
        sqids_sqids_failures[j++] = err;
        free(enc);
    }
    sqids_free(sqids);

    fputs("\n", stdout);

    if (j) {
        fputs("\n", stdout);
    }

    for (i = 0;; ++i) {
        if (!sqids_sqids_failures[i]) {
            break;
        }

        fputs(sqids_sqids_failures[i], stderr);
        free(sqids_sqids_failures[i]);
    }

    return j;
}
