#ifndef __GNUREGEX_H__
#define __GNUREGEX_H__

#ifdef __cplusplus
# ifndef __STDC__
#  define RE_KLUDGE_STDC
#  define __STDC__ 1
# endif
extern "C" {
#endif

#include "regex.h"

extern void gnuregex_free(void *);

#ifdef __cplusplus
}
# ifdef RE_KLUDGE_STDC
#  undef __STDC__
#  undef RE_KLUDGE_STDC
# endif
#endif	/* C++ */

#endif //__GNUREGEX_H__
