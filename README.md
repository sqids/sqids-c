# [Sqids C](https://sqids.org/c)

Sqids (pronounced "squids") is a small library that lets you generate YouTube-looking IDs from numbers. It's good for link shortening, fast & URL-safe ID generation and decoding back into numbers for quicker database lookups.

## Getting started

The library is built using GNU Autotools.

``` bash
./bootstrap
./configure
make
```

## Blocklists

The library includes several [blocklists](https://github.com/sqids/sqids-blocklist) that are enabled and built by default.

You have the option to build it without them by passing `--disable-default-blocklist` to `configure`.

## Memory management

Unlike [Hashids](https://github.com/tzvetkoff/hashids.c), Sqids relies heavily on dynamic memory allocation.

## API

### `sqids_new`

``` c
sqids_t *
sqids_new(char *alphabet, int min_len, sqids_bl_list_t *blocklist)
```

Sqids structure constructor.

If you pass `NULL` null as the alphabet, `SQIDS_DEFAULT_ALPHABET` will be used.

A `NULL` blocklist will result in no blocklist at all.
See the blocklist API below for further information.

The returned structure should be freed using `sqids_free`.

### `sqids_free`

``` c
/* free a sqids structure */
void
sqids_free(sqids_t *sqids)
```

Sqids structure destructor.

Deallocates the Sqids structure including the alphabet and the blocklist, if present.

### `sqids_shuffle`

``` c
void
sqids_shuffle(char *alphabet)
```

Consistent shuffle.

Chances are you won't be using this anywhere outside Sqids.

### `sqids_encode`

``` c
char *
sqids_encode(sqids_t *sqids, unsigned int num_cnt, unsigned long long *nums)
```

Encode function.

Encodes an array of numbers to a string hash.

The returned string should be freed with `sqids_mem_free` (which defaults to `free`.)

### `sqids_vencode`

``` c
char *
sqids_vencode(sqids_t *sqids, unsigned int num_cnt, ...)
```

Variadic version of `sqids_encode`.

### `sqids_decode`

``` c
int
sqids_decode(sqids_t *sqids, char *str, unsigned long long *nums, unsigned int num_max)
```

Decode function.

Decodes a hash back to numbers while performing limit checking.

### `sqids_num_cnt`

``` c
int
sqids_num_cnt(sqids_t *sqids, char *str)
```

Number counting function.

Returns the count of numbers in an hash.

### `sqids_bl_list_new`

``` c
sqids_bl_list_t *
sqids_bl_list_new(int (*match_func)(char *, char *))
```

Blocklist constructor.

Returns a new empty blocklist, which itself is a doubly linked list.

If you pass `NULL` as `match_func`, it will default to `sqids_bl_match_func`.

THe result should be freed with `sqids_bl_list_free`.

### `sqids_bl_list_free`

``` c
void
sqids_bl_list_free(sqids_bl_list_t *list)
```

Blocklist destructor.

Frees the blocklist and all its data.

### `sqids_bl_add_tail`

``` c
sqids_bl_node_t *
sqids_bl_add_tail(sqids_bl_list_t *list, char *s)
```

Adds a word to the end of the blocklist.

### `sqids_bl_add_head`

``` c
sqids_bl_node_t *
sqids_bl_add_head(sqids_bl_list_t *list, char *s)
```

Adds a word to the beginning of the blocklist.

### `sqids_bl_match_func`

``` c
int
sqids_bl_match_func(char *str, char *bad_word)
```

Tests if a string matches a bad word.

### `sqids_bl_list_all`

``` c
sqids_bl_list_t *
sqids_bl_list_all(int (*match_func)(char *, char *))
```

A default blocklist containing a combination of all the languages below.

### `sqids_bl_list_de`

``` c
sqids_bl_list_t *
sqids_bl_list_de(int (*match_func)(char *, char *))
```

German blocklist.

### `sqids_bl_list_en`

``` c
sqids_bl_list_t *
sqids_bl_list_en(int (*match_func)(char *, char *))
```

English blocklist.

### `sqids_bl_list_es`

``` c
sqids_bl_list_t *
sqids_bl_list_es(int (*match_func)(char *, char *))
```

Spanish blocklist.

### `sqids_bl_list_fr`

``` c
sqids_bl_list_t *
sqids_bl_list_fr(int (*match_func)(char *, char *))
```

French blocklist.

### `sqids_bl_list_hi`

``` c
sqids_bl_list_t *
sqids_bl_list_hi(int (*match_func)(char *, char *))
```

Hindi blocklist.

### `sqids_bl_list_it`

``` c
sqids_bl_list_t *
sqids_bl_list_it(int (*match_func)(char *, char *))
```

Italian blocklist.

### `sqids_bl_list_pt`

``` c
sqids_bl_list_t *
sqids_bl_list_pt(int (*match_func)(char *, char *))
```

Portuguese blocklist.

## CLI

A command-line utility is provided so one can easily encode/decode hashes and experiment with the library.

## Examples

A very minimalistic example:

``` c
#include <sqids.h>

sqids_t *sqids = sqids_new(SQIDS_DEFAULT_ALPHABET, 0, sqids_bl_list_all(NULL));
char *hash;

unsigned long long nums1 = {1, 3, 3, 7};
hash = sqids_encode(sqids, 4, nums1);       // => wFfQJbnS
sqids_decode(sqids, hash, nums1, 4);        // => 4
sqids_mem_free(hash);

sqids_free(sqids);
```

For a more thorough example of the API, take a look at [src/main.c](src/main.c).

## License

[MIT](LICENSE)
