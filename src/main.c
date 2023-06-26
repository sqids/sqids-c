#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <errno.h>

#include "sqids.h"

enum {
    COMMAND_ENCODE = 0,
    COMMAND_DECODE = 1,
};

static void
usage(const char *progname, FILE *out)
{
    fputs("\n", out);
    fputs("Usage:\n", out);
    fprintf(out, "  %s [options] <arguments>\n", progname);

    fputs("\n", out);
    fputs("Options:\n", out);
    fputs("  -e, --encode              set command to encode (default)\n",
        out);
    fputs("  -d, --decode              set command to decode\n", out);
    fputs("  -a, --alphabet            set alphabet [" SQIDS_DEFAULT_ALPHABET
        "]\n", out);
    fputs("  -l, --min-length          set hash minimum length [0]\n", out);
    fputs("  -b, --default-blocklist   include a default blocklist "
        "(de,en,es,fr,hi,it,pt,none,all) [all]\n", out);
    fputs("  -w, --block-word          add a word to the blocklist\n", out);
    fputs("  -h, --help                print this message and exit\n", out);
    fputs("  -v, --version             print version information and exit\n",
        out);

    fputs("\n", out);
    exit(out == stderr ? EXIT_FAILURE : EXIT_SUCCESS);
}

static unsigned long long
parse_num(const char *s, char **p)
{
    int radix = 10;

    if (*s == '0') {
        radix = 8;
        ++s;

        if (*s == 'x' || *s == 'X') {
            radix = 16;
            ++s;
        }
    }

    return strtoull(s, p, radix);
}

int
main(int argc, char **argv)
{
    sqids_t *sqids;
    sqids_bl_list_t *blocklist;
    char *alphabet = SQIDS_DEFAULT_ALPHABET, *p, *buf;
    int command = COMMAND_ENCODE, min_len = 0, ch, i, j, num_cnt;

    static const struct option longopts[] = {
        {"encode", no_argument, NULL, 'e'},
        {"decode", no_argument, NULL, 'd'},
        {"alphabet", required_argument, NULL, 'a'},
        {"min-length", required_argument, NULL, 'l'},
        {"default-blocklist", required_argument, NULL, 'b'},
        {"block-word", required_argument, NULL, 'w'},
        {"help", no_argument, NULL, 'h'},
        {"version", no_argument, NULL, 'v'},
        {NULL, 0, NULL, 0}
    };

    /* start with default blocklist */
    if (!(blocklist = sqids_bl_list_all(sqids_bl_match_func))) {
        fprintf(stderr, "Could not allocate default blocklist: %s\n",
            strerror(errno));
        return EXIT_FAILURE;
    }

    /* parse command line options */
    while ((ch = getopt_long(argc, argv, "+eda:l:b:w:hv", longopts,
        NULL)) != -1) {
        switch (ch) {
            case 'e':
                command = COMMAND_ENCODE;
                break;
            case 'd':
                command = COMMAND_DECODE;
                break;
            case 'a':
                alphabet = optarg;
                break;
            case 'l':
                min_len = strtoul(optarg, &p, 10);
                if (p == optarg) {
                    fprintf(stderr, "Invalid minimum length: %s\n", optarg);
                    return EXIT_FAILURE;
                }
                break;
            case 'b':
                if (strcmp(optarg, "de") == 0) {
                    sqids_bl_list_free(blocklist);
                    blocklist = sqids_bl_list_de(sqids_bl_match_func);
                } else if (strcmp(optarg, "en") == 0) {
                    sqids_bl_list_free(blocklist);
                    blocklist = sqids_bl_list_en(sqids_bl_match_func);
                } else if (strcmp(optarg, "es") == 0) {
                    sqids_bl_list_free(blocklist);
                    blocklist = sqids_bl_list_es(sqids_bl_match_func);
                } else if (strcmp(optarg, "fr") == 0) {
                    sqids_bl_list_free(blocklist);
                    blocklist = sqids_bl_list_fr(sqids_bl_match_func);
                } else if (strcmp(optarg, "hi") == 0) {
                    sqids_bl_list_free(blocklist);
                    blocklist = sqids_bl_list_hi(sqids_bl_match_func);
                } else if (strcmp(optarg, "it") == 0) {
                    sqids_bl_list_free(blocklist);
                    blocklist = sqids_bl_list_it(sqids_bl_match_func);
                } else if (strcmp(optarg, "pt") == 0) {
                    sqids_bl_list_free(blocklist);
                    blocklist = sqids_bl_list_pt(sqids_bl_match_func);
                } else if (strcmp(optarg, "none") == 0) {
                    sqids_bl_list_free(blocklist);
                    blocklist = sqids_bl_list_new(sqids_bl_match_func);
                } else if (strcmp(optarg, "all") == 0) {
                    sqids_bl_list_free(blocklist);
                    blocklist = sqids_bl_list_all(sqids_bl_match_func);
                } else {
                    fprintf(stderr, "Unknown blocklist value: %s\n", optarg);
                    return EXIT_FAILURE;
                }

                if (!blocklist) {
                    fprintf(stderr, "Could not allocate user blocklist: %s\n",
                        strerror(errno));
                    return EXIT_FAILURE;
                }

                break;
            case 'w':
                if (!sqids_bl_add_tail(blocklist, optarg)) {
                    fprintf(stderr, "Could not add word to blocklist: %s\n",
                        strerror(errno));
                    if (blocklist) {
                        sqids_bl_list_free(blocklist);
                    }
                    return EXIT_FAILURE;
                }
                break;
            case 'h':
                usage(argv[0], stdout);
                break;
            case 'v':
                fputs(SQIDS_VERSION_STRING, stdout);
                return EXIT_SUCCESS;
            default:
                usage(argv[0], stderr);
        }
    }

    /* no arguments? */
    if (optind == argc) {
        usage(argv[0], stderr);
    }

    /* initialize sqids */
    if (!(sqids = sqids_new(alphabet, min_len, blocklist))) {
        fprintf(stderr, "Could not allocate sqids: %s\n", strerror(errno));
        sqids_bl_list_free(blocklist);
        return EXIT_FAILURE;
    }

    if (command == COMMAND_ENCODE) {
        /* encode */
        num_cnt = argc - optind;
        unsigned long long nums[num_cnt];

        /* collect input */
        for (i = 0; i < num_cnt; ++i) {
            nums[i] = parse_num(argv[optind + i], &p);
            if (p == argv[optind + i]) {
                fprintf(stderr, "Could not parse number: %s\n", argv[i]);
                sqids_free(sqids);
                return EXIT_FAILURE;
            }
        }

        /* encode */
        if (!(buf = sqids_encode(sqids, num_cnt, nums))) {
            fprintf(stderr, "Could not allocate encoding buffer: %s\n",
                strerror(errno));
            sqids_free(sqids);
            return EXIT_FAILURE;
        }

        /* print & go home */
        printf("%s\n", buf);
        sqids_mem_free(buf);
    } else if (command == COMMAND_DECODE) {
        /* decode */
        for (i = optind; i < argc; ++i) {
            if (!(num_cnt = sqids_num_cnt(sqids, argv[i]))) {
                fprintf(stderr, "Could not parse hash: %s\n", argv[i]);
                sqids_free(sqids);
                return EXIT_FAILURE;
            }

            unsigned long long nums[num_cnt];
            num_cnt = sqids_decode(sqids, argv[i], nums, num_cnt);
            for (j = 0; j < num_cnt; ++j) {
                printf("%llu", nums[j]);
                if (j < num_cnt - 1) {
                    fputs(" ", stdout);
                }
            }

            fputs("\n", stdout);
        }
    } else {
        /* !@#$? */
    }

    sqids_free(sqids);

    return EXIT_SUCCESS;
}
