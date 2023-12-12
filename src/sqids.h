#ifndef SQIDS_H
#define SQIDS_H 1

/*****************************************************************************/
/* {{{ version information                                                   */
/*****************************************************************************/

#define SQIDS_VERSION_STRING "0.2.1"
#define SQIDS_VERSION_MAJOR 0
#define SQIDS_VERSION_MINOR 2
#define SQIDS_VERSION_PATCH 1

/* }}}                                                                       */

/*****************************************************************************/
/* {{{ error handling stuff                                                  */
/*****************************************************************************/

#define SQIDS_ERR_ALLOC         0x01
#define SQIDS_ERR_ALPHABET      0x02
#define SQIDS_ERR_MAX_RETRIES   0x03
#define SQIDS_ERR_INVALID       0x04
#define SQIDS_ERR_OVERFLOW      0x05

extern int *__sqids_errno_addr(void);
#define sqids_errno (*__sqids_errno_addr())

/* }}}                                                                       */

/*****************************************************************************/
/* {{{ memory stuff                                                          */
/*****************************************************************************/

extern void *(*sqids_mem_alloc)(unsigned int);
extern void (*sqids_mem_free)(void *);

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
struct sqids_bl_s {
    sqids_bl_node_t *head;
    sqids_bl_node_t *tail;
    int (*match_func)(char *, char *);
};
typedef struct sqids_bl_s sqids_bl_t;

/**
 * allocate a new blocklist
 */
sqids_bl_t *
sqids_bl_new(int (*)(char *, char *));

/**
 * free a list and all its data
 */
void
sqids_bl_free(sqids_bl_t *);

/**
 * default blocklist for `de`
 * if the library was built with --disable-default-blocklist, this will return an empty list
 */
sqids_bl_t *
sqids_bl_list_de(int (*)(char *, char *));

/**
 * default blocklist for `en`
 * if the library was built with --disable-default-blocklist, this will return an empty list
 */
sqids_bl_t *
sqids_bl_list_en(int (*)(char *, char *));

/**
 * default blocklist for `es`
 * if the library was built with --disable-default-blocklist, this will return an empty list
 */
sqids_bl_t *
sqids_bl_list_es(int (*)(char *, char *));

/**
 * default blocklist for `fr`
 * if the library was built with --disable-default-blocklist, this will return an empty list
 */
sqids_bl_t *
sqids_bl_list_fr(int (*)(char *, char *));

/**
 * default blocklist for `hi`
 * if the library was built with --disable-default-blocklist, this will return an empty list
 */
sqids_bl_t *
sqids_bl_list_hi(int (*)(char *, char *));

/**
 * default blocklist for `it`
 * if the library was built with --disable-default-blocklist, this will return an empty list
 */
sqids_bl_t *
sqids_bl_list_it(int (*)(char *, char *));

/**
 * default blocklist for `pt`
 * if the library was built with --disable-default-blocklist, this will return an empty list
 */
sqids_bl_t *
sqids_bl_list_pt(int (*)(char *, char *));

/**
 * default combined blocklist (all contrib languages)
 * if the library was built with --disable-default-blocklist, this will return an empty list
 */
sqids_bl_t *
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
sqids_bl_add_tail(sqids_bl_t *, char *);

/**
 * add a string at the beginning of the list
 */
sqids_bl_node_t *
sqids_bl_add_head(sqids_bl_t *, char *);

/**
 * search for a string in the list
 */
sqids_bl_node_t *
sqids_bl_find(sqids_bl_t *, char *);

/**
 * default list match function
 */
int
sqids_bl_match(char *, char *);

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
    sqids_bl_t *blocklist;
};
typedef struct sqids_s sqids_t;

/**
 * allocate a new sqids structure
 */
sqids_t *
sqids_new(char *, unsigned int, sqids_bl_t *);

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
 * numbers count
 */
int
sqids_num_cnt(sqids_t *, char *);

/**
 * decode
 */
int
sqids_decode(sqids_t *, char *, unsigned long long *, unsigned int);

/* }}}                                                                       */

#endif /* !defined(SQIDS_H) */

/* vim: set fen fdm=marker fmr={{{,}}} fdl=0 fdc=1 ts=4 sts=4 sw=4 et: */
