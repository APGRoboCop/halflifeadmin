/*
 * ===========================================================================
 *
 * $Id: amlibc.h,v 1.6 2003/05/07 21:07:33 darope Exp $
 *
 *
 * Copyright (c) 1999-2002 Alfred Reynolds, Florian Zschocke, Magua
 *
 *   This file is part of Admin Mod.
 *
 *   Admin Mod is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   Admin Mod is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with Admin Mod; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *   In addition, as a special exception, the author gives permission to
 *   link the code of this program with the Half-Life Game Engine ("HL 
 *   Engine") and Modified Game Libraries ("MODs") developed by VALVe, 
 *   L.L.C ("Valve") and Modified Game Libraries developed by Gearbox 
 *   Software ("Gearbox").  You must obey the GNU General Public License 
 *   in all respects for all of the code used other than the HL Engine and 
 *   MODs from Valve or Gearbox. If you modify this file, you may extend 
 *   this exception to your version of the file, but you are not obligated 
 *   to do so.  If you do not wish to do so, delete this exception statement
 *   from your version.
 *
 * ===========================================================================
 *
 * Comments:
 *
 * Standard C library functions with changed behaviour for 
 * programming convenience.
 *
 */

#ifdef LINUX
#  include <sys/types.h>
#  include <stdint.h> /* uint64_t */
#else
#  if (_MSC_VER <= 1300)
typedef __int64 int64_t;
typedef unsigned __int64 uint64_t;
#  else
typedef long long int64_t;
typedef unsigned long long uint64_t;
#  endif

#  include <stddef.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Behaves like strncpy() with the following differences:
 * - does not fill the end of the string with zeroes.
 * - the destination string is always zero-terminated
 */
char *am_strncpy(char *dest, const char *src, size_t n);


/*
 * Behaves like strstr() but match is case-insensitive.
 */
char *am_strcasestr(const char *haystack, const char *needle);



int am_strcasecmp ( const char* s1, const char* s2 );

void* am_memccpy(void *s1, const void *s2, int c, size_t n);

int am_tolower(int ch);


int am_memccmp(const void *s1, const void *s2, int c, size_t n);

uint64_t am_strtoui64(const char *nptr, char **endptr, int base);



#ifdef __cplusplus
}
#endif
