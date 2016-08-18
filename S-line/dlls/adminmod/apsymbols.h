/*#define GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__ )

#if GCC_VERSION > 30302
__asm__(".symver regexec,regexec@GLIBC_2.2.5");
#endif

Original script for AdminMod 2.50 since 2004 - [APG]RoboCop[CL] */

#if defined(__GNUC__) && defined(__LP64__)  /* only under 64 bit gcc */
#include <features.h>       /* for glibc version */
#if defined(__GLIBC__) && (__GLIBC__ == 2) && (__GLIBC_MINOR__ >= 14)
/* force to be from earlier compatible system */
__asm__(".symver regexec,regexec@GLIBC_2.2.5");
#endif
#undef _FEATURES_H      /* so gets reloaded if necessary */
#endif