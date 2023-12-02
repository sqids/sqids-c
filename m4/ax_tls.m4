dnl
dnl SYNOPSIS
dnl
dnl   AX_TLS([action-if-found], [action-if-not-found])
dnl
dnl DESCRIPTION
dnl
dnl   Tests if the compiler supports thread local storage.
dnl   Defines TLS if TLS is supported by the compiler.
dnl   ${ac_cv_tls} can be used in [action-if-found].
dnl

AC_DEFUN([AX_TLS], [
  AC_MSG_CHECKING([for thread local storage (TLS) class])
  AC_CACHE_VAL([ac_cv_tls], [
    for ax_tls_keyword in '_Thread_local' '__thread' '__declspec(thread)' 'none'; do
      AS_CASE([${ax_tls_keyword}], [none], [ac_cv_tls='none'; break], [
        AC_COMPILE_IFELSE([
          AC_LANG_PROGRAM([[
            #include <stdlib.h>

            static void
            foo() {
              static ]] ${ax_tls_keyword} [[ int tlsvar;
              exit(0);
            }
          ]], [])
        ], [
          ac_cv_tls="${ax_tls_keyword}"
          break
        ], [
          use_alloca=no
        ])
      ])
    done
  ])
  AC_MSG_RESULT([${ac_cv_tls}])

  AS_IF([test "${ac_cv_tls}" != "none"], [
    AC_DEFINE_UNQUOTED([TLS], [${ac_cv_tls}], [Compiler TLS class, if supported.])
    m4_ifnblank([$1], [$1])
  ], [
    m4_ifnblank([$2], [$2])
  ])
])
