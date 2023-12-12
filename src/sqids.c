#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>

#include "sqids.h"

/*****************************************************************************/
/* {{{ error handling stuff                                                  */
/*****************************************************************************/

TLS int __sqids_errno_val;
int *
__sqids_errno_addr()
{
    return &__sqids_errno_val;
}

/* }}}                                                                       */

/*****************************************************************************/
/* {{{ memory stuff                                                          */
/*****************************************************************************/

/* default alloc() implementation */
static inline void *
sqids_mem_alloc_default(unsigned int siz)
{
    return malloc(siz);
}

/* default free() implementation */
static inline void
sqids_mem_free_default(void *ptr)
{
    free(ptr);
}

void *(*sqids_mem_alloc)(unsigned int) = sqids_mem_alloc_default;
void (*sqids_mem_free)(void *) = sqids_mem_free_default;

/* }}}                                                                       */

/*****************************************************************************/
/* {{{ blocklist stuff                                                       */
/*****************************************************************************/

/* allocate a new list */
sqids_bl_t *
sqids_bl_new(int (*match_func)(char *, char *))
{
    sqids_bl_t *result;

    if (!(result = sqids_mem_alloc(sizeof(sqids_bl_t)))) {
        sqids_errno = SQIDS_ERR_ALLOC;
        return NULL;
    }

    if (!(result->match_func = match_func)) {
        result->match_func = sqids_bl_match;
    }

    result->head = NULL;
    result->tail = NULL;

    return result;
}

/* free a list and all its data */
void
sqids_bl_free(sqids_bl_t *bl)
{
    sqids_bl_node_t *iter, *next;

    sqids_bl_foreach_safe(bl->head, iter, next) {
        if (iter->s) {
            sqids_mem_free(iter->s);
        }

        sqids_mem_free(iter);
    }

    sqids_mem_free(bl);
}

#if !defined(SQIDS_DEFAULT_BLOCKLIST) || SQIDS_DEFAULT_BLOCKLIST == 0
/* empty `de` blocklist when compiled with --disable-default-blocklist */
sqids_bl_list_t *
sqids_bl_list_de(int (*match_func)(char *, char *))
{
    return sqids_bl_new(match_func);
}

/* empty `en` blocklist when compiled with --disable-default-blocklist */
sqids_bl_list_t *
sqids_bl_list_en(int (*match_func)(char *, char *))
{
    return sqids_bl_new(match_func);
}

/* empty es` blocklist when compiled with --disable-default-blocklist */
sqids_bl_list_t *
sqids_bl_list_es(int (*match_func)(char *, char *))
{
    return sqids_bl_new(match_func);
}

/* empty `fr` blocklist when compiled with --disable-default-blocklist */
sqids_bl_list_t *
sqids_bl_list_fr(int (*match_func)(char *, char *))
{
    return sqids_bl_new(match_func);
}

/* empty `hi` blocklist when compiled with --disable-default-blocklist */
sqids_bl_list_t *
sqids_bl_list_hi(int (*match_func)(char *, char *))
{
    return sqids_bl_new(match_func);
}

/* empty `it` blocklist when compiled with --disable-default-blocklist */
sqids_bl_list_t *
sqids_bl_list_it(int (*match_func)(char *, char *))
{
    return sqids_bl_new(match_func);
}

/* empty `pt` blocklist when compiled with --disable-default-blocklist */
sqids_bl_list_t *
sqids_bl_list_pt(int (*match_func)(char *, char *))
{
    return sqids_bl_new(match_func);
}

/* empty combined blocklist when compiled with --disable-default-blocklist */
sqids_bl_list_t *
sqids_bl_list_all(int (*match_func)(char *, char *))
{
    return sqids_bl_new(match_func);
}
#endif

/* add a string at the end of the list */
sqids_bl_node_t *
sqids_bl_add_tail(sqids_bl_t *bl, char *s)
{
    sqids_bl_node_t *node;
    int len;

    if (!(node = sqids_mem_alloc(sizeof(sqids_bl_node_t)))) {
        sqids_errno = SQIDS_ERR_ALLOC;
        return NULL;
    }

    len = strlen(s);
    if (!(node->s = sqids_mem_alloc(len + 1))) {
        sqids_mem_free(node);
        sqids_errno = SQIDS_ERR_ALLOC;
        return NULL;
    }

    memcpy(node->s, s, len + 1);
    node->prev = bl->tail;
    node->next = NULL;

    /* assumption: if list->tail != NULL, list->head != NULL */
    if (bl->tail != NULL) {
        bl->tail->next = node;
    } else {
        bl->head = node;
    }

    bl->tail = node;

    return node;
}

/* add a string at the beginning of the list */
sqids_bl_node_t *
sqids_bl_add_head(sqids_bl_t *bl, char *s)
{
    sqids_bl_node_t *node;
    int len;

    if (!(node = sqids_mem_alloc(sizeof(sqids_bl_node_t)))) {
        sqids_errno = SQIDS_ERR_ALLOC;
        return NULL;
    }

    len = strlen(s);
    if (!(node->s = sqids_mem_alloc(len + 1 + 1))) {
        sqids_mem_free(node);
        sqids_errno = SQIDS_ERR_ALLOC;
        return NULL;
    }

    memcpy(node->s, s, len);
    node->prev = NULL;
    node->next = bl->head;

    /* assumption: if list->head != NULL, list->tail != NULL */
    if (bl->head != NULL) {
        bl->head->prev = node;
    } else {
        bl->tail = node;
    }

    bl->head = node;

    return node;
}

/* search for a string in the list */
sqids_bl_node_t *
sqids_bl_find(sqids_bl_t *bl, char *s)
{
    sqids_bl_node_t *iter;
    sqids_bl_foreach(bl->head, iter) {
        if (bl->match_func(s, iter->s)) {
            return iter;
        }
    }

    return NULL;
}

/* default list search func */
int
sqids_bl_match(char *s, char *bad_word)
{
    int slen = strlen(s), blen = strlen(bad_word);

    if (slen < blen) {
        return 0;
    }

    if (slen <= 3 || blen <= 3) {
        return strcasecmp(s, bad_word) == 0;
    }

    if (strpbrk(bad_word, "0123456789")) {
        return strncasecmp(s, bad_word, blen) == 0 ||
            strncasecmp(s + slen - blen, bad_word, blen) == 0;
    }

    for (; *s; s++) {
        if (strncasecmp(s, bad_word, blen) == 0) {
            return 1;
        }
    }

    return 0;
}

/* }}}                                                                       */

/*****************************************************************************/
/* {{{ sqids stuff                                                           */
/*****************************************************************************/

/* allocate a new sqids structure */
sqids_t *
sqids_new(char *alphabet, unsigned int min_len, sqids_bl_t *blocklist)
{
    sqids_t *result;
    int len;

    if (!(result = sqids_mem_alloc(sizeof(sqids_t)))) {
        sqids_errno = SQIDS_ERR_ALLOC;
        return NULL;
    }

    if (!alphabet) {
        alphabet = SQIDS_DEFAULT_ALPHABET;
    }

    len = strlen(alphabet);
    if (len < 3) {
        sqids_mem_free(result);
        sqids_errno = SQIDS_ERR_ALPHABET;
        return NULL;
    }

    if (!(result->alphabet = sqids_mem_alloc(len + 1))) {
        sqids_mem_free(result);
        sqids_errno = SQIDS_ERR_ALLOC;
        return NULL;
    }

    memcpy(result->alphabet, alphabet, len + 1);
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
        sqids_bl_free(sqids->blocklist);
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
sqids_encode_internal(sqids_t *sqids, char *s, unsigned int num_cnt,
    unsigned long long *nums, int increment)
{
    unsigned long long num;
    int i, j, len, tmp, offset, prefix;
    char *p, *pb;

    /* sanity check */
    len = strlen(sqids->alphabet);
    if (increment > len) {
        sqids_errno = SQIDS_ERR_MAX_RETRIES;
        return 1;
    }

    /* get a semi-random offset from input numbers */
    for (i = 0, offset = num_cnt; i < num_cnt; ++i) {
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
    p = s;
    *p++ = prefix;

    /* iterate over numbers and encode each */
    for (i = 0; i < num_cnt; ++i) {
        /* save current string pointer so we can easily reverse the number */
        pb = p;

        /* encode the number in reverse */
        num = nums[i];
        do {
            *p++ = alphabet[num % (len - 1) + 1];
            num /= (len - 1);
        } while (num > 0);

        /* reverse it so we can decode it in the future */
        for (j = 0; j < (p - pb) / 2; ++j) {
            tmp = *(pb + j);
            *(pb + j) = *(p - 1 - j);
            *(p - 1 - j) = tmp;
        }

        /* more numbers to encode - append a separator, shuffle the alphabet */
        if (i < num_cnt - 1) {
            /* the separator is the first character in the current alphabet */
            *p++ = alphabet[0];
            sqids_shuffle(alphabet);
        }
    }

    /* ensure a terminator */
    *p = 0;

    /* handle min_len */
    if (p - s < sqids->min_len) {
        /* append the last separator */
        *p++ = alphabet[0];

        /* keep appending separators and alphabet until we're done */
        while (p - s < sqids->min_len) {
            sqids_shuffle(alphabet);

            /* the alphabet has enough material to feed the final id,
               we can safely terminate it */
            if (len - 1 > sqids->min_len - (p - s)) {
                alphabet[sqids->min_len - (p - s)] = 0;
            }

            /* append the alphabet */
            for (pb = alphabet; *pb; ++pb) {
                *p++ = *pb;
            }
        }
    }

    /* terminate the buffer */
    *p = 0;

    /* handle bad words */
    if (sqids->blocklist && sqids_bl_find(sqids->blocklist, s)) {
        return sqids_encode_internal(sqids, s, num_cnt, nums, increment + 1);
    }

    return 0;
}

/* estimate encoded buffer needs */
static inline int
sqids_estimate(sqids_t *sqids, unsigned int num_cnt, unsigned long long *nums)
{
    int i, result;
    double log2len = log2(strlen(sqids->alphabet) - 1);

    for (i = 0, result = 0; i < num_cnt; ++i) {
        switch (nums[i]) {
            case 0:
                result += 2;
                break;
            case 0xFFFFFFFFFFFFFFFFull:
                result += ceil(log2(nums[i]) / log2len) + 1;
                break;
            default:
                result += ceil(log2(nums[i] + 1) / log2len) + 1;
        }
    }

    return (result > sqids->min_len ? result : sqids->min_len) + 1;
}

/* encode */
char *
sqids_encode(sqids_t *sqids, unsigned int num_cnt, unsigned long long *nums)
{
    char *result;

    /* allocate string buffer */
    if (!(result = sqids_mem_alloc(sqids_estimate(sqids, num_cnt, nums)))) {
        sqids_errno = SQIDS_ERR_ALLOC;
        return NULL;
    }

    /* encode */
    if (sqids_encode_internal(sqids, result, num_cnt, nums, 0) != 0) {
        sqids_mem_free(result);
        return NULL;
    }

    return result;
}

/* variadic encode */
char *
sqids_vencode(sqids_t *sqids, unsigned int num_cnt, ...)
{
    char *result;
    unsigned long long nums[num_cnt];
    int i;
    va_list ap;

    /* collect args */
    va_start(ap, num_cnt);
    for (i = 0; i < num_cnt; ++i) {
        nums[i] = va_arg(ap, unsigned long long);
    }
    va_end(ap);

    /* allocate string buffer */
    if (!(result = sqids_mem_alloc(sqids_estimate(sqids, num_cnt, nums)))) {
        sqids_errno = SQIDS_ERR_ALLOC;
        return NULL;
    }

    /* encode */
    if (sqids_encode_internal(sqids, result, num_cnt, nums, 0) != 0) {
        sqids_mem_free(result);
        return NULL;
    }

    return result;
}

/* decode number count */
int
sqids_num_cnt(sqids_t *sqids, char *s)
{
    int i, j, len, tmp, offset, prefix, separator;
    char *p;

    /* safety first - scan str for unknown characters */
    for (i = 0, j = strlen(s); i < j; ++i) {
        if (!strchr(sqids->alphabet, s[i])) {
            sqids_errno = SQIDS_ERR_INVALID;
            return 0;
        }
    }

    /* empty string - no numbers (technically not an error) */
    if (!j) {
        return 0;
    }

    p = s;
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

/* decode */
int
sqids_decode(sqids_t *sqids, char *s, unsigned long long *nums,
    unsigned int num_max)
{
    unsigned long long num, prev;
    int i, j, len, tmp, offset, prefix, separator;
    char *p;

    /* safety first - scan str for unknown characters */
    for (i = 0, j = strlen(s); i < j; ++i) {
        if (!strchr(sqids->alphabet, s[i])) {
            sqids_errno = SQIDS_ERR_INVALID;
            return -1;
        }
    }

    /* empty string - nothing to decode (technically not an error) */
    if (!j) {
        return 0;
    }

    p = s;
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
            prev = num;

            num *= len - 1;
            num += strchr(alphabet + 1, *p) - alphabet - 1;

            /* overflow protection */
            if (num < prev) {
                sqids_errno = SQIDS_ERR_OVERFLOW;
                return -1;
            }
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

/* }}}                                                                       */

/* vim: set fen fdm=marker fmr={{{,}}} fdl=0 fdc=1 ts=4 sts=4 sw=4 et: */
