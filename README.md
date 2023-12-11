# [Sqids C](https://sqids.org/c)

![build](https://github.com/sqids/sqids-c/actions/workflows/build.yml/badge.svg)

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

You can still override the memory management functions if needed, by reassigning `sqids_mem_alloc`/`sqids_mem_free`.

## Error handling

Sqids defines a thread-safe `sqids_errno` with the following possible values:

| Value                   | Description                                                         |
| ----------------------- | ------------------------------------------------------------------- |
| `SQIDS_ERR_ALLOC`       | Memory allocation failed. Generally this means `errno` is `ENOMEM`. |
| `SQIDS_ERR_ALPHABET`    | Alphabet is too short.                                              |
| `SQIDS_ERR_MAX_RETRIES` | Max encoding retries reached.                                       |
| `SQIDS_ERR_INVALID`     | Hash contains invalid characters.                                   |
| `SQIDS_ERR_OVERFLOW`    | Integer overflow.                                                   |

Keep in mind that you should first test the function result and then inspect `sqids_errno` - if a function succeeds, `sqids_errno` is left untouched.

## API

### `sqids_new`

``` c
sqids_t *
sqids_new(char *alphabet, int min_len, sqids_bl_t *blocklist)
```

Sqids structure constructor.

If you pass `NULL` null as the alphabet, `SQIDS_DEFAULT_ALPHABET` will be used.

A `NULL` blocklist will result in no blocklist at all.
See the blocklist API below for further information.

The returned structure should be freed using `sqids_free`.

In case of failure, `NULL` is returned and `sqids_errno` is set accordingly.

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

In case of failure, `NULL` is returned and `sqids_errno` is set accordingly.

### `sqids_vencode`

``` c
char *
sqids_vencode(sqids_t *sqids, unsigned int num_cnt, ...)
```

Variadic version of `sqids_encode`.

### `sqids_num_cnt`

``` c
int
sqids_num_cnt(sqids_t *sqids, char *s)
```

Number counting function.

Returns the count of numbers in an hash.
Keep in mind that this function can return `0` as empty strings are not considered errors.

In case of failure, `-1` is returned and `sqids_errno` is set accordingly.

### `sqids_decode`

``` c
int
sqids_decode(sqids_t *sqids, char *s, unsigned long long *nums, unsigned int num_max)
```

Decode function.

Decodes a hash back to numbers.
Keep in mind that this function can return `0` as empty strings are not considered errors.

Result is the count of numbers decoded in the hash.

In case of failure, `-1` is returned and `sqids_errno` is set accordingly.

### `sqids_bl_new`

``` c
sqids_bl_t *
sqids_bl_new(int (*match_func)(char *, char *))
```

Blocklist constructor.

Returns a new empty blocklist, which itself is a doubly linked list.

If you pass `NULL` as `match_func`, it will default to `sqids_bl_match`.

In case of failure, `NULL` is returned and `sqids_errno` is set accordingly.

### `sqids_bl_free`

``` c
void
sqids_bl_free(sqids_bl_t *bl)
```

Blocklist destructor.

Frees the blocklist and all its data.

### `sqids_bl_add_tail`

``` c
sqids_bl_node_t *
sqids_bl_add_tail(sqids_bl_t *bl, char *s)
```

Adds a word to the end of the blocklist.

Result is a pointer to the new blocklist node.

In case of failure, `NULL` is returned and `sqids_errno` is set accordingly.

### `sqids_bl_add_head`

``` c
sqids_bl_node_t *
sqids_bl_add_head(sqids_bl_t *bl, char *s)
```

Adds a word to the beginning of the blocklist.

Result is a pointer to the new blocklist node.

In case of failure, `NULL` is returned and `sqids_errno` is set accordingly.

### `sqids_bl_find`

``` c
sqids_bl_node_t *
sqids_bl_find(sqids_bl_t *bl, char *s)
```

Tests if a string matches any of the bad words in the blocklist.

Result is pointer to the matching blocklist node, or `NULL` if no match is found.

### `sqids_bl_match`

``` c
int
sqids_bl_match(char *s, char *bad_word)
```

Default blocklist match function.

Tests if a string matches a bad word.

Result is `1` in case of a match, `0` otherwise.

### `sqids_bl_list_*`

``` c
sqids_bl_t *
sqids_bl_list_LANG(int (*match_func)(char *, char *))
```

Will either return the combined blocklist of a language-specific one.
If you pass `NULL` as `match_func`, `sqids_bl_match` will be used as default.

In case of failure, `NULL` is returned and `sqids_errno` is set accordingly.

| Function            | Description                                                                |
| ------------------- | -------------------------------------------------------------------------- |
| `sqids_bl_list_all` | The default blocklist, contains all language-specific blocklists combined. |
| `sqids_bl_list_de`  | German blocklist.                                                          |
| `sqids_bl_list_en`  | English blocklist.                                                         |
| `sqids_bl_list_es`  | Spanish blocklist.                                                         |
| `sqids_bl_list_fr`  | French blocklist.                                                          |
| `sqids_bl_list_hi`  | Hindi blocklist.                                                           |
| `sqids_bl_list_it`  | Italian blocklist.                                                         |
| `sqids_bl_list_pt`  | Portuguese blocklist.                                                      |

## CLI

A command-line utility is provided so one can easily encode/decode hashes and experiment with the library.

## Examples

Simple encode & decode:

``` c
#include <sqids.h>

sqids_t *sqids = sqids_new(SQIDS_DEFAULT_ALPHABET, 0, sqids_bl_list_all(NULL));

unsigned long long nums[] = {1, 2, 3};
char *hash = sqids_encode(sqids, 3, nums);  // => "86Rf07"
sqids_decode(sqids, hash, nums, 3);         // => 3

sqids_mem_free(hash);
sqids_free(sqids);
```

Enforce a minimum length:

``` c
#include <sqids.h>

sqids_t *sqids = sqids_new(SQIDS_DEFAULT_ALPHABET, 10, sqids_bl_list_all(NULL));

unsigned long long nums[] = {1, 2, 3};
char *hash = sqids_encode(sqids, 3, nums);  // => "86Rf07xd4z"
sqids_decode(sqids, hash, nums, 3);         // => 3

sqids_mem_free(hash);
sqids_free(sqids);
```

Using a custom alphabet will produce different results:

``` c
#include <sqids.h>

sqids_t *sqids = sqids_new("FxnXM1kBN6cuhsAvjW3Co7l2RePyY8DwaU04Tzt9fHQrqSVKdpimLGIJOgb5ZE", 0, sqids_bl_list_all(NULL));

unsigned long long nums[] = {1, 2, 3};
char *hash = sqids_encode(sqids, 3, nums);  // => "B4aajs"
sqids_decode(sqids, hash, nums, 3);         // => 3

sqids_mem_free(hash);
sqids_free(sqids);
```

Custom blocklist is just as simple:

``` c
#include <sqids.h>

sqids_bl_t *bl = sqids_bl_new(NULL);
sqids_bl_add_tail(bl, "86Rf07");
sqids_t *sqids = sqids_new(SQIDS_DEFAULT_ALPHABET, 0, bl);

unsigned long long nums[] = {1, 2, 3};
char *hash = sqids_encode(sqids, 3, nums);  // => "se8ojk"
sqids_decode(sqids, hash, nums, 3);         // => 3

sqids_mem_free(hash);
sqids_free(sqids);
```

For a more thorough example of the API, take a look at [src/main.c](src/main.c).

## License

[MIT](LICENSE)
