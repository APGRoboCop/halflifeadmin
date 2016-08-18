/*
 * Things needed to compile under linux.
 *
 * Should be reworked totally to use GNU's 'configure'
 */

/*
 * Getchar is not a 'cool' replacement for MSDOS getch: Linux/unix depends on the features activated or not about the
 * controlling terminal's tty. This means that ioctl(2) calls must be performed, for instance to have the controlling terminal tty's
 * in 'raw' mode, if we want to be able to fetch a single character. This also means that everything must be put back
 * correctly when the program ends.
 *
 * For interactive use of SRUN/SDBG if would be much better to use GNU's readline package: the user would be able to have
 * a complete emacs/vi like line editing system.
 *
 * So we stick to getchar at the moment... (one needs to key ctrl-d to terminate input if getch is called with a controlling
 * terminal driven by a tty having -raw)
 */
#define getch				getchar
#define	stricmp(a,b)	strcasecmp(a,b)

/*
 * WinWorld wants '\'. Unices do not.
 */
#define	DIRECTORY_SEP_CHAR	'/'
#define	DIRECTORY_SEP_STR		"/"

/*
 * Sc considers that a computer is Big Enddian if BIG_ENDIAN is defined.
 * Instead, Linux (well glibc) gives the order of bytes '4321' (see endian.h) for
 * BIG_ENDIAN, LITTLE_ENDIAN, and even PDP_ENDIAN.
 *
 * So we must define BIG_ENDIAN for SC only if the compiler is really
 * Big Endian. __BYTE_ORDER is designed for that.
 */
#undef	BIG_ENDIAN
#ifndef	__BYTE_ORDER
#include <stdlib.h>
#endif
#ifndef	__BYTE_ORDER
# error	"Can't figure computer byte order (__BYTE_ORDER macro not found)"
#endif

#if  __BYTE_ORDER ==  __BIG_ENDIAN
#define	BIG_ENDIAN
#endif

