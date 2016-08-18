#include <string.h>
#include "statics.h"


apat* pat_search_key( apat* _pPatricia, char* _pcKey ) {

  apat *pThis, *pNext;
  size_t iKeySize = strlen( _pcKey );

  if ( _pPatricia == 0 || _pcKey == 0 ) return 0;
  if ( iKeySize > static_cast<size_t>(PAT_KEY_SIZE) ) return 0;

#if SAVEKEY
  char pcKey[PAT_KEY_SIZE];
  for ( size_t i = 0; i <= iKeySize; i++ ) {
    pcKey[i] = (_pcKey[i] << 1);
  }  // for
#else
  char* pcKey = _pcKey;
#endif


  pThis = pNext =_pPatricia; 
  do { 
    pThis = pNext; 
    pNext = _pPatricia + (pNext->aiSons[(pat_bits(pcKey, iKeySize, pNext->iBitLevel, PAT_RELBITS))]); 
  }  while (pThis->iBitLevel < pNext->iBitLevel); // while

  if ( strcmp(pcKey, pNext->pcKey) == 0 ) {
    return pNext;
  }  // if

  return 0;
}  // search_key()

#if SAVEKEY
const char* pat_get_key( apat* _pNode ) {
  char pcKey[PAT_KEY_SIZE];
  for ( int i = 0; i < _pNode->iKeySize; i++ ) {
    pcKey[i] = (_pNode->pcKey[i] >> 1) & 0x7f;
  } // for
  return pcKey;
}  // getkey()
#endif

#define STACKSIZE 16

const char* get_am_string( char* _pcDest, int _iMaxLen, char* _pucSrc, unsigned char _aucPairTable[128][2] ) {

  static char acStaticBuf[STATSTRING_MAXSIZE];
  unsigned char stack[STACKSIZE];
  short c, top = 0;
  int len;
  char* pcDest = 0;

  if ( _pcDest == 0 ) {
    _pcDest = acStaticBuf;
    _iMaxLen = STATSTRING_MAXSIZE;
    pcDest = acStaticBuf;
  } else {
    pcDest = _pcDest;
  }  // if-else

  len = 1;              /* already 1 byte for '\0' */
  for (;;) {

    /* Pop byte from stack or read byte from the input string */
    if (top)
      c = stack[--top];
    else if ((c = *(unsigned char *)_pucSrc++) == '\0')
      break;

    /* Push pair on stack or output byte to the output string */
    if (c > 127) {
      //assert(top+2 <= STACKSIZE);
      stack[top++] = _aucPairTable[c-128][1];
      stack[top++] = _aucPairTable[c-128][0];
    }
    else {
      len++;
      if (_iMaxLen > 1) { /* reserve one byte for the '\0' */
        *pcDest++ = c;
        _iMaxLen--;
      }
    }
  }
  *pcDest = '\0';
  return _pcDest;           /* return number of bytes decoded */
}
