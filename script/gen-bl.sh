#!/usr/bin/env bash
set -e

# config
languages=(de en es fr hi it pt blocklist)

# fs
root="$(cd "$(dirname "$(readlink -f "${BASH_SOURCE[0]}")")/.." && pwd)"
src="${root}/src"
tmp="${root}/tmp"
date="$(date +%c)"

# fetch
for language in "${languages[@]}"; do
  if [[ ! -f "${tmp}/${language}.json" || "${1}" = '--force' || "${1}" = '-f' ]]; then
    echo "Fetching ${language}.json" >&2
    curl -sSL "https://raw.githubusercontent.com/sqids/sqids-blocklist/main/output/${language}.json" >"${tmp}/${language}.json"
  fi
done

# head
code="/* generated: ${date} */"'

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stddef.h>

#include "sqids.h"

#ifdef SQIDS_DEFAULT_BLOCKLIST

#define SQIDS_BL_ADD(bl, s) \
    if (!sqids_bl_add_tail(bl, s)) { sqids_bl_list_free(bl); return NULL; }
'

# generate
for language in "${languages[@]}"; do
  if [[ "${language}" = 'blocklist' ]]; then
    name='all'
  else
    name="${language}"
  fi

  echo "Generating sqids_bl_list_${name} from ${language}.json"

  # shellcheck disable=SC2016
  code="${code}
sqids_bl_list_t *
sqids_bl_list_${name}(int (*match_func)(char *, char *))
{
    sqids_bl_list_t *result;

    if (!(result = sqids_bl_list_new(match_func))) {
        return NULL;
    }
"

  while read -r word; do
    word="${word//\"/\\\"}"
    word="${word//\\/\\\\}"
    line=$'\n'"    SQIDS_BL_ADD(result, \"${word}\");"
    code="${code}${line}"
  done < <(jq -r '.[]' "${tmp}/${language}.json")

  code="${code}

    return result;
}
"

done

# tail
code="${code}
#undef SQIDS_BL_ADD

#endif

/* vim: set fen fdm=marker fmr={{{,}}} fdl=0 fdc=1 ts=4 sts=4 sw=4 et: */"

# write
echo "${code}" >"${src}/bl.c"

# vim:ts=2:sts=2:sw=2:et
