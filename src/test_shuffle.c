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

struct sqids_shuffle_test_s {
    char *str;
    char *exp;
    int line;
};
typedef struct sqids_shuffle_test_s sqids_shuffle_test_t;

sqids_shuffle_test_t sqids_shuffle_tests[] = {
    {SQIDS_DEFAULT_ALPHABET,
        "fwjBhEY2uczNPDiloxmvISCrytaJO4d71T0W3qnMZbXVHg6eR8sAQ5KkpLUGF9", __LINE__},
    {"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ",
        "ec38UaynYXvoxSK7RV9uZ1D2HEPw6isrdzAmBNGT5OCJLk0jlFbtqWQ4hIpMgf", __LINE__},
    {"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ",
        "ec38UaynYXvoxSK7RV9uZ1D2HEPw6isrdzAmBNGT5OCJLk0jlFbtqWQ4hIpMgf", __LINE__},
    {"1023456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ",
        "xI3RUayk1MSolQK7e09zYmFpVXPwHiNrdfBJ6ZAT5uCWbntgcDsEqjv4hLG28O", __LINE__},
    {"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ",
        "ec38UaynYXvoxSK7RV9uZ1D2HEPw6isrdzAmBNGT5OCJLk0jlFbtqWQ4hIpMgf", __LINE__},
    {"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXZY",
        "x038UaykZMSolIK7RzcbYmFpgXEPHiNr1d2VfGAT5uJWQetjvDswqn94hLC6BO", __LINE__},
    {"0123456789",
        "4086517392", __LINE__},
    {"12345",
        "24135", __LINE__},
    {"abcdefghijklmnopqrstuvwxyz",
        "lbfziqvscptmyxrekguohwjand", __LINE__},
    {"ABCDEFGHIJKLMNOPQRSTUVWXYZ",
        "ZXBNSIJQEDMCTKOHVWFYUPLRGA", __LINE__},
    {NULL, NULL, 0},
};

char *sqids_shuffle_failures[lengthof(sqids_shuffle_tests)] = {};

int
main(int argc, char **argv)
{
    int i, j;
    sqids_shuffle_test_t *test;
    char *dup, *err;

    for (i = 0, j = 0;; ++i) {
        test = &sqids_shuffle_tests[i];

        if (!test->str || !test->exp || !test->line) {
            break;
        }

        if (i && i % 72 == 0) {
            fputs("\n", stdout);
        }

        dup = strdup(test->str);
        sqids_shuffle(dup);

        if (strcmp(dup, test->exp) == 0) {
            fputc('.', stdout);
        } else {
            fputc('F', stdout);

            (void)asprintf(
                &err,
                "%s:%d: "
                "sqids_shuffle(\"%s\")\n"
                "  expected: \"%s\",\n"
                "       got: \"%s\"\n",
                __FILE__,
                test->line,
                test->str,
                test->exp,
                dup);
            sqids_shuffle_failures[j++] = err;
        }

        free(dup);
    }

    fputs("\n", stdout);

    if (j) {
        fputs("\n", stdout);
    }

    for (i = 0;; ++i) {
        if (!sqids_shuffle_failures[i]) {
            break;
        }

        fputs(sqids_shuffle_failures[i], stderr);
        free(sqids_shuffle_failures[i]);
    }

    return j;
}
