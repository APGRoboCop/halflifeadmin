/*
 * ===========================================================================
 *
 * $Id: statics.h,v 1.3 2003/03/26 20:44:10 darope Exp $
 *
 *
 * Copyright (c) 2002-2003 Florian Zschocke
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
 */

#ifndef _STATICS_H_
#define _STATICS_H_


#define SAVEKEY 1

typedef int AttrType;

struct apat {
  int iBitLevel;
  int iKeySize;
  const char* pcKey;
  AttrType atAttribute;
  int aiSons[2];
};

const char PAT_RELBITS = 1;
const int PAT_KEY_SIZE = 20;


#ifndef DEFECLIST
extern apat eclist[];
#endif

inline char pat_bits( char* _pcKey, int _iKeySize, unsigned int _uiPos, unsigned char _ucNum ) {
  unsigned int uiNumBits = sizeof(char) * 8;
  unsigned int uiOffset = _uiPos / uiNumBits;
  if ( uiOffset >= static_cast<unsigned int>(_iKeySize) ) return 0;
  unsigned int uiBitPos = _uiPos % uiNumBits;
  return( (_pcKey[uiOffset] >> (uiNumBits-uiBitPos-_ucNum)) & ~(~0 << _ucNum) );
}

#if !SAVEKEY
inline const char* pat_get_key( apat* pNode ) {
  return pNode->pcKey;
}
#else
const char* pat_get_key( apat* pNode );
#endif

apat* pat_search_key( apat* Patricia, char* Key );
const char* get_am_string( char* Dest, int MaxLength, char* Src, unsigned char Table[128][2] );

#ifndef DEFSTATLIST
extern unsigned char statstr_table[][2];
extern char* statstr[];
#endif
const int STATSTRING_MAXSIZE = 77;
const int STATSTRING_COUNT = 9;


#endif
