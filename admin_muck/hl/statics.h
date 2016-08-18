
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
const int PAT_KEY_SIZE = 23;


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
const int STATSTRING_MAXSIZE = 59;
const int STATSTRING_COUNT = 4;


#endif
