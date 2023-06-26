#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "sqids.h"

int
main(int argc, char **argv)
{
    sqids_str_t *str;

    assert((str = sqids_str_new()));
    assert(sqids_str_append_str(str, "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcde"));
    assert(str->len == 63);
    assert(str->siz == 64);
    assert(strcmp(str->s, "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcde") == 0);
    assert(sqids_str_append_char(str, 'f'));
    assert(str->len == 64);
    assert(str->siz == 128);
    sqids_str_free(str);

    assert((str = sqids_str_new()));
    assert(str->len == 0);
    assert(str->siz == 64);
    assert(sqids_str_append_str(str, "0123456789abcdef"));
    assert(str->len == 16);
    assert(str->siz == 64);
    assert(strcmp(str->s, "0123456789abcdef") == 0);
    assert(sqids_str_append_str(str, "0123456789abcdef"));
    assert(str->len == 32);
    assert(str->siz == 64);
    assert(strcmp(str->s, "0123456789abcdef0123456789abcdef") == 0);
    assert(sqids_str_append_str(str, "0123456789abcdef"));
    assert(str->len == 48);
    assert(str->siz == 64);
    assert(strcmp(str->s, "0123456789abcdef0123456789abcdef0123456789abcdef") == 0);
    assert(sqids_str_append_str(str, "0123456789abcdef"));
    assert(str->len == 64);
    assert(str->siz == 128);
    assert(strcmp(str->s, "0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef") == 0);
    sqids_str_free(str);

    return EXIT_SUCCESS;
}
