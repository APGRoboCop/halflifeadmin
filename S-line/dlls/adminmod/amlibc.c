// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
/*
 * ===========================================================================
 *
 * $Id: amlibc.c,v 1.2 2003/03/26 20:44:05 darope Exp $
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
 * Additional copyright info:
 *
 * Most of the code below was taken from the diet libc. The diet libc is GPL
 * software, the copyright holder is Felix von Leitner.
 * http://www.fefe.de/dietlibc/ 
 *
 * The code was modified for Admin Mod needs.
 *
 * ===========================================================================
 *
 * Comments:
 *
 * Standard C library functions with changed behaviour for 
 * programming convenience.
 *
 */

#include <ctype.h>  /* isspace() */
#include <string.h> /* strlen() */
#include "amlibc.h"

 
void *am_memccpy(void *dst, const void *src, int c, size_t count) 
{
  char *a = dst;
  const char *b = src;
  ++count;
  while (--count)
  {
    *a++ = *b;
    if (*b==c)
    {
      return (void *)a;
    }
    b++;
  }
  return 0;
} 


char *am_strncpy(char *dest, const char *src, size_t n) 
{
	if ( am_memccpy(dest,src,0,n) == 0 ) {
		dest[n-1] = 0; // make sure the string is zero-terminated 
	}  // if
	return dest;
}


int  am_strcasecmp ( const char* s1, const char* s2 )
{
    register unsigned int  x2;
    register unsigned int  x1;

    while (1) {
        x2 = *s2++ - 'A'; if (x2 < 26u) x2 += 32;
        x1 = *s1++ - 'A'; if (x1 < 26u) x1 += 32;
        if ( x2 != x1 )
            break;
        if ( x1 == (unsigned int)-'A' )
            break;
    }

    return x1 - x2;
}



char *am_strcasestr(const char *haystack, const char *needle) 
{
	const size_t nl = strlen( needle );
	const size_t hl = strlen( haystack );

  if ( nl > hl ) return 0;

	//for is only compatible with GCC C99 mode, use -std=c99 [APG]RoboCop[CL]
  for ( size_t i = hl - nl + 1; i; --i ) {
    if ( am_tolower(*haystack) == am_tolower(*needle) && !am_strcasecmp(haystack,needle)) return (char*)haystack;

    ++haystack;
  }

  return 0;
}


int am_tolower(int ch) 
{   
	if ( (unsigned int)(ch - 'A') < 26u )
		ch += 'a' - 'A';
	return ch; 
} 



uint64_t am_strtoui64(const char *nptr, char **endptr, int base)
{
  uint64_t v=0;

  while(isspace(*nptr)) ++nptr;

  if (*nptr == '+') ++nptr;
  if (!base) {
    if (*nptr=='0') {
      base=8;
      if ((*(nptr+1)=='x')||(*(nptr+1)=='X')) {
	nptr+=2;
	base=16;
      }
    }
    else
      base=10;
  }
  while(*nptr) {
    register unsigned char c=*nptr;
    c=(c>='a'?c-'a'+10:c>='A'?c-'A'+10:c-'0');
    if (c>=base) break;
    v=v*base+c;
    ++nptr;
  }
  if (endptr) *endptr=(char *)nptr;
  return v;
}

