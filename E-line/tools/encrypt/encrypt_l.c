/*
 * ===========================================================================
 *
 * $Id: encrypt_l.c,v 1.1.1.1 2003/08/14 18:51:28 bugblatter Exp $
 *
 *
 * Copyright (c) 1999-2003 Alfred Reynolds, Florian Zschocke, Magua
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
 * ===========================================================================
 *
 * Comments:
 *
 */


#include "encrypt_l.h"
#include <crypt.h>
#include <md5.h>
#include <stdlib.h>
#include <string.h>

#ifdef LINUX
#  include <sys/time.h>
#else
#  include <time.h>
#  define random rand
#endif


static const char* encrypt_crypt( const char* _pcCleartext )
{
    char acSalt[3];
    const char acKeys[] = { '.', '/', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
                            'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l',
                            'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x',
                            'y', 'z', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
                            'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V',
                            'W', 'X', 'Y', 'Z'  };
 
	// Initialize the pseudo random number generator
#ifdef LINUX
    struct timeval tv;
    gettimeofday( &tv, NULL );
    srandom( tv.tv_sec ^ tv.tv_usec );
#else
    srand( (unsigned)time(NULL) );
#endif

    if ( _pcCleartext == NULL ) return NULL;

	// Get a random salt
    acSalt[0] = acKeys[random()%64];
    acSalt[1] = acKeys[random()%64];
    acSalt[2] = 0;
 
    return crypt( _pcCleartext, acSalt ); 	
}


static const char* encrypt_md5( const char* _pcCleartext )
{
	MD5_CTX mdContext;
	unsigned int iLen = strlen ( _pcCleartext );
	int i, iHigh, iLow;
	static char acStaticString[33];

	MD5Init( &mdContext );
	MD5Update( &mdContext, _pcCleartext, iLen );
	MD5Final( &mdContext );

 
	// Fill the return string with the ascii representation of the MD5 digest.
	for (i = 0; i < 16; i++) {
		iHigh = mdContext.digest[i] >> 4;
		iLow  = mdContext.digest[i] & 0x0f;

		if ( iHigh <= 9 ) {
			acStaticString[2*i] = iHigh + 48;
		} else {
			acStaticString[2*i] = iHigh + 87;
		}

		if ( iLow <= 9 ) {
			acStaticString[2*i+1] = iLow + 48;
		} else {
			acStaticString[2*i+1] = iLow + 87;
		}
	}

	acStaticString[32] = '\0';

	return acStaticString;
}



const char* encrypt_password( const char* _pcCleartext, t_encrypt_method _tMethod ) 
{

	switch( _tMethod ) {
	case EM_NONE:
		return _pcCleartext;
		
	case EM_CRYPT:
		return encrypt_crypt( _pcCleartext );

	case EM_MD5:
		return encrypt_md5( _pcCleartext );

	case EM_UNDEF:
		return NULL;

	}  // switch

	return NULL;
}
