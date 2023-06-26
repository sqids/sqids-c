#ifndef SQIDS_H
#define SQIDS_H 1

/*****************************************************************************/
/* {{{ version information                                                   */
/*****************************************************************************/

#define SQIDS_VERSION_STRING "0.1.0"
#define SQIDS_VERSION_MAJOR 0
#define SQIDS_VERSION_MINOR 1
#define SQIDS_VERSION_PATCH 0

/* }}}                                                                       */

/*****************************************************************************/
/* {{{ memory stuff                                                          */
/*****************************************************************************/

extern void *(*sqids_mem_alloc)(unsigned int);
extern void *(*sqids_mem_realloc)(void *, unsigned int);
extern void (*sqids_mem_free)(void *);
extern char *(*sqids_mem_strdup)(char *);

/* }}}                                                                       */

/*****************************************************************************/
/* {{{ string stuff                                                          */
/*****************************************************************************/

/**
 * dynamic string
 */
struct sqids_str_s {
    char *s;
    int len;
    int siz;
};
typedef struct sqids_str_s sqids_str_t;

/**
 * dynamic string chunk size
 */
extern int sqids_str_chunk;

/**
 * allocate a new dynamic string
 */
sqids_str_t *
sqids_str_new();

/**
 * free a dynamic string
 */
void
sqids_str_free(sqids_str_t *);

/**
 * grow a dynamic string to specific size
 */
int
sqids_str_grow(sqids_str_t *, int);

/**
 * ensure a dynamic string can hold a certain size, null-terminator included
 */
int
sqids_str_cap(sqids_str_t *, int);


/**
 * append a char to a dynamic string
 */
int
sqids_str_append_char(sqids_str_t *, int);

/**
 * append a string to a dynamic string
 */
int
sqids_str_append_str(sqids_str_t *str, char *p);

/* }}}                                                                       */

/*****************************************************************************/
/* {{{ blocklist stuff                                                       */
/*****************************************************************************/

/**
 * blocklist node
 */
struct sqids_bl_node_s {
    char *s;
    struct sqids_bl_node_s *prev;
    struct sqids_bl_node_s *next;
};
typedef struct sqids_bl_node_s sqids_bl_node_t;

/**
 * blocklist structure
 */
struct sqids_bl_list_s {
    sqids_bl_node_t *head;
    sqids_bl_node_t *tail;
    int (*match_func)(char *, char *);
    unsigned int len;
};
typedef struct sqids_bl_list_s sqids_bl_list_t;

/**
 * allocate a new blocklist
 */
sqids_bl_list_t *
sqids_bl_list_new(int (*)(char *, char *));

/**
 * free a list and all its data
 */
void
sqids_bl_list_free(sqids_bl_list_t *);

/**
 * default blocklist for `de`
 * if the library was built with --disable-default-blocklist, this will return an empty list
 */
sqids_bl_list_t *
sqids_bl_list_de(int (*)(char *, char *));

/**
 * default blocklist for `en`
 * if the library was built with --disable-default-blocklist, this will return an empty list
 */
sqids_bl_list_t *
sqids_bl_list_en(int (*)(char *, char *));

/**
 * default blocklist for `es`
 * if the library was built with --disable-default-blocklist, this will return an empty list
 */
sqids_bl_list_t *
sqids_bl_list_es(int (*)(char *, char *));

/**
 * default blocklist for `fr`
 * if the library was built with --disable-default-blocklist, this will return an empty list
 */
sqids_bl_list_t *
sqids_bl_list_fr(int (*)(char *, char *));

/**
 * default blocklist for `hi`
 * if the library was built with --disable-default-blocklist, this will return an empty list
 */
sqids_bl_list_t *
sqids_bl_list_hi(int (*)(char *, char *));

/**
 * default blocklist for `it`
 * if the library was built with --disable-default-blocklist, this will return an empty list
 */
sqids_bl_list_t *
sqids_bl_list_it(int (*)(char *, char *));

/**
 * default blocklist for `pt`
 * if the library was built with --disable-default-blocklist, this will return an empty list
 */
sqids_bl_list_t *
sqids_bl_list_pt(int (*)(char *, char *));

/**
 * default combined blocklist (all contrib languages)
 * if the library was built with --disable-default-blocklist, this will return an empty list
 */
sqids_bl_list_t *
sqids_bl_list_all(int (*)(char *, char *));

/**
 * iterate over a blocklist
 */
#define sqids_bl_foreach(head, pos) for (pos = (head); pos != NULL; pos = pos->next)

/**
 * iterate over a blocklist with safety checks
 */
#define sqids_bl_foreach_safe(head, pos, next) for (pos = (head), next = pos ? pos->next : NULL; pos != NULL; pos = next, next = pos ? pos->next : NULL)

/**
 * add a string at the end of the list
 */
sqids_bl_node_t *
sqids_bl_add_tail(sqids_bl_list_t *, char *);

/**
 * add a string at the beginning of the list
 */
sqids_bl_node_t *
sqids_bl_add_head(sqids_bl_list_t *, char *);

/**
 * search for a string in the list
 */
sqids_bl_node_t *
sqids_bl_find(sqids_bl_list_t *, char *);

/**
 * default list search func
 */
int
sqids_bl_match_func(char *, char *);

/* }}}                                                                       */

/*****************************************************************************/
/* {{{ sqids stuff                                                           */
/*****************************************************************************/

/**
 * default alphabet
 */
#define SQIDS_DEFAULT_ALPHABET "abcdefghijklmnopqrstuvwxyz" \
                               "ABCDEFGHIJKLMNOPQRSTUVWXYZ" \
                               "0123456789"

/**
 * the sqids structure
 */
struct sqids_s {
    char *alphabet;
    unsigned int min_len;
    sqids_bl_list_t *blocklist;
};
typedef struct sqids_s sqids_t;

/**
 * allocate a new sqids structure
 */
sqids_t *
sqids_new(char *, int, sqids_bl_list_t *);

/**
 * free a sqids structure
 */
void
sqids_free(sqids_t *);

/**
 * consistent shuffle
 */
void
sqids_shuffle(char *);

/**
 * encode
 */
char *
sqids_encode(sqids_t *sqids, unsigned int num_cnt, unsigned long long *nums);

/**
 * variadic encode
 */
char *
sqids_vencode(sqids_t *sqids, unsigned int num_cnt, ...);

/**
 * decode
 */
int
sqids_decode(sqids_t *, char *, unsigned long long *, unsigned int);

/**
 * numbers count
 */
int
sqids_num_cnt(sqids_t *, char *);

/* }}}                                                                       */

#endif /* !defined(SQIDS_H) */

/* vim: set fen fdm=marker fmr={{{,}}} fdl=0 fdc=1 ts=4 sts=4 sw=4 et: */
