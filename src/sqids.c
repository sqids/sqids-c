#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>

/* only for debugging */
#include <stdio.h>

#include "sqids.h"

/*****************************************************************************/
/* {{{ memory stuff                                                          */
/*****************************************************************************/

/* default alloc() implementation */
static inline void *
sqids_mem_alloc_default(unsigned int siz)
{
    return calloc(siz, 1);
}

/* default realloc() implementation */
static inline void *
sqids_mem_realloc_default(void *ptr, unsigned int siz)
{
    return realloc(ptr, siz);
}

/* default free() implementation */
static inline void
sqids_mem_free_default(void *ptr)
{
    free(ptr);
}

/* default strdup() implementation */
static inline char *
sqids_mem_strdup_default(char *src)
{
    return strdup(src);
}

void *(*sqids_mem_alloc)(unsigned int) = sqids_mem_alloc_default;
void *(*sqids_mem_realloc)(void *, unsigned int) = sqids_mem_realloc_default;
void (*sqids_mem_free)(void *) = sqids_mem_free_default;
char *(*sqids_mem_strdup)(char *) = sqids_mem_strdup_default;

/* }}}                                                                       */

/*****************************************************************************/
/* {{{ string stuff                                                          */
/*****************************************************************************/

/* dynamic string chunk size */
int sqids_str_chunk = 64;

/* allocate a new dynamic string */
sqids_str_t *
sqids_str_new() {
    sqids_str_t *result;

    if (!(result = sqids_mem_alloc(sizeof(sqids_str_t)))) {
        return NULL;
    }
    if (!(result->s = sqids_mem_alloc(sqids_str_chunk))) {
        sqids_mem_free(result);
        return NULL;
    }

    result->siz = sqids_str_chunk;

    return result;
}

/* free a dynamic string */
void
sqids_str_free(sqids_str_t *str)
{
    if (str->s) {
        free(str->s);
    }

    free(str);
}

/* grow a dynamic string to specific size */
int
sqids_str_grow(sqids_str_t *str, int siz)
{
    char *s;

    if (!(s = sqids_mem_realloc(str->s, siz))) {
        return 0;
    }

    str->s = s;
    str->siz = siz;
    memset(str->s + str->len, 0, siz - str->len - 1);

    return 1;
}

/* ensure a dynamic string can hold a certain size */
int
sqids_str_cap(sqids_str_t *str, int siz)
{
    int newsiz;

    if (siz > str->siz) {
        newsiz = ceil((float)siz / (float)sqids_str_chunk) * sqids_str_chunk;
        return sqids_str_grow(str, newsiz);
    }

    return 1;
}

/* append a char to a dynamic string */
int
sqids_str_append_char(sqids_str_t *str, int ch)
{
    if (!sqids_str_cap(str, str->len + 4 + 1)) {
        return 0;
    }

    str->s[str->len++] = ch;

    return 1;
}

/* append a string to a dynamic string */
int
sqids_str_append_str(sqids_str_t *str, char *p)
{
    int len;

    len = strlen(p);
    if (!sqids_str_cap(str, str->len + len + 1)) {
        return 0;
    }

    memcpy(str->s + str->len, p, len + 1);
    str->len += len;

    return 1;
}

/* }}}                                                                       */

/*****************************************************************************/
/* {{{ blocklist stuff                                                       */
/*****************************************************************************/

/* allocate a new list */
sqids_bl_list_t *
sqids_bl_list_new(int (*match_func)(char *, char *))
{
    sqids_bl_list_t *result;

    if (!(result = sqids_mem_alloc(sizeof(sqids_bl_list_t)))) {
        return NULL;
    }

    if (!(result->match_func = match_func)) {
        result->match_func = sqids_bl_match_func;
    }

    return result;
}

/* free a list and all its data */
void
sqids_bl_list_free(sqids_bl_list_t *list)
{
    sqids_bl_node_t *iter, *next;

    sqids_bl_foreach_safe(list->head, iter, next) {
        if (iter->s) {
            sqids_mem_free(iter->s);
        }

        sqids_mem_free(iter);
    }

    sqids_mem_free(list);
}

#if !defined(SQIDS_DEFAULT_BLOCKLIST) || SQIDS_DEFAULT_BLOCKLIST == 0
/* empty `de` blocklist when compiled with --disable-default-blocklist */
sqids_bl_list_t *
sqids_bl_list_de(int (*match_func)(char *, char *))
{
    return sqids_bl_list_new(match_func);
}

/* empty `en` blocklist when compiled with --disable-default-blocklist */
sqids_bl_list_t *
sqids_bl_list_en(int (*match_func)(char *, char *))
{
    return sqids_bl_list_new(match_func);
}

/* empty es` blocklist when compiled with --disable-default-blocklist */
sqids_bl_list_t *
sqids_bl_list_es(int (*match_func)(char *, char *))
{
    return sqids_bl_list_new(match_func);
}

/* empty `fr` blocklist when compiled with --disable-default-blocklist */
sqids_bl_list_t *
sqids_bl_list_fr(int (*match_func)(char *, char *))
{
    return sqids_bl_list_new(match_func);
}

/* empty `hi` blocklist when compiled with --disable-default-blocklist */
sqids_bl_list_t *
sqids_bl_list_hi(int (*match_func)(char *, char *))
{
    return sqids_bl_list_new(match_func);
}

/* empty `it` blocklist when compiled with --disable-default-blocklist */
sqids_bl_list_t *
sqids_bl_list_it(int (*match_func)(char *, char *))
{
    return sqids_bl_list_new(match_func);
}

/* empty `pt` blocklist when compiled with --disable-default-blocklist */
sqids_bl_list_t *
sqids_bl_list_pt(int (*match_func)(char *, char *))
{
    return sqids_bl_list_new(match_func);
}

/* empty combined blocklist when compiled with --disable-default-blocklist */
sqids_bl_list_t *
sqids_bl_list_all(int (*match_func)(char *, char *))
{
    return sqids_bl_list_new(match_func);
}
#endif

/* add a string at the end of the list */
sqids_bl_node_t *
sqids_bl_add_tail(sqids_bl_list_t *list, char *s)
{
    sqids_bl_node_t *node;

    if (!(node = sqids_mem_alloc(sizeof(sqids_bl_node_t)))) {
        return NULL;
    }

    if (!(node->s = strdup(s))) {
        sqids_mem_free(node);
        return NULL;
    }

    node->prev = list->tail;
    node->next = NULL;

    /* assumption: if list->tail != NULL, list->head != NULL */
    if (list->tail != NULL) {
        list->tail->next = node;
    } else {
        list->head = node;
    }

    list->tail = node;
    ++list->len;

    return node;
}

/* add a string at the beginning of the list */
sqids_bl_node_t *
sqids_bl_add_head(sqids_bl_list_t *list, char *s)
{
    sqids_bl_node_t *node;

    if (!(node = sqids_mem_alloc(sizeof(sqids_bl_node_t)))) {
        return NULL;
    }

    if (!(node->s = strdup(s))) {
        return NULL;
    }

    node->prev = NULL;
    node->next = list->head;

    /* assumption: if list->head != NULL, list->tail != NULL */
    if (list->head != NULL) {
        list->head->prev = node;
    } else {
        list->tail = node;
    }

    list->head = node;
    ++list->len;

    return node;
}

/* search for a string in the list */
sqids_bl_node_t *
sqids_bl_find(sqids_bl_list_t *list, char *s)
{
    sqids_bl_node_t *iter;
    sqids_bl_foreach(list->head, iter) {
        if (list->match_func(s, iter->s)) {
            return iter;
        }
    }

    return NULL;
}

/* default list search func */
int
sqids_bl_match_func(char *str, char *bad_word)
{
    if (strlen(str) < strlen(bad_word)) {
        return 0;
    }

    if (strlen(str) <= 3 || strlen(bad_word) <= 3) {
        return strcasecmp(str, bad_word) == 0;
    }

    if (strpbrk(bad_word, "0123456789")) {
        return strncasecmp(str, bad_word, strlen(bad_word)) == 0 ||
            strncasecmp(str + strlen(str) - strlen(bad_word), bad_word,
                strlen(bad_word)) == 0;
    }

    return strcasestr(str, bad_word) != NULL;
}

/* }}}                                                                       */

/*****************************************************************************/
/* {{{ sqids stuff                                                           */
/*****************************************************************************/

/* allocate a new sqids structure */
sqids_t *
sqids_new(char *alphabet, int min_len, sqids_bl_list_t *blocklist)
{
    sqids_t *result;

    if (!(result = sqids_mem_alloc(sizeof(sqids_t)))) {
        return NULL;
    }

    if (!alphabet) {
        alphabet = SQIDS_DEFAULT_ALPHABET;
    }

    if (!(result->alphabet = sqids_mem_strdup(alphabet))) {
        sqids_mem_free(result);
        return NULL;
    }

    sqids_shuffle(result->alphabet);

    result->min_len = min_len;
    result->blocklist = blocklist;

    return result;
}

/* free a sqids structure */
void
sqids_free(sqids_t *sqids)
{
    if (sqids->alphabet) {
        sqids_mem_free(sqids->alphabet);
    }

    if (sqids->blocklist) {
        sqids_bl_list_free(sqids->blocklist);
    }

    sqids_mem_free(sqids);
}

/* consistent shuffle */
void
sqids_shuffle(char *alphabet)
{
    int i, j, x, tmp, len;

    for (i = 0, len = strlen(alphabet), j = len - 1; j > 0; ++i, --j) {
        x = (i * j + alphabet[i] + alphabet[j]) % len;
        tmp = alphabet[x];
        alphabet[x] = alphabet[i];
        alphabet[i] = tmp;
    }
}

/* internal encode */
static inline int
sqids_encode_internal(sqids_t *sqids, sqids_str_t *str, unsigned int num_cnt,
    unsigned long long *nums, int increment)
{
    unsigned long long num;
    int i, j, ch, len, n, tmp, offset, prefix;
    char *p, *pb;

    /* sanity check */
    len = strlen(sqids->alphabet);
    if (increment > len) {
        return 1;
    }

    /* get a semi-random offset from input numbers */
    for (i = 0, offset = num_cnt; i < num_cnt;
        ++i) {
        offset = sqids->alphabet[nums[i] % len] + i + offset;
    }
    offset %= len;

    /* take increment into account when retrying generation */
    offset = (offset + increment) % len;

    /* rearrange the internal alphabet so that the second half comes first */
    char alphabet[len + 1];
    memcpy(alphabet, sqids->alphabet + offset, len - offset);
    memcpy(alphabet + len - offset, sqids->alphabet, offset);
    alphabet[len] = 0;

    /* prefix is always the first character in the internal alphabet */
    prefix = sqids->alphabet[offset];

    /* reverse the internal alphabet */
    for (j = 0; j < len / 2; ++j) {
        tmp = alphabet[j];
        alphabet[j] = alphabet[len - j - 1];
        alphabet[len - j - 1] = tmp;
    }

    /* start with prefix */
    if (!sqids_str_append_char(str, prefix)) {
        return 1;
    }
    p = str->s + str->len;

    /* iterate over numbers and encode each */
    for (i = 0; i < num_cnt; ++i) {
        /* encode the number in reverse */
        n = 0;
        num = nums[i];
        do {
            ch = alphabet[num % (len - 1) + 1];
            if (!(sqids_str_append_char(str, ch))) {
                return 1;
            }
            ++n;
            num /= (len - 1);
        } while (num > 0);

        /* realloc() does not guarantee the same pointer after growing */
        p = str->s + str->len;
        pb = p - n;

        /* reverse it so we can decode it in the future */
        for (j = 0; j < n / 2; ++j) {
            tmp = *(pb + j);
            *(pb + j) = *(p - 1 - j);
            *(p - 1 - j) = tmp;
        }

        /* more numbers to encode - append a separator, shuffle the alphabet */
        if (i < num_cnt - 1) {
            /* the separator is the first character in the current alphabet */
            if (!sqids_str_append_char(str, alphabet[0])) {
                return 1;
            }

            sqids_shuffle(alphabet);
        }
    }

    /* handle min_len */
    if (str->len < sqids->min_len) {
        /* append the last separator */
        if (!sqids_str_append_char(str, alphabet[0])) {
            return 1;
        }

        /* keep appending separators and alphabet until we're done */
        while (str->len < sqids->min_len) {
            sqids_shuffle(alphabet);

            /* the alphabet has enough material to feed the final id,
               we can safely terminate it */
            if (sqids->min_len - str->len < len - 1) {
                alphabet[sqids->min_len - str->len] = 0;
            }

            if (!sqids_str_append_str(str, alphabet)) {
                return 1;
            }
        }
    }

    /* terminate the buffer */
    if (!sqids_str_append_char(str, 0)) {
        return 1;
    }

    /* handle bad words */
    if (sqids->blocklist && sqids_bl_find(sqids->blocklist, str->s)) {
        memset(str->s, 0, str->siz);
        str->len = 0;
        return sqids_encode_internal(sqids, str, num_cnt, nums, increment + 1);
    }

    return 0;
}

/* encode */
char *
sqids_encode(sqids_t *sqids, unsigned int num_cnt, unsigned long long *nums)
{
    sqids_str_t *str;
    char *result;

    /* allocate a string buffer */
    if (!(str = sqids_str_new())) {
        return NULL;
    }

    /* encode */
    if (sqids_encode_internal(sqids, str, num_cnt, nums, 0) != 0) {
        sqids_str_free(str);
        return NULL;
    }

    result = str->s;
    sqids_mem_free(str);

    return result;
}

/* variadic encode */
char *
sqids_vencode(sqids_t *sqids, unsigned int num_cnt, ...)
{
    sqids_str_t *str;
    char *result;
    unsigned long long nums[num_cnt];
    int i;
    va_list ap;

    /* allocate a string buffer */
    if (!(str = sqids_str_new())) {
        return NULL;
    }

    /* collect args */
    va_start(ap, num_cnt);
    for (i = 0; i < num_cnt; ++i) {
        nums[i] = va_arg(ap, unsigned long long);
    }
    va_end(ap);

    /* encode */
    if (sqids_encode_internal(sqids, str, num_cnt, nums, 0) != 0) {
        sqids_str_free(str);
        return NULL;
    }

    result = str->s;
    sqids_mem_free(str);
    return result;
}

/* decode */
int
sqids_decode(sqids_t *sqids, char *str, unsigned long long *nums,
    unsigned int num_max)
{
    unsigned long long num;
    int i, j, len, tmp, offset, prefix, separator;
    char *p;

    /* safety first - scan str for unknown characters */
    for (i = 0, j = strlen(str); i < j; ++i) {
        if (!strchr(sqids->alphabet, str[i])) {
            return 0;
        }
    }

    p = str;
    len = strlen(sqids->alphabet);

    /* extract prefix */
    prefix = *p++;

    /* determine the offset */
    offset = strchr(sqids->alphabet, prefix) - sqids->alphabet;

    /* rearrange alphabet back into its original form */
    char alphabet[len + 1];
    memcpy(alphabet, sqids->alphabet + offset, len - offset);
    memcpy(alphabet + len - offset, sqids->alphabet, offset);
    alphabet[len] = 0;

    /* reverse the internal alphabet */
    for (j = 0; j < len / 2; ++j) {
        tmp = alphabet[j];
        alphabet[j] = alphabet[len - j - 1];
        alphabet[len - j - 1] = tmp;
    }

    /* walk the hash */
    for (i = 0; *p && i < num_max;) {
        separator = alphabet[0];

        /* empty chunk - we're done */
        if (*p == separator) {
            break;
        }

        /* do parse */
        num = 0;
        for (; *p && *p != separator; ++p) {
            num *= len - 1;
            num += strchr(alphabet + 1, *p) - alphabet - 1;
        }
        nums[i++] = num;

        /* more numbers - shuffle the alphabet */
        if (*p == separator) {
            sqids_shuffle(alphabet);
            ++p;
        }
    }

    return i;
}

/* decode number count */
int
sqids_num_cnt(sqids_t *sqids, char *str)
{
    int i, j, len, tmp, offset, prefix, separator;
    char *p;

    /* safety first - scan str for unknown characters */
    for (i = 0, j = strlen(str); i < j; ++i) {
        if (!strchr(sqids->alphabet, str[i])) {
            return 0;
        }
    }

    p = str;
    len = strlen(sqids->alphabet);

    /* extract prefix */
    prefix = *p++;

    /* determine the offset */
    offset = strchr(sqids->alphabet, prefix) - sqids->alphabet;

    /* rearrange alphabet back into its original form */
    char alphabet[len + 1];
    memcpy(alphabet, sqids->alphabet + offset, len - offset);
    memcpy(alphabet + len - offset, sqids->alphabet, offset);
    alphabet[len] = 0;

    /* reverse the internal alphabet */
    for (j = 0; j < len / 2; ++j) {
        tmp = alphabet[j];
        alphabet[j] = alphabet[len - j - 1];
        alphabet[len - j - 1] = tmp;
    }

    /* walk the hash */
    for (i = 0; *p;) {
        separator = alphabet[0];

        /* empty chunk - we're done */
        if (*p == separator) {
            break;
        }

        /* do skip */
        for (; *p && *p != separator; ++p) {}
        ++i;

        /* more numbers - shuffle the alphabet */
        if (*p == separator) {
            sqids_shuffle(alphabet);
            ++p;
        }
    }

    return i;
}

/* }}}                                                                       */

/* vim: set fen fdm=marker fmr={{{,}}} fdl=0 fdc=1 ts=4 sts=4 sw=4 et: */
