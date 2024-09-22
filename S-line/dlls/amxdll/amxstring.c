/***************************************************************
 * 
 * File: amxstring.c
 *
 * Date: 06.02.2001
 *
 * Author: Florian Zschocke
 *
 * Description: This file exposes the string manipulation
 *              functions from string.h to the Small engine.
 *
 *  $Id: amxstring.c,v 1.2 2004/07/23 21:39:19 darope Exp $
 *
 ****************************************************************/

//#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include "amx.h"

#ifdef WIN32
#include <malloc.h>
#define strcasecmp _stricmp
#define strncasecmp _strnicmp
//#define snprintf _snprintf
#endif

/************************  SOME CONFIGURATION  ***********************************************************/

#define AMXFAIL (-1)
#define AMXNULL 0
#define AMXOK 1
#define AMXNULLP (-1)

// #define DEBUG

#define USE_PACKED_STRINGS 0  /* 1 to enable, 0 to disable */

#define ENABLE_RAISE_ERROR 0  /* 1 raises error, 0 doesn't */

#define STATIC_STRING_LEN 512    /* size of static strings */
#define STATIC_STRINGS 3         /* number of static strings */
#define LOCAL_STRINGS 23         /* overall number of strings that can be used */
#define TOKEN_STRING_LEN 1024    /* size of static string to be tokenized (strtok) */
#define GTOKEN_STRING_LEN 1024    /* size of static string to be tokenized (strgtok) */

/*************************  MACROS AND GLOBAL VARIABLES  *************************************************/

#if ENABLE_RAISE_ERROR == 1
# define ASSERT_ERROR free_strings();amx_RaiseError(amx, AMX_ERR_ASSERT)
# define BOUNDS_ERROR free_strings();amx_RaiseError(amx, AMX_ERR_BOUNDS)
# define MEMACCESS_ERROR free_strings();amx_RaiseError(amx, AMX_ERR_MEMACCESS)
# define NATIVE_ERROR free_strings();amx_RaiseError(amx, AMX_ERR_NATIVE)
# define MEMORY_ERROR free_strings();amx_RaiseError(amx, AMX_ERR_MEMORY)
# define RAISE_ERROR(a) free_strings();amx_RaiseError(amx, (a))
#endif
#if ENABLE_RAISE_ERROR == 2
# define ASSERT_ERROR fprintf(stderr,"Raising error ASSERT\n");amx_RaiseError(amx, AMX_ERR_ASSERT)
# define BOUNDS_ERROR  fprintf(stderr,"Raising error BOUNDS\n");amx_RaiseError(amx, AMX_ERR_BOUNDS)
# define MEMACCESS_ERROR  fprintf(stderr,"Raising error MEMACCESS\n");amx_RaiseError(amx, AMX_ERR_MEMACCESS)
# define NATIVE_ERROR  fprintf(stderr,"Raising error NATIVE\n");amx_RaiseError(amx, AMX_ERR_NATIVE)
# define MEMORY_ERROR  fprintf(stderr,"Raising error MEMORY\n");amx_RaiseError(amx, AMX_ERR_MEMORY)
# define RAISE_ERROR(a)  fprintf(stderr,"Raising error %d:\n",err);amx_RaiseError(amx, (a))
#else
# define ASSERT_ERROR 
# define BOUNDS_ERROR 
# define MEMACCESS_ERROR 
# define NATIVE_ERROR 
# define MEMORY_ERROR 
# define RAISE_ERROR(a)
#endif


typedef struct {
  char cDynamic;
  char* pcString;
  size_t tSpace;
} struct_Lstring;

static char g_cTokenStringInitialized = 0;
static char* g_pcTokenStringPos = NULL;
static char* g_pcTokenStringEnd = NULL;
static char g_acTokenString[TOKEN_STRING_LEN];

static char g_cGTokenStringInitialized = 0;
static char* g_pcGTokenStringPos = NULL;
static char* g_pcGTokenStringEnd = NULL;
static char g_acGTokenString[GTOKEN_STRING_LEN];

static char g_aacStaticStrings[STATIC_STRINGS][STATIC_STRING_LEN];
static struct_Lstring g_sLstring[LOCAL_STRINGS];


#define min(a,b) ((a)<(b))?(a):(b)
#define max(a,b) ((a)>(b))?(a):(b)



/***********************************  LOCAL HELPER FUNCTIONS  *******************************************************/



/*
   get_space()
   allocate space to hold a string
*/
static int get_space( char** _string, size_t _iMinLen, size_t* _piStrLen ) 
{
  char* pcNewstr = NULL;
  int i;
  int iLocalFree = 0;
  int iStaticFree = 0;
  int iLsIndex = -1;
  const int err = AMX_ERR_NONE;

  if ( _string == NULL ) {
    return AMX_ERR_NATIVE;
  }  // if


  /* check if we can get another string */
  for ( i = 0; i < LOCAL_STRINGS; i++ ) {
    if ( g_sLstring[i].pcString != NULL ) {
      if ( g_sLstring[i].cDynamic == 0 ) {
	iStaticFree++;
      }  // if
      iLocalFree++;
    } else {
      if ( iLsIndex < 0 ) iLsIndex = i;
    }  // if-else
  }  // for
  iStaticFree = STATIC_STRINGS - iStaticFree;
  iLocalFree = LOCAL_STRINGS - iLocalFree;
  if ( iLocalFree < 1 ) {
    return AMX_ERR_NATIVE;
  }  // if

  /* check if we can put the string into a static memory buffer */
  /* otherwise we allocate dynamic memory for it */
  if ( _iMinLen < STATIC_STRING_LEN && (iStaticFree > 0) ) {

    memset( g_aacStaticStrings[iLsIndex], 0, (STATIC_STRING_LEN * sizeof(char)) ); 
    *_string = g_aacStaticStrings[iLsIndex];
    g_sLstring[iLsIndex].pcString = g_aacStaticStrings[iLsIndex];
    g_sLstring[iLsIndex].cDynamic = 0;
    g_sLstring[iLsIndex].tSpace = STATIC_STRING_LEN;

  } else {
#ifdef DEBUG
    fprintf( stderr, "bind allocating %d times %d byte\n",_iMinLen+1,sizeof(char));  
#endif
    pcNewstr = (char*)calloc( _iMinLen+1, sizeof(char) );
    if ( pcNewstr == 0 ) {
      return AMX_ERR_MEMORY;
    }  // if
    if ( g_sLstring[iLsIndex].pcString != 0 ) {
      free( g_sLstring[iLsIndex].pcString );
    }  // if
    g_sLstring[iLsIndex].pcString = pcNewstr;
    g_sLstring[iLsIndex].cDynamic = 1;
    g_sLstring[iLsIndex].tSpace = _iMinLen +1 ;
    *_string = pcNewstr;
    pcNewstr = 0;

  }  // if-else

  /* return the actual memory size allocated */
  if ( _piStrLen != NULL ) {
    *_piStrLen = g_sLstring[iLsIndex].tSpace - 1;
  }  // if

  return err;
}  // get_space()  


/*
  str_space()
  return the allocated memory for a string pointer
*/
static size_t str_space( const char* _pcPointer ) {

  size_t tSpace = 0;
  int iLoop;
  
  for ( iLoop = 0; iLoop < LOCAL_STRINGS; iLoop++ ) {
    if ( g_sLstring[iLoop].pcString == _pcPointer ) {
      tSpace = g_sLstring[iLoop].tSpace;
      break;
    }  // if
  }  // for

  return tSpace;

}  // str_space()

/*
   bind_string()
   bind a Small string to a C string memory area
*/
static int bind_string( AMX* _amx, char** _string, cell _cstrAddress, size_t* _piStrLen, size_t _iMinLen ) {

  int err = AMX_ERR_NONE;
  int iCstrLength = 0;
  size_t iNewstrLen = 0;
  cell* ptCellString = 0;

  if ( _string == NULL ) {
    return AMX_ERR_NATIVE;
  }  // if
  

  /* get the memory address of the Small string */
  err = amx_GetAddr( _amx, _cstrAddress, &ptCellString );
  if ( err != AMX_ERR_NONE ) {
    return err;
  }  // if

  /* get the size of the Small string */
  err = amx_StrLen( ptCellString, &iCstrLength );
  if ( err != AMX_ERR_NONE ) {
    return err;
  }  // if

  /* if more space is requested for this string we allocate more */
  iNewstrLen = (iCstrLength > _iMinLen) ? iCstrLength : _iMinLen;

  /* get memory space to hold the string */
  err = get_space( _string, iNewstrLen, _piStrLen );
  if ( err != AMX_ERR_NONE ) {
    return err;
  }  // if

  /* change of plans: we return the string length not space*/
  if ( _piStrLen != NULL )
    *_piStrLen = iCstrLength;

  return err;
}  // bind_string()


/*
  get_string()
  get a Small string into a C string 
*/
static int get_string( AMX* _amx, char** _string, cell _cstrAddress, size_t* _piStrLen, size_t _iMinLen ) {


  int err = AMX_ERR_NONE;
  int iCstrLength = 0;
  size_t tNewstrLen = 0;

  cell* ptCellString = 0;

  if ( _string == NULL ) {
    return( AMX_ERR_NATIVE );
  } // if
  

  /* get the memory address of the Small string */
  err = amx_GetAddr( _amx, _cstrAddress, &ptCellString );
  if ( err != AMX_ERR_NONE ) {
    return err;
  }  // if

  /* get the size of the Small string */
  err = amx_StrLen( ptCellString, &iCstrLength );
  if ( err != AMX_ERR_NONE ) {
    return err;
  }  // if

  /* if we want more space than the string is long we expand it */
  tNewstrLen = ((size_t)iCstrLength > _iMinLen) ? (size_t)iCstrLength : _iMinLen;

  /* get memory space to hold the string */
  err = get_space( _string, tNewstrLen, _piStrLen );
  if ( err != AMX_ERR_NONE ) {
    return err;
  }  // if

  /* set the C string to the Small string */
  err = amx_GetString( *_string, ptCellString );
  if ( err != AMX_ERR_NONE ) {
    return err;
  }  // if

  /* change of plans: we return the string length not space */
  if ( _piStrLen != NULL )
    *_piStrLen = (size_t)iCstrLength;

  return err;
}  // get_string()


/* 
   set_string() 
   set a C string to a Small string 
*/
static int set_string( AMX* _amx, cell _cstrAddress, char* _string, size_t _iMaxLen ) {

  int err = AMX_ERR_NONE;
  cell* ptCellString = NULL;
  const char* pcEmpty = "";
  const char* pcString = NULL;
  
  char cTemp = 0;

  /* get the memory address of the Small string */
  err = amx_GetAddr( _amx, _cstrAddress, &ptCellString );
  if ( err != AMX_ERR_NONE ) {
    return err;
  }  // if


  if ( _string != 0 ) {
#ifdef DEBUG
    fprintf( stderr, "String to set: %s\n", _string );
#endif

    /* cut the C string to iMaxLen */
    if ( (_iMaxLen > 0) && (_iMaxLen < strlen(_string)) ) {
      cTemp = *(_string + _iMaxLen);
      *(_string + _iMaxLen) = 0;
    }  // if
    pcString = _string;
  } else {
    pcString = pcEmpty;
#ifdef DEBUG
    fprintf( stderr, "Zero string set to empty string:%s.\n", pcString );
#endif
  }  // if


  /* set the Small string */
  err = amx_SetString( ptCellString, pcString, USE_PACKED_STRINGS );
  /* restore our string */
  if ( cTemp ) *(_string + _iMaxLen) = cTemp;

  if ( err != AMX_ERR_NONE ) {
    return err;
  }  // if

  return err;
}  // set_string()


/* 
   string_len() 
   get the length of a Small string
*/
static int string_len( AMX* _amx, cell _cstrAddress, size_t* _iStrLen ) {

  int err = AMX_ERR_NONE;
  int len = 0;
  cell* ptCellString = 0;

  /* get the memory address of the Small string */
  err = amx_GetAddr( _amx, _cstrAddress, &ptCellString );
  if ( err != AMX_ERR_NONE ) {
    return err;
  }  // if

  /* set the Small string */
  err = amx_StrLen( ptCellString, &len );
  if ( err != AMX_ERR_NONE ) {
    return err;
  }  // if
  *_iStrLen = (size_t)len;
  
  return err;
}  // string_len()




/* 
   clear_string() 
   set a Small string to an empty string 
*/
static int clear_string( AMX* _amx, cell _cstrAddress, size_t _iStrLen ) {

  int err = AMX_ERR_NONE;
  int i = 0;
  cell* ptCellString = 0;

  /* get the memory address of the Small string */
  err = amx_GetAddr( _amx, _cstrAddress, &ptCellString );
  if ( err != AMX_ERR_NONE ) {
    return err;
  }  // if

#if USE_PACKED_STRINGS
  _iStrLen /= 4;
#endif
  
  if ( _iStrLen <= 1 ) {
    *ptCellString = 0;
  } else {
    for ( i = 0; i < _iStrLen; i++ ) ptCellString[i] = 0;
  }  // if-else

  return err;
}  // clear_string()


/*
   free_strings()
   free allocated memory
*/
static void free_strings( void ) 
{

  /* free all dynamic strings and clear the local strings set */
  int index;

#ifdef DEBUG
  int err;
  for ( err = 0; err < 5; err++ ){
    fprintf( stderr, "free dyn: %d pointer: %p string: %s \n", g_sLstring[err].cDynamic, g_sLstring[err].pcString, g_sLstring[err].pcString );
  }

  for ( err = 0; err < STATIC_STRINGS; err++ ){
    fprintf( stderr, "static: %d pointer: %p string: %s \n", err, g_aacStaticStrings[err], g_aacStaticStrings[err] );
  }
#endif

  for ( index = 0; index < LOCAL_STRINGS; index++ ) {
    if ( g_sLstring[index].cDynamic == 1 ) {
      free( g_sLstring[index].pcString ); 
    } else if ( g_sLstring[index].pcString != NULL ) {
      memset( g_sLstring[index].pcString, 0, (STATIC_STRING_LEN * sizeof(char)) );
    }  // if-else
    g_sLstring[index].pcString = 0;
    g_sLstring[index].cDynamic = 0;
  }  // for

  return;
}  // free_strings()


/* 
   Return()
   cleanup bofore returning
*/
static cell Return( cell _iReturnValue ) 
{

  /* free all dynamic strings before returning */
  free_strings();
  return _iReturnValue;
}  //Return()




/*********************************   ENHANCED LOCAL STRING FUNCTIONS *****************************************************/
 

static const char* _strgtok( char* _pcString, char* _pcDelimiter, char* _pcGrouping ) {
	const char* pcDelimiters = NULL;
	const char* pcGroupingSet = NULL;
	const char* pcToken = NULL;

  static char* pcString;
  static char* pcStart;
  static char* pcEndOfString;
  static char* pcGrouper;

  static char bInGroup;

  char  cLastGrouper = 0;


  /* get the delimiters */
  if ( _pcDelimiter == NULL ) return NULL;
  pcDelimiters = _pcDelimiter;

  /* get the grouping set */
  if ( _pcGrouping == NULL ) return NULL;
  pcGroupingSet = _pcGrouping;

  /* get the string */
  if ( _pcString == NULL ) {
    /* if this is a subsequent call without an initial call, return */
    if ( pcString == NULL ) return NULL;
  } else {
    pcString = _pcString;
    pcEndOfString = pcString + strlen( _pcString );
    pcStart = pcString;
    pcGrouper = NULL;
    bInGroup = 0;
  }  // if-else

  /* check for a group */
  if ( ! bInGroup ) {
    pcGrouper = strpbrk( pcStart, pcGroupingSet );
    if ( pcGrouper != NULL && pcGrouper > pcString ) {
      while ( pcGrouper != NULL && *(pcGrouper - 1) == '\\' ) {
	memmove( (pcGrouper-1), pcGrouper, (pcEndOfString - pcGrouper) );
	pcGrouper = strpbrk( pcGrouper, pcGroupingSet );
      }  // while
    }  // if

    /* skip leading delimiter chars, since strtok does this, too */
    pcStart += strspn( pcStart, pcDelimiters );

  }  // if


  if ( (!bInGroup) && pcGrouper == NULL ) {
    /* there are no groups */
    
    /* call to strtok, always initial */
    pcToken = strtok( pcStart, pcDelimiters );
      
    /* set static pointers */
    pcStart += strlen( pcStart );
    pcStart++;

    /* check if string is done with */
    if ( pcStart >= pcEndOfString ) {
      pcString = NULL;
      pcStart = NULL;
      pcEndOfString = NULL;
      pcGrouper = NULL;
    }  // if

  } else if ( bInGroup || pcGrouper == pcStart ) {
    /* a group starts */
    
    /* set the start to the start of the group */
    if ( pcStart == pcGrouper ) pcStart++;
    
    /* get the end of the group */
    pcGrouper = strpbrk( pcStart, pcGroupingSet );
    if ( pcGrouper != NULL && pcGrouper > pcString ) {
      while ( pcGrouper != NULL && *(pcGrouper - 1) == '\\' ) {
	memmove( (pcGrouper-1), pcGrouper, (pcEndOfString - pcGrouper) );
	pcGrouper = strpbrk( pcGrouper, pcGroupingSet );
      }  // while
    }  // if
    
    if ( pcGrouper == NULL ) {
      pcGrouper = pcEndOfString;
    } else {
      cLastGrouper = *pcGrouper;
      *pcGrouper = 0;
    }  // if-else
    
    pcToken = pcStart;
    
    /* set static pointers */
    pcStart = pcGrouper + 1;

    bInGroup = 0;

    /* check if string is done with */
    if ( pcStart >= pcEndOfString ) {
      pcString = NULL;
      pcStart = NULL;
      pcEndOfString = NULL;
      pcGrouper = NULL;
    }  // if
    

  } else if ( (!bInGroup) && pcGrouper > pcStart ) {  
    /* tokenize the part before the group */

    cLastGrouper = *pcGrouper;
    *pcGrouper = 0;

    /* call to strtok, always initial */
    pcToken = strtok( pcStart, pcDelimiters );
    
    /* set static pointers */
    pcStart += strlen( pcStart );
    pcStart++;

    /* check if we entered the group */
    if ( pcStart > pcGrouper ) {
      bInGroup = 1;
    } else {
      bInGroup = 0;
      *pcGrouper = cLastGrouper;
    }  // if-else

    /* check if string is done with */
    if ( pcStart >= pcEndOfString ) {
      pcString = NULL;
      pcStart = NULL;
      pcEndOfString = NULL;
      pcGrouper = NULL;
      bInGroup = 0;
    }  // if
    
    /* reset the grouping char */
    //*pcGrouper = cLastGrouper;
    
  } else {
    /* this case should not occur */
    return NULL;
  }  // if-else
    
  
  return pcToken;
}  // _strgtok()






/*********************************   EXPORTED FUNCTIONS  ****************************************************************/




/* amx_strcpy ( destination, source, maxlen ) 
 *
 * implements char* strcpy ( char* to, char* from )
 * destination: string array to copy to
 * source     : string array to copy from
 * maxlen     : maximum length of destination inlcuding \0
 *
 * returns AMXNULL on error, else 1
 *
 */

static cell AMX_NATIVE_CALL amx_strcpy( AMX* amx, cell* params )
{

  int err = 0;
  const int iNumArgs = params[0] / sizeof(cell);
  size_t iMaxLength = 0;
  size_t iFromLength = 0;
  char* pcTo = NULL;
  char* pcFrom = NULL;
  const char* pcRetVal = NULL;

  const int REQNUMARGS = 3;

  if ( iNumArgs != REQNUMARGS ) {
    NATIVE_ERROR;
    return Return(AMXFAIL);
  }  // if

  /* first get the source!! */
  err = get_string( amx, &pcFrom, params[2], &iFromLength, 0L );
  if ( err != AMX_ERR_NONE ) {
    RAISE_ERROR(err);
    return Return(AMXFAIL);
  }  // if

  if ( params[3] ) {
    iMaxLength = min( (size_t)(params[3]-1), iFromLength );
  }  // if

  /* allocate memory long enough for the source!! */
  err = bind_string( amx, &pcTo, params[1], NULL, iFromLength );
  if ( err != AMX_ERR_NONE ) {
    RAISE_ERROR(err);
    return Return(AMXFAIL);
  }  // if


  /* call the standard string function */
  pcRetVal = strcpy( pcTo, pcFrom );
  if ( pcRetVal == NULL ) {
    NATIVE_ERROR;
    return Return(AMXNULL);
  }  // if


  /* set the string in the VM */
  err = set_string( amx, params[1], pcTo, iMaxLength );
  if ( err != AMX_ERR_NONE ) {
    RAISE_ERROR(err);
    return Return(AMXFAIL);
  }  // if
  

  return Return(AMXOK);

}  // amx_strcpy()



/* amx_strncpy ( destination, source, num, maxlen ) 
 *
 * implements char* strncpy ( char* to, char* from, size_t num )
 * destination: string array to copy to
 * source     : string array to copy from
 * num        : number of characters copied from 'from'
 * maxlen     : maximum length of destination including \0
 *
 * returns AMXNULL on error, else 1
 *
 */

static cell AMX_NATIVE_CALL amx_strncpy( AMX* amx, cell* params )
{

  int err = 0;
  const int iNumArgs = params[0] / sizeof(cell);
  size_t iToLength = 0;
  size_t iFromLength = 0;
  size_t iMaxLength = 0;
  char* pcTo = NULL;
  char* pcFrom = NULL;
  const char* pcRetVal = NULL;

  const int REQNUMARGS = 4;

  if ( iNumArgs != REQNUMARGS ) {
    NATIVE_ERROR;
    return Return(AMXFAIL);
  }  // if

  /* first get the source to determine the length */
  err = get_string( amx, &pcFrom, params[2], &iFromLength, 0L );
  if ( err != AMX_ERR_NONE ) {
    RAISE_ERROR(err);
    return Return(AMXFAIL);
  }  // if

  /* lets not get things beyond the string boundary */
  iToLength = ( params[3] > iFromLength) ? iFromLength : params[3];
  if ( params[4] ) {
    iMaxLength = min( (params[4]-1), iFromLength );
  }  // if

  /* allocate memory long enough for the source!! */
  err = bind_string( amx, &pcTo, params[1], NULL, iFromLength );
  if ( err != AMX_ERR_NONE ) {
    RAISE_ERROR(err);
    return Return(AMXFAIL);
  }  // if


  /* call the standard string function  */
  pcRetVal = strncpy( pcTo, pcFrom, iToLength );
  if ( pcRetVal == NULL ) {
    NATIVE_ERROR;
    return Return(AMXNULL);
  }  // if

  /* set the string in the VM */
  err = set_string( amx, params[1], pcTo, iMaxLength );
  if ( err != AMX_ERR_NONE ) {
    RAISE_ERROR(err);
    return Return(AMXFAIL);
  }  // if


  return Return(AMXOK);

}  // amx_strncpy()




/* amx_strcat ( destination, source, maxlen ) 
 *
 * implements char* strcat ( char* to, char* from )
 * destination: string array to concatenate to
 * source     : string array to copy from
 * maxlen     : maximum length of destination including \0
 *
 * returns AMXNULL on error, else 1
 *
 */

static cell AMX_NATIVE_CALL amx_strcat( AMX* amx, cell* params )
{

  int err = 0;
  const int iNumArgs = params[0] / sizeof(cell);
  size_t iToLength = 0;
  size_t iFromLength = 0;
  size_t iNewLength = 0;
  size_t iMaxLength = 0;
  char* pcTo = NULL;
  char* pcFrom = NULL;
  const char* pcRetVal = NULL;

  const int REQNUMARGS = 3;

  if ( iNumArgs != REQNUMARGS ) {
    NATIVE_ERROR;
    return Return(AMXFAIL);
  }  // if

  /* first get the source */
  err = get_string( amx, &pcFrom, params[2], &iFromLength, 0L );
  if ( err != AMX_ERR_NONE ) {
    RAISE_ERROR(err);
    return Return(AMXFAIL);
  }  // if
  
  /* get the size of the destination */
  err = string_len( amx, params[1], &iToLength );
  if ( err != AMX_ERR_NONE ) {
    RAISE_ERROR(err);
    return Return(AMXFAIL);
  }  // if
  

  /* get the size of the destination */
  iNewLength = iFromLength + iToLength;
  
  /* respect array boundaries */
  if( params[3] != 0 ) {
    iMaxLength = min( iNewLength, (params[3]-1) );
  }  // if

  /* get the destination */
  err = get_string( amx, &pcTo, params[1], &iToLength, iNewLength );
  if ( err != AMX_ERR_NONE ) {
    RAISE_ERROR(err);
    return Return(AMXFAIL);
  }  // if

  /* not enough memory? */
  if( str_space( pcTo ) <= iNewLength ) {
    NATIVE_ERROR;
    return Return(AMXFAIL);
  }  // if


  /* call the standard string function */
  pcRetVal = strcat( pcTo, pcFrom );
  if ( pcRetVal == NULL ) {
    NATIVE_ERROR;
    return Return(AMXNULL);
  }  // if


  /* set the string in the VM */
  err = set_string( amx, params[1], pcTo, iMaxLength );
  if ( err != AMX_ERR_NONE ) {
    RAISE_ERROR(err);
    return Return(AMXFAIL);
  }  // if
  

  return Return(AMXOK);

}  // amx_strcat()






/* amx_strncat ( destination, source, num, maxlen ) 
 *
 * implements char* strncat ( char* to, char* from, length )
 * destination: string array to concatenate to
 * source     : string array to copy from
 * num        : number of characters copied from source to destination
 * maxlen     : maximum lnegth of destination inlcuding \0
 *
 * returns AMXNULL on error, else 1
 *
 */

static cell AMX_NATIVE_CALL amx_strncat( AMX* amx, cell* params )
{

  int err = 0;
  const int iNumArgs = params[0] / sizeof(cell);
  size_t iToLength = 0;
  size_t iFromLength = 0;
  size_t iNewLength = 0;
  size_t iMaxLength = 0;
  char* pcTo = NULL;
  char* pcFrom = NULL;
  const char* pcRetVal = NULL;

  const int REQNUMARGS = 4;

  if ( iNumArgs != REQNUMARGS ) {
    NATIVE_ERROR;
    return Return(AMXFAIL);
  }  // if

  /* first get the source */
  err = get_string( amx, &pcFrom, params[2], &iFromLength, 0L );
  if ( err != AMX_ERR_NONE ) {
    RAISE_ERROR(err);
    return Return(AMXFAIL);
  }  // if
  
  /* get the size of the destination */
  err = string_len( amx, params[1], &iToLength );
  if ( err != AMX_ERR_NONE ) {
    RAISE_ERROR(err);
    return Return(AMXFAIL);
  }  // if
  

  /* get the size of the destination */
  iNewLength = (size_t)params[3] + iToLength;

  /* respect array boundaries */
  if( params[4] != 0 ) {
    iMaxLength = min( iNewLength, (params[4]-1) );
  }  // if

  /* get the destination */
  err = get_string( amx, &pcTo, params[1], &iToLength, iNewLength );
  if ( err != AMX_ERR_NONE ) {
    RAISE_ERROR(err);
    return Return(AMXFAIL);
  }  // if

  if( str_space(pcTo) <= iNewLength ) {
    NATIVE_ERROR;
    return Return(AMXFAIL);
  }  // if


  /* call the standard string function */
  pcRetVal = strncat( pcTo, pcFrom, params[3] );
  if ( pcRetVal == NULL ) {
    NATIVE_ERROR;
    return Return(AMXNULL);
  }  // if


  /* set the string in the VM */
  err = set_string( amx, params[1], pcTo, iMaxLength );
  if ( err != AMX_ERR_NONE ) {
    RAISE_ERROR(err);
    return Return(AMXFAIL);
  }  // if
  

  return Return(AMXOK);

}  // amx_strncat()



/* amx_strcmp ( string1, string2 ) 
 *
 * implements char* strcmp ( const char* s1, const char* s1 )
 * string1 : string to be compared with string2
 * string2 : string against which string1 is compared
 *
 * returns 0 on equality, 1 if string2 > string1 else -1
 *
 */

static cell AMX_NATIVE_CALL amx_strcmp( AMX* amx, cell* params )
{

  int err = 0;
  const int iNumArgs = params[0] / sizeof(cell);
  int iRetVal = 0;

  char* pcString1 = NULL;
  char* pcString2 = NULL;

  const int REQNUMARGS = 2;

  if ( iNumArgs != REQNUMARGS ) {
    NATIVE_ERROR;
    return Return(AMXFAIL);
  }  // if

  /* first get the source!! */
  err = get_string( amx, &pcString2, params[2], NULL, 0L );
  if ( err != AMX_ERR_NONE ) {
    RAISE_ERROR(err);
    return Return(AMXFAIL);
  }  // if

  /* allocate memory long enough for the source!! */
  err = get_string( amx, &pcString1, params[1], NULL, 0L );
  if ( err != AMX_ERR_NONE ) {
    RAISE_ERROR(err);
    return Return(AMXFAIL);
  }  // if


  /* call the standard string function */
  iRetVal = strcmp( pcString1, pcString2 );

  return Return(iRetVal);

}  // amx_strcmp()






/* amx_strncmp ( string1, string2, num ) 
 *
 * implements char* strncmp ( const char* s1, const char* s1, size_t size )
 * string1 : string to be compared with string2
 * string2 : string against which string1 is compared
 * num     : number of initial charaters to compare
 *
 * returns 0 on equality, 1 if string2 > string1 else -1
 *
 */

static cell AMX_NATIVE_CALL amx_strncmp( AMX* amx, cell* params )
{

  int err = 0;
  const int iNumArgs = params[0] / sizeof(cell);
  int iRetVal = 0;

  char* pcString1 = NULL;
  char* pcString2 = NULL;

  const int REQNUMARGS = 3;

  if ( iNumArgs != REQNUMARGS ) {
    NATIVE_ERROR;
    return Return(AMXFAIL);
  }  // if

  /* first get the source!! */
  err = get_string( amx, &pcString2, params[2], NULL, 0L );
  if ( err != AMX_ERR_NONE ) {
    RAISE_ERROR(err);
    return Return(AMXFAIL);
  }  // if

  /* allocate memory long enough for the source!! */
  err = get_string( amx, &pcString1, params[1], NULL, 0L );
  if ( err != AMX_ERR_NONE ) {
    RAISE_ERROR(err);
    return Return(AMXFAIL);
  }  // if


  /* call the standard string function */
  iRetVal = strncmp( pcString1, pcString2, params[3] );

  return Return(iRetVal);

}  // amx_strncmp()






/* amx_strcasecmp ( string1, string2 ) 
 *
 * implements char* strcasecmp ( const char* s1, const char* s1 )
 * string1 : string to be compared with string2
 * string2 : string against which string1 is compared
 *
 * returns 0 on equality, 1 if string2 > string1 else -1
 *
 */

static cell AMX_NATIVE_CALL amx_strcasecmp( AMX* amx, cell* params )
{

  int err = 0;
  const int iNumArgs = params[0] / sizeof(cell);
  int iRetVal = 0;

  char* pcString1 = NULL;
  char* pcString2 = NULL;

  const int REQNUMARGS = 2;

  if ( iNumArgs != REQNUMARGS ) {
    NATIVE_ERROR;
    return Return(AMXFAIL);
  }  // if

  /* first get the source!! */
  err = get_string( amx, &pcString2, params[2], NULL, 0L );
  if ( err != AMX_ERR_NONE ) {
    RAISE_ERROR(err);
    return Return(AMXFAIL);
  }  // if

  /* allocate memory long enough for the source!! */
  err = get_string( amx, &pcString1, params[1], NULL, 0L );
  if ( err != AMX_ERR_NONE ) {
    RAISE_ERROR(err);
    return Return(AMXFAIL);
  }  // if


  /* call the standard string function */
  iRetVal = strcasecmp( pcString1, pcString2 );


  return Return(iRetVal);

}  // amx_strcasecmp()






/* amx_strncasecmp ( string1, string2, num ) 
 *
 * implements char* strncasecmp ( const char* s1, const char* s1, size_t size )
 * string1 : string to be compared with string2
 * string2 : string against which string1 is compared
 * num     : number of initial charaters to compare
 *
 * returns 0 on equality, 1 if string2 > string1 else -1
 *
 */

static cell AMX_NATIVE_CALL amx_strncasecmp( AMX* amx, cell* params )
{

  int err = 0;
  const int iNumArgs = params[0] / sizeof(cell);
  int iRetVal = 0;

  char* pcString1 = NULL;
  char* pcString2 = NULL;

  const int REQNUMARGS = 3;

  if ( iNumArgs != REQNUMARGS ) {
    NATIVE_ERROR;
    return Return(AMXFAIL);
  }  // if

  /* first get the source!! */
  err = get_string( amx, &pcString2, params[2], NULL, 0L );
  if ( err != AMX_ERR_NONE ) {
    RAISE_ERROR(err);
    return Return(AMXFAIL);
  }  // if

  /* allocate memory long enough for the source!! */
  err = get_string( amx, &pcString1, params[1], NULL, 0L );
  if ( err != AMX_ERR_NONE ) {
    RAISE_ERROR(err);
    return Return(AMXFAIL);
  }  // if



  /* call the standard string function */
  iRetVal = strncasecmp( pcString1, pcString2, params[3] );

  return Return(iRetVal);

}  // amx_strncasecmp()



/* amx_strchr ( string, char ) 
 *
 * implements char* strchr ( const char* string, int character )
 * string   : string to search
 * character: character to search for in string
 *
 * returns index of character in array (zero-based) 
 * or AMXNULLP if the character is not found in the string
 *
 */

static cell AMX_NATIVE_CALL amx_strchr( AMX* amx, cell* params )
{

  int err = 0;
  const int iNumArgs = params[0] / sizeof(cell);
  int iRetVal = 0;

  char* pcString = NULL;
  const char* pcRetVal = NULL;

  const int REQNUMARGS = 2;

  if ( iNumArgs != REQNUMARGS ) {
    NATIVE_ERROR;
    return Return(AMXFAIL);
  }  // if

  /* first get the source!! */
  err = get_string( amx, &pcString, params[1], NULL, 0L );
  if ( err != AMX_ERR_NONE ) {
    RAISE_ERROR(err);
    return Return(AMXFAIL);
  }  // if


  /* call the standard string function */
  pcRetVal = strchr( pcString, params[2] );
  
  if ( pcRetVal == NULL ) {
    return Return(AMXNULLP);
  }
  iRetVal = pcRetVal - pcString;
  return Return(iRetVal);
  // if-else

}  // amx_strchr()





/* amx_strrchr ( string, char ) 
 *
 * implements char* strrchr ( const char* string, int character )
 * string   : string to search from the right
 * character: character to search for in string
 *
 * returns index of character in array (zero-based) 
 * or AMXNULLP if the character is not found in the string
 *
 */

static cell AMX_NATIVE_CALL amx_strrchr( AMX* amx, cell* params )
{

  int err = 0;
  const int iNumArgs = params[0] / sizeof(cell);
  int iRetVal = 0;

  char* pcString = NULL;
  const char* pcRetVal = NULL;

  const int REQNUMARGS = 2;

  if ( iNumArgs != REQNUMARGS ) {
    NATIVE_ERROR;
    return Return(AMXFAIL);
  }  // if

  /* first get the source!! */
  err = get_string( amx, &pcString, params[1], NULL, 0L );
  if ( err != AMX_ERR_NONE ) {
    RAISE_ERROR(err);
    return Return(AMXFAIL);
  }  // if


  /* call the standard string function */
  pcRetVal = strrchr( pcString, params[2] );
  
  if ( pcRetVal == NULL ) {
    return Return(AMXNULLP);
  }
  iRetVal = pcRetVal - pcString;
  return Return(iRetVal);
  // if-else

}  // amx_strrchr()





/* amx_strstr ( string, substring ) 
 *
 * implements char* strstr ( const char* string, const char* substring )
 * string   : string to search in
 * substring: substring to search for in string
 *
 * returns index of substring in array (zero-based) 
 * or AMXNULLP if the substring is not found in the string
 *
 */

static cell AMX_NATIVE_CALL amx_strstr( AMX* amx, cell* params )
{

  int err = 0;
  const int iNumArgs = params[0] / sizeof(cell);
  int iRetVal = 0;

  char* pcString = NULL;
  char* pcSubString = NULL;
  const char* pcRetVal = NULL;


  const int REQNUMARGS = 2;

  if ( iNumArgs != REQNUMARGS ) {
    NATIVE_ERROR;
    return Return(AMXFAIL);
  }  // if

  /* get the haystack */
  err = get_string( amx, &pcString, params[1], NULL, 0L );
  if ( err != AMX_ERR_NONE ) {
    RAISE_ERROR(err);
    return Return(AMXFAIL);
  }  // if

  /* get the needle */
  err = get_string( amx, &pcSubString, params[2], NULL, 0L );
  if ( err != AMX_ERR_NONE ) {
    RAISE_ERROR(err);
    return Return(AMXFAIL);
  }  // if


  /* call the standard string function */
  pcRetVal = strstr( pcString, pcSubString );
  
  if ( pcRetVal == NULL ) {
    return Return(AMXNULLP);
  }
  iRetVal = pcRetVal - pcString;
  return Return(iRetVal);
  // if-else

}  // amx_strstr()




/* amx_strstrx ( string, substring ) 
 *
 * modified implementation of char* strstr ( const char* string, const char* substring )
 * string   : string to search in
 * substring: substring to search for in string
 *
 * returns index of substring in array (zero-based) 
 * or AMXNULLP if the substring is not found in the string
 * an empty substring cannot be found in a non-empty string
 *
 */

static cell AMX_NATIVE_CALL amx_strstrx( AMX* amx, cell* params )
{

  int err = 0;
  const int iNumArgs = params[0] / sizeof(cell);
  int iRetVal = 0;
  size_t iStringLen = 0;
  size_t iSubStringLen = 0;

  char* pcString = NULL;
  char* pcSubString = NULL;
  const char* pcRetVal = NULL;


  const int REQNUMARGS = 2;

  if ( iNumArgs != REQNUMARGS ) {
    NATIVE_ERROR;
    return Return(AMXFAIL);
  }  // if

  /* get the haystack */
  err = get_string( amx, &pcString, params[1], &iStringLen, 0L );
  if ( err != AMX_ERR_NONE ) {
    RAISE_ERROR(err);
    return Return(AMXFAIL);
  }  // if

  /* get the needle */
  err = get_string( amx, &pcSubString, params[2], &iSubStringLen, 0L );
  if ( err != AMX_ERR_NONE ) {
    RAISE_ERROR(err);
    return Return(AMXFAIL);
  }  // if

  if ( iStringLen != 0 && iSubStringLen == 0 ) {
    return Return(AMXNULLP);
  }  // if

  /* call the standard string function */
  pcRetVal = strstr( pcString, pcSubString );
  
  if ( pcRetVal == NULL ) {
    return Return(AMXNULLP);
  }
  iRetVal = pcRetVal - pcString;
  return Return(iRetVal);
  // if-else

}  // amx_strstrx()




/* amx_strcasestr ( string, substring ) 
 *
 * implements case insensitive char* strstr ( const char* string, const char* substring )
 * string   : string to search in
 * substring: substring to search for in string
 *
 * returns index of substring in array (zero-based) 
 * or AMXNULLP if the substring is not found in the string
 *
 */

static cell AMX_NATIVE_CALL amx_strcasestr( AMX* amx, cell* params )
{

  int err = 0;
  const int iNumArgs = params[0] / sizeof(cell);
  int iRetVal = 0;

  char* pcString = NULL;
  char* pcSubString = NULL;
  char* pcRetVal = NULL;


  const int REQNUMARGS = 2;

  if ( iNumArgs != REQNUMARGS ) {
    NATIVE_ERROR;
    return Return(AMXFAIL);
  }  // if

  /* get the haystack */
  err = get_string( amx, &pcString, params[1], NULL, 0L );
  if ( err != AMX_ERR_NONE ) {
    RAISE_ERROR(err);
    return Return(AMXFAIL);
  }  // if

  /* get the needle */
  err = get_string( amx, &pcSubString, params[2], NULL, 0L );
  if ( err != AMX_ERR_NONE ) {
    RAISE_ERROR(err);
    return Return(AMXFAIL);
  }  // if

  /* transform the both strings into lower case */
  pcRetVal = pcString;
  while ( *pcRetVal != 0 ) {
    *pcRetVal = tolower( *pcRetVal );
    pcRetVal++;
  }  // while
  pcRetVal = pcSubString;
  while ( *pcRetVal != 0 ) {
    *pcRetVal = tolower( *pcRetVal );
    pcRetVal++;
  }  // while

  /* call the standard string function */
  pcRetVal = strstr( pcString, pcSubString );
  
  if ( pcRetVal == NULL ) {
    return Return(AMXNULLP);
  }
  iRetVal = pcRetVal - pcString;
  return Return(iRetVal);
  // if-else

}  // amx_strcasestr()




/* amx_strcasestrx ( string, substring ) 
 *
 * modified implementation of case insensitive char* strstr ( const char* string, const char* substring )
 * string   : string to search in
 * substring: substring to search for in string
 *
 * returns index of substring in array (zero-based) 
 * or AMXNULLP if the substring is not found in the string
 * an empty substring cannot be found in a non-empty string
 *
 */

static cell AMX_NATIVE_CALL amx_strcasestrx( AMX* amx, cell* params )
{

  int err = 0;
  const int iNumArgs = params[0] / sizeof(cell);
  int iRetVal = 0;
  size_t iStringLen = 0;
  size_t iSubStringLen = 0;

  char* pcString = NULL;
  char* pcSubString = NULL;
  char* pcRetVal = NULL;


  const int REQNUMARGS = 2;

  if ( iNumArgs != REQNUMARGS ) {
    NATIVE_ERROR;
    return Return(AMXFAIL);
  }  // if

  /* get the haystack */
  err = get_string( amx, &pcString, params[1], &iStringLen, 0L );
  if ( err != AMX_ERR_NONE ) {
    RAISE_ERROR(err);
    return Return(AMXFAIL);
  }  // if

  /* get the needle */
  err = get_string( amx, &pcSubString, params[2], &iSubStringLen, 0L );
  if ( err != AMX_ERR_NONE ) {
    RAISE_ERROR(err);
    return Return(AMXFAIL);
  }  // if

  if ( iStringLen != 0 && iSubStringLen == 0 ) {
    return Return(AMXNULLP);
  }  // if

  /* transform the both strings into lower case */
  pcRetVal = pcString;
  while ( *pcRetVal != 0 ) {
    *pcRetVal = tolower( *pcRetVal );
    pcRetVal++;
  }  // while
  pcRetVal = pcSubString;
  while ( *pcRetVal != 0 ) {
    *pcRetVal = tolower( *pcRetVal );
    pcRetVal++;
  }  // while

  /* call the standard string function */
  pcRetVal = strstr( pcString, pcSubString );
  
  if ( pcRetVal == NULL ) {
    return Return(AMXNULLP);
  }
  iRetVal = pcRetVal - pcString;
  return Return(iRetVal);
  // if-else

}  // amx_strcasestrx()






/* amx_strspn ( string, skipset ) 
 *
 * implements size_t strspn ( const char* string, const char* skipset )
 * string  : string to search in
 * skipset : set of characters to search for
 *
 * returns length of initial substring of string consisting entirely 
 * of characters from skipset.
 *
 */

static cell AMX_NATIVE_CALL amx_strspn( AMX* amx, cell* params )
{

  int err = 0;
  const int iNumArgs = params[0] / sizeof(cell);
  size_t tSubStringLen = 0;

  char* pcString = NULL;
  char* pcSkipSet = NULL;


  const int REQNUMARGS = 2;

  if ( iNumArgs != REQNUMARGS ) {
    NATIVE_ERROR;
    return Return(AMXFAIL);
  }  // if

  /* get the string */
  err = get_string( amx, &pcString, params[1], NULL, 0L );
  if ( err != AMX_ERR_NONE ) {
    RAISE_ERROR(err);
    return Return(AMXFAIL);
  }  // if

  /* get the skipset */
  err = get_string( amx, &pcSkipSet, params[2], NULL, 0L );
  if ( err != AMX_ERR_NONE ) {
    RAISE_ERROR(err);
    return Return(AMXFAIL);
  }  // if


  /* call the standard string function */
  tSubStringLen = strspn( pcString, pcSkipSet );
  
  return Return(tSubStringLen);


}  // amx_strspn()





/* amx_strcspn ( string, stopset ) 
 *
 * implements size_t strcspn ( const char* string, const char* stopset )
 * string  : string to search in
 * skipset : set of characters to search for
 *
 * returns length of initial substring of string consisting entirely 
 * of characters that are _not_ members of skipset.
 *
 */

static cell AMX_NATIVE_CALL amx_strcspn( AMX* amx, cell* params )
{

  int err = 0;
  const int iNumArgs = params[0] / sizeof(cell);
  size_t tSubStringLen = 0;

  char* pcString = NULL;
  char* pcStopSet = NULL;


  const int REQNUMARGS = 2;

  if ( iNumArgs != REQNUMARGS ) {
    NATIVE_ERROR;
    return Return(AMXFAIL);
  }  // if

  /* get the string */
  err = get_string( amx, &pcString, params[1], NULL, 0L );
  if ( err != AMX_ERR_NONE ) {
    RAISE_ERROR(err);
    return Return(AMXFAIL);
  }  // if

  /* get the stopset */
  err = get_string( amx, &pcStopSet, params[2], NULL, 0L );
  if ( err != AMX_ERR_NONE ) {
    RAISE_ERROR(err);
    return Return(AMXFAIL);
  }  // if


  /* call the standard string function */
  tSubStringLen = strcspn( pcString, pcStopSet );
  
  return Return(tSubStringLen);


}  // amx_strcspn()



/* amx_strtok ( string, delimiters, token, tokenlength ) 
 *
 * implements char* strtok ( const char* string, const char* delimiters )
 * string     : the string to be tokenized
 * delimiters : a set of characters that surround tokens as delimiters
 * token      : the string to hold the returned token
 * tokenlength: the maximum length of the token to be returned
 *
 * returns the length of the token found and the token in token
 * or AMXNULLP if no more tokens are found
 *
 */

static cell AMX_NATIVE_CALL amx_strtok( AMX* amx, cell* params )
{

  int err = 0;
  const int iNumArgs = params[0] / sizeof(cell);
  int iRetVal = 0;

  size_t tStringLen = 0;

  char* pcString = NULL;
  char* pcDelimiters = NULL;
  char* pcTokenString = NULL;
  char* pcRetVal = NULL;


  const int REQNUMARGS = 4;

  if ( iNumArgs != REQNUMARGS ) {
    NATIVE_ERROR;
    return Return(AMXFAIL);
  }  // if

  /* clear the token */
  err = clear_string( amx, params[3], 0L );
  if ( err != AMX_ERR_NONE ) {
    RAISE_ERROR(err);
    return Return(AMXFAIL);
  }  // if
  
  /* check if this is an intialising or a subsequent call */
  err = string_len( amx, params[1], &tStringLen );
  if ( err != AMX_ERR_NONE ) {
    RAISE_ERROR(err);
    return Return(AMXFAIL);
  }  // if
  
  /* if this is a subsequent call without a intial call return an error */
  if ( tStringLen == 0 && g_cTokenStringInitialized == 0 ) {
    //NATIVE_ERROR;
    return Return(AMXFAIL);
  }  // if



  if ( tStringLen > 0 ) { /* initial call */

    /* if the token string is too large for the TokenBuffer return an error */
    if ( tStringLen >= TOKEN_STRING_LEN ) {
      NATIVE_ERROR;
      return Return(AMXFAIL);
    }  // if

    /* get the token string */
    err = get_string( amx, &pcString, params[1], NULL, 0L );
    if ( err != AMX_ERR_NONE ) {
      RAISE_ERROR(err);
      return Return(AMXFAIL);
    }  // if

    /* put the token string in our static buffer */
    memset( g_acTokenString, 0, TOKEN_STRING_LEN);
    memcpy( g_acTokenString, pcString, tStringLen );
    g_cTokenStringInitialized = 1;
    pcTokenString = g_acTokenString;
    g_pcTokenStringPos = g_acTokenString;
    g_pcTokenStringEnd = g_pcTokenStringPos + strlen( g_acTokenString );

  } else {  /* subsequent call */

    pcTokenString = g_pcTokenStringPos;

  }  // if-else

  /* get the delimiters */
  err = get_string( amx, &pcDelimiters, params[2], NULL, 0L );
  if ( err != AMX_ERR_NONE ) {
    RAISE_ERROR(err);
    return Return(AMXFAIL);
  }  // if


  /* call the standard string function */
  pcRetVal = strtok( pcTokenString, pcDelimiters );
  
  if ( pcRetVal == NULL ) {
    /* no tokens are left */
    g_cTokenStringInitialized = 0;
    memset( g_acTokenString, 0, TOKEN_STRING_LEN);
    // set_string( amx, params[3], g_acTokenString, 0 );
    g_pcTokenStringPos = NULL;
    g_pcTokenStringEnd = g_acTokenString;
    return Return(AMXNULLP);

  }
  /* set the return string and return the length of the token */
  set_string( amx, params[3], pcRetVal, params[4] );
  iRetVal = strlen( pcRetVal );

  /* move the static position pointer to point after the token */
  g_pcTokenStringPos += strlen( g_pcTokenStringPos );
  g_pcTokenStringPos++; /* blame strtok */
  /* move it beyond any delimiter characters */
  // g_pcTokenStringPos += strspn( g_pcTokenStringPos, pcDelimiters );
  if ( g_pcTokenStringPos >= g_pcTokenStringEnd ) {
	  g_pcTokenStringPos = NULL;
	  g_pcTokenStringEnd = g_acTokenString;
	  g_cTokenStringInitialized = 0;
  }  // if

  return Return(iRetVal);
  // if-else

}  // amx_strtok()



/* amx_strtokrest ( string, maxlen )
 *
 * Returns whatever is left from a string after subsequent calls
 * to strtok on this string, empty string if nothing is left.
 *
 * string : the string to return the leftovers in
 * maxlen : maximum length of characters returned.
 *
 * Returns the length of the returned string
 * or AMXNULLP if no string is returned
 *
 */

static cell AMX_NATIVE_CALL amx_strtokrest( AMX* amx, cell* params )
{
	const int iNumArgs = params[0] / sizeof(cell);
  int iRetVal = 0;


  const int REQNUMARGS = 2;

  if ( iNumArgs != REQNUMARGS ) {
    NATIVE_ERROR;
    return Return(AMXFAIL);
  }  // if

  clear_string( amx, params[1], 0L );

  /* if this is a call without an intial call to strtok() return an error */
  if ( g_cTokenStringInitialized == 0 ) {
    //NATIVE_ERROR;
    return Return(AMXFAIL);
  }  // if

  if ( g_pcTokenStringPos == NULL ) {
    /* no rest is left */
    return Return(AMXNULLP);

  }
	/* set the return string and return the length of the token */
	set_string( amx, params[1], g_pcTokenStringPos, params[2] );
	iRetVal = strlen( g_pcTokenStringPos );

	return Return(iRetVal);
	// if-else

}  // amx_strtokrest()





/* amx_strgtok ( string, delimiters, grouping, token, tokenlength ) 
 *
 * implements char* strgtok ( const char* string, const char* delimiters )
 * augemnted with a set of grouping characters
 *
 * string     : the string to be tokenized
 * delimiters : a set of characters that surround tokens as delimiters
 * grouping   : a set of characters that group delimited tokens together
 * token      : the string to hold the returned token
 * tokenlength: the maximum length of the token to be returned
 *
 * returns the length of the token found and the token in token
 * or AMXNULLP if no more tokens are found
 *
 */

static cell AMX_NATIVE_CALL amx_strgtok( AMX* amx, cell* params )
{

  int err = 0;
  const int iNumArgs = params[0] / sizeof(cell);
  int iRetVal = 0;

  size_t tStringLen = 0;

  char* pcString = NULL;
  char* pcDelimiters = NULL;
  char* pcGrouping = NULL;
  char* pcTokenString = NULL;
  char* pcRetVal = NULL;


  const int REQNUMARGS = 5;

  if ( iNumArgs != REQNUMARGS ) {
    NATIVE_ERROR;
    return Return(AMXFAIL);
  }  // if

  /* clear the token */
  err = clear_string( amx, params[4], 0L );
  if ( err != AMX_ERR_NONE ) {
    RAISE_ERROR(err);
    return Return(AMXFAIL);
  }  // if
  
  /* check if this is an intialising or a subsequent call */
  err = string_len( amx, params[1], &tStringLen );
  if ( err != AMX_ERR_NONE ) {
    RAISE_ERROR(err);
    return Return(AMXFAIL);
  }  // if
  
  /* if this is a subsequent call without a intial call return an error */
  if ( tStringLen == 0 && g_cGTokenStringInitialized == 0 ) {
    //NATIVE_ERROR;
    return Return(AMXFAIL);
  }  // if



  if ( tStringLen > 0 ) { /* initial call */

    /* if the token string is too large for the TokenBuffer return an error */
    if ( tStringLen >= GTOKEN_STRING_LEN ) {
      NATIVE_ERROR;
      return Return(AMXFAIL);
    }  // if

    /* get the token string */
    err = get_string( amx, &pcString, params[1], NULL, 0L );
    if ( err != AMX_ERR_NONE ) {
      RAISE_ERROR(err);
      return Return(AMXFAIL);
    }  // if

    /* put the token string in our static buffer */
    memset( g_acGTokenString, 0, GTOKEN_STRING_LEN);
    memcpy( g_acGTokenString, pcString, tStringLen );
    g_cGTokenStringInitialized = 1;
    pcTokenString = g_acGTokenString;
    g_pcGTokenStringPos = g_acGTokenString;
    g_pcGTokenStringEnd = g_pcGTokenStringPos + strlen( g_acGTokenString );

  } else {  /* subsequent call */

    //pcTokenString = g_pcGTokenStringPos;
    pcTokenString = NULL;
  }  // if-else

  /* get the delimiters */
  err = get_string( amx, &pcDelimiters, params[2], NULL, 0L );
  if ( err != AMX_ERR_NONE ) {
    RAISE_ERROR(err);
    return Return(AMXFAIL);
  }  // if

  /* get the grouping */
  err = get_string( amx, &pcGrouping, params[3], NULL, 0L );
  if ( err != AMX_ERR_NONE ) {
    RAISE_ERROR(err);
    return Return(AMXFAIL);
  }  // if


  /* call our enhanced string function */
  pcRetVal = _strgtok( pcTokenString, pcDelimiters, pcGrouping );
  
  if ( pcRetVal == NULL ) {
    /* no tokens are left */
    g_cGTokenStringInitialized = 0;
    memset( g_acGTokenString, 0, GTOKEN_STRING_LEN);
    // set_string( amx, params[4], g_acGTokenString, 0 );
    g_pcGTokenStringPos = NULL;
    g_pcGTokenStringEnd = g_acGTokenString;
    return Return(AMXNULLP);

  }
  /* set the return string and return the length of the token */
  set_string( amx, params[4], pcRetVal, params[5] );
  iRetVal = strlen( pcRetVal );

  /* move the static position pointer to point after the token */
  g_pcGTokenStringPos += strlen( g_pcGTokenStringPos );
  g_pcGTokenStringPos++; /* blame strtok */
  /* move it beyond any delimiter characters */
  // g_pcTokenStringPos += strspn( g_pcTokenStringPos, pcDelimiters );
  if ( g_pcGTokenStringPos >= g_pcGTokenStringEnd ) {
	  g_pcGTokenStringPos = NULL;
	  g_pcGTokenStringEnd = g_acGTokenString;
	  g_cGTokenStringInitialized = 0;
  }  // if

  return Return(iRetVal);
  // if-else

}  // amx_strgtok()



/* amx_strgtokrest ( string, maxlen )
 *
 * Returns whatever is left from a string after subsequent calls
 * to strtok on this string, empty string if nothing is left.
 *
 * string : the string to return the leftovers in
 * maxlen : maximum length of characters returned.
 *
 * Returns the length of the returned string
 * or AMXNULLP if no string is returned
 *
 */

static cell AMX_NATIVE_CALL amx_strgtokrest( AMX* amx, cell* params )
{
	const int iNumArgs = params[0] / sizeof(cell);
  int iRetVal = 0;


  const int REQNUMARGS = 2;

  if ( iNumArgs != REQNUMARGS ) {
    NATIVE_ERROR;
    return Return(AMXFAIL);
  }  // if

  clear_string( amx, params[1], 0L );

  /* if this is a call without an intial call to strtok() return an error */
  if ( g_cGTokenStringInitialized == 0 ) {
    //NATIVE_ERROR;
    return Return(AMXFAIL);
  }  // if

  if ( g_pcGTokenStringPos == NULL ) {
    /* no rest is left */
    return Return(AMXNULLP);

  }
	/* set the return string and return the length of the token */
	set_string( amx, params[1], g_pcGTokenStringPos, params[2] );
	iRetVal = strlen( g_pcGTokenStringPos );

	return Return(iRetVal);
	// if-else

}  // amx_strgtokrest()





/* amx_strsplit ( string, delimiters, token, tokenlength, ... ) 
 *
 * Break up a string into separate tokens.
 * string     : the string to be tokenized
 * delimiters : a set of characters that surround tokens as delimiters
 * token      : the string to hold the returned token
 * tokenlength: the maximum length of the token to be returned
 *
 * returns the number of tokens found
 * or AMXNULLP if no tokens are found
 *
 */

static cell AMX_NATIVE_CALL amx_strsplit( AMX* amx, cell* params )
{

  int err = 0;
  const int iNumArgs = params[0] / sizeof(cell);
  int iNumTokens = 0;
  int iNextToken = 3;

  char* pcString = NULL;
  char* pcDelimiters = NULL;
  char* pcRetVal = NULL;

  cell* ptCell = NULL;

  const int REQNUMARGS = 4;

  if ( iNumArgs < REQNUMARGS ) {
    NATIVE_ERROR;
    return Return(AMXFAIL);
  }  // if

  /* check if we have a length for each token */
  if ( iNumArgs % 2 ) {
    NATIVE_ERROR;
    return Return(AMXFAIL);
  }  // if
  

  /* get the token string */
  err = get_string( amx, &pcString, params[1], NULL, 0L );
  if ( err != AMX_ERR_NONE ) {
    RAISE_ERROR(err);
    return Return(AMXFAIL);
  }  // if
  

  /* get the delimiters */
  err = get_string( amx, &pcDelimiters, params[2], NULL, 0L );
  if ( err != AMX_ERR_NONE ) {
    RAISE_ERROR(err);
    return Return(AMXFAIL);
  }  // if


  /* initial call to strtok */
  pcRetVal = strtok( pcString, pcDelimiters );
  
  while ( pcRetVal != NULL && iNextToken < iNumArgs) {

    /* get the memory address of the Small string size reference */
    err = amx_GetAddr( amx, params[iNextToken+1], &ptCell );
    if ( err != AMX_ERR_NONE ) {
      RAISE_ERROR(err);
      return Return(AMXFAIL);
    }  // if 

    // set string to token but with maximum length -1 (for '\0') 
    set_string( amx, params[iNextToken], pcRetVal, ((size_t)*ptCell)-1 );
    iNextToken += 2;
    iNumTokens++;
    /* subsequent call to strtok */
    pcRetVal = strtok( NULL, pcDelimiters );

  } // while

  return Return(iNumTokens);

}  // amx_strsplit()




/* amx_strgsplit ( string, delimiters, grouping, token, tokenlength, ... ) 
 *
 * Break up a string into separate tokens.
 * string     : the string to be tokenized
 * delimiters : a set of characters that surround tokens as delimiters
 * grouping   : a set of characters that surround tokens which contain delimiters
 * token      : the string to hold the returned token
 * tokenlength: the maximum length of the token to be returned
 *
 * returns the number of tokens found
 * or AMXNULLP if no tokens are found
 *
 */

static cell AMX_NATIVE_CALL amx_strgsplit( AMX* amx, cell* params )
{

  int err = 0;
  int iNumArgs = params[0] / sizeof(cell);
  int iNumTokens = 0;
  int iNextToken = 4;

  size_t tStringSize = 0;

  char* pcString = NULL;
  char* pcDelimiters = NULL;
  char* pcGroupingSet = NULL;
  char* pcRetVal = NULL;

  char* pcGrouper = NULL;
  char* pcStart = NULL;
  char* pcEndOfString = NULL;

  char  cInGroup = 0;
  char  cLastGrouper = 0;

  cell* ptCell = NULL;

  const int REQNUMARGS = 5;

  if ( iNumArgs < REQNUMARGS ) {
    NATIVE_ERROR;
    return Return(AMXFAIL);
  }  // if

  /* check if we have a length for each token */
  if ( ! (iNumArgs % 2) ) {
    NATIVE_ERROR;
    return Return(AMXFAIL);
  }  // if
  
#ifdef DEBUG
  fprintf( stderr, "Args: %d\n", iNumArgs );
#endif

  /* get the string */
  err = get_string( amx, &pcString, params[1], &tStringSize, 0L );
  if ( err != AMX_ERR_NONE ) {
    RAISE_ERROR(err);
    return Return(AMXFAIL);
  }  // if
  
  pcEndOfString = pcString + tStringSize;

  /* get the delimiters */
  err = get_string( amx, &pcDelimiters, params[2], NULL, 0L );
  if ( err != AMX_ERR_NONE ) {
    RAISE_ERROR(err);
    return Return(AMXFAIL);
  }  // if


  /* get the grouping set */
  err = get_string( amx, &pcGroupingSet, params[3], NULL, 0L );
  if ( err != AMX_ERR_NONE ) {
    RAISE_ERROR(err);
    return Return(AMXFAIL);
  }  // if


  pcStart = pcString;

  while ( (iNextToken < iNumArgs) && (pcStart < pcEndOfString) ) {
    /* check for a group */
    pcGrouper = strpbrk( pcStart, pcGroupingSet );

    if ( pcGrouper == NULL ) {
      /* we just carrry on like in strsplit because there are no groups */

#ifdef DEBUG
      fprintf( stderr, "No more groups\n" );
#endif

      /* initial call to strtok */
      pcRetVal = strtok( pcStart, pcDelimiters );
      
      while ( pcRetVal != NULL && iNextToken < iNumArgs) {
	
	/* get the memory address of the Small string size reference */
	err = amx_GetAddr( amx, params[iNextToken+1], &ptCell );
	if ( err != AMX_ERR_NONE ) {
	  RAISE_ERROR(err);
	  return Return(AMXFAIL);
	}  // if
	
#ifdef DEBUG
	fprintf( stderr, "token %d: %s\n", iNumTokens, pcRetVal );
	fprintf( stderr, "set_string( amx, params[%d]=%d, %s, *params[%d]=%d\n", iNextToken,  params[iNextToken], pcRetVal, iNextToken+1, (int)*ptCell );
#endif
	
	/* set string to token but with maximum length -1 (for '\0') */
	set_string( amx, params[iNextToken], pcRetVal, ((size_t)*ptCell)-1 );
	iNextToken += 2;
	iNumTokens++;
	/* subsequent call to strtok */
	pcRetVal = strtok( NULL, pcDelimiters );
	
      } // while
      
      return Return(iNumTokens);

    }
    if ( pcGrouper == pcStart ) {
      
#ifdef DEBUG
      fprintf( stderr, "Start of group\n" );
#endif

	    /* a group starts */
	    cInGroup = 1;

	    /* set the start to the start of the group */
	    pcStart++;

	    /* get the end of the group */
	    pcGrouper = strpbrk( pcStart, pcGroupingSet );
	    if ( pcGrouper == NULL ) {
		    pcGrouper = pcEndOfString;
	    } else {
		    cLastGrouper = *pcGrouper;
		    *pcGrouper = 0;
	    }  // if-else
      

	    /* get the memory address of the Small string size reference */
	    err = amx_GetAddr( amx, params[iNextToken+1], &ptCell );
	    if ( err != AMX_ERR_NONE ) {
		    RAISE_ERROR(err);
		    return Return(AMXFAIL);
	    }  // if
	
#ifdef DEBUG
	fprintf( stderr, "token %d: %s\n", iNumTokens, pcStart );
	fprintf( stderr, "set_string( amx, params[%d]=%d, %s, *params[%d]=%d\n", iNextToken,  params[iNextToken], pcStart, iNextToken+1, (int)*ptCell );
#endif
	
	    /* set the Small string */
	    set_string( amx, params[iNextToken], pcStart, ((size_t)*ptCell)-1 );
	    iNextToken += 2;
	    iNumTokens++;
	
	    if ( pcGrouper != pcEndOfString ) {
		    *pcGrouper = cLastGrouper;
	    }  // if
	    pcStart = pcGrouper + 1;
	    if ( pcStart >= pcEndOfString ) return Return(iNumTokens);
	
    } else if ( pcGrouper > pcStart ) {  
      
#ifdef DEBUG
      fprintf( stderr, "Tokenizing before group\n" );
#endif

	    /* tokenize the part before the group */
	    cLastGrouper = *pcGrouper;
	    *pcGrouper = 0;
	    /* initial call to strtok */
	    pcRetVal = strtok( pcStart, pcDelimiters );
      
	    while ( pcRetVal != NULL && iNextToken < iNumArgs) {
	
		    /* get the memory address of the Small string size reference */
		    err = amx_GetAddr( amx, params[iNextToken+1], &ptCell );
		    if ( err != AMX_ERR_NONE ) {
			    RAISE_ERROR(err);
			    return Return(AMXFAIL);
		    }  // if 
	
#ifdef DEBUG
	fprintf( stderr, "token %d: %s\n", iNumTokens, pcRetVal );
	fprintf( stderr, "set_string( amx, params[%d]=%d, %s, *params[%d]=%d\n", iNextToken,  params[iNextToken], pcRetVal, iNextToken+1, (int)*ptCell );
#endif
	
		    set_string( amx, params[iNextToken], pcRetVal, ((size_t)*ptCell)-1 );
		    iNextToken += 2;
		    iNumTokens++;
		    /* subsequent call to strtok */
		    pcRetVal = strtok( NULL, pcDelimiters );
	
	    } // while
      
	    /* carry on with the group */
	    *pcGrouper = cLastGrouper;
	    pcStart = pcGrouper;
	    if ( pcStart >= pcEndOfString ) return Return(iNumTokens);

    } else {
	    /* this case should not occur */
	    return Return(iNumTokens);
    }
    // if-else
    
  }  // while
  

  return Return(iNumTokens);
}  // amx_strgsplit()



/* amx_strsep ( string, delimiters, token, tokenlength, ..., rest, restlength ) 
 *
 * Break up a string into separate tokens. The last string parameter
 * will hold the rest of the string.
 *
 * string     : the string to be tokenized
 * delimiters : a set of characters that surround tokens as delimiters
 * token      : the string to hold the returned token
 * tokenlength: the maximum length of the token to be returned
 * rest       : the rest of the string after tokenization
 * restlength : the maximum length of the rest to be returned
 *
 * returns the number of tokens found
 * or AMXNULLP if no tokens are found
 *
 */

static cell AMX_NATIVE_CALL amx_strsep( AMX* amx, cell* params )
{

  int err = 0;
  const int iNumArgs = params[0] / sizeof(cell);
  int iNumTokens = 0;
  int iNextToken = 3;

  size_t tStringLen = 0;

  char* pcString = NULL;
  char* pcDelimiters = NULL;
  char* pcRest = NULL;
  char* pcRetVal = NULL;

  cell* ptCell = NULL;

  /* why REQNUMARGS of 6? If you don't want to have the 
     rest returned you should really use strsplit() */
  const int REQNUMARGS = 6;

  if ( iNumArgs < REQNUMARGS ) {
    NATIVE_ERROR;
    return Return(AMXFAIL);
  }  // if

  /* check if we have a length for each token */
  if ( iNumArgs % 2 ) {
    NATIVE_ERROR;
    return Return(AMXFAIL);
  }  // if
  

  /* get the token string */
  err = get_string( amx, &pcString, params[1], NULL, 0L );
  if ( err != AMX_ERR_NONE ) {
    RAISE_ERROR(err);
    return Return(AMXFAIL);
  }  // if
  
  tStringLen = strlen( pcString );

  /* get the delimiters */
  err = get_string( amx, &pcDelimiters, params[2], NULL, 0L );
  if ( err != AMX_ERR_NONE ) {
    RAISE_ERROR(err);
    return Return(AMXFAIL);
  }  // if


  /* initial call to strtok */
  pcRetVal = strtok( pcString, pcDelimiters );
  
  while ( pcRetVal != NULL && iNextToken < (iNumArgs - 2) ) {

    pcRest = pcRetVal + strlen(pcRetVal) + 1;
    pcRest += strspn( pcRest, pcDelimiters );
    if ( pcRest >= pcString + tStringLen ) pcRest = NULL;

    /* get the memory address of the Small string size reference */
    err = amx_GetAddr( amx, params[iNextToken+1], &ptCell );
    if ( err != AMX_ERR_NONE ) {
      RAISE_ERROR(err);
      return Return(AMXFAIL);
    }  // if 

    // set string to token but with maximum length -1 (for '\0') 
    set_string( amx, params[iNextToken], pcRetVal, ((size_t)*ptCell)-1 );
    iNextToken += 2;
    iNumTokens++;
    /* subsequent call to strtok */
    if ( iNextToken < (iNumArgs -2) ) {
      pcRetVal = strtok( NULL, pcDelimiters );
    }  // if

  } // while


  /* get the memory address of the Small string size reference */
  err = amx_GetAddr( amx, params[iNumArgs], &ptCell );
  if ( err != AMX_ERR_NONE ) {
    RAISE_ERROR(err);
    return Return(AMXFAIL);
  }  // if 
  if ( pcRest != NULL ) {
    set_string( amx, params[iNumArgs-1], pcRest, ((size_t)*ptCell)-1 );
    pcRest = NULL;
  } else {
    set_string( amx, params[iNumArgs-1], "", ((size_t)*ptCell)-1 );
  }  // if-else

  return Return(iNumTokens);

}  // amx_strsep()




/* amx_strgsep ( string, delimiters, grouping, token, tokenlength, ..., rest, restlength ) 
 *
 * Break up a string into separate tokens. The last string parameter
 * will hold the rest of the string.
 *
 * string     : the string to be tokenized
 * delimiters : a set of characters that surround tokens as delimiters
 * grouping   : a set of characters that surround tokens which contain delimiters
 * token      : the string to hold the returned token
 * tokenlength: the maximum length of the token to be returned
 * rest       : the rest of string after the tokenization
 * restlength : the maximum length of the rest to be returned
 *
 * returns the number of tokens found
 * or AMXNULLP if no tokens are found
 *
 */

static cell AMX_NATIVE_CALL amx_strgsep( AMX* amx, cell* params )
{

  int err = 0;
  int iNumArgs = params[0] / sizeof(cell);
  int iNumTokens = 0;
  int iNextToken = 4;

  size_t tStringSize = 0;

  char* pcString = NULL;
  char* pcDelimiters = NULL;
  char* pcGroupingSet = NULL;
  char* pcRest = NULL;
  char* pcRetVal = NULL;

  char* pcGrouper = NULL;
  char* pcStart = NULL;
  char* pcEndOfString = NULL;

  char  cInGroup = 0;
  char  cLastGrouper = 0;

  cell* ptCell = NULL;

  /* Why REQNUMARGS of 7? If you don't want
     the rest returned, you should use strsep() */
  const int REQNUMARGS = 7;

  if ( iNumArgs < REQNUMARGS ) {
    NATIVE_ERROR;
    return Return(AMXFAIL);
  }  // if

  /* check if we have a length for each token */
  if ( ! (iNumArgs % 2) ) {
    NATIVE_ERROR;
    return Return(AMXFAIL);
  }  // if
  
#ifdef DEBUG
  fprintf( stderr, "Args: %d\n", iNumArgs );
#endif

  /* get the string */
  err = get_string( amx, &pcString, params[1], &tStringSize, 0L );
  if ( err != AMX_ERR_NONE ) {
    RAISE_ERROR(err);
    return Return(AMXFAIL);
  }  // if
  
  pcEndOfString = pcString + tStringSize;

  /* get the delimiters */
  err = get_string( amx, &pcDelimiters, params[2], NULL, 0L );
  if ( err != AMX_ERR_NONE ) {
    RAISE_ERROR(err);
    return Return(AMXFAIL);
  }  // if


  /* get the grouping set */
  err = get_string( amx, &pcGroupingSet, params[3], NULL, 0L );
  if ( err != AMX_ERR_NONE ) {
    RAISE_ERROR(err);
    return Return(AMXFAIL);
  }  // if


  pcStart = pcString;

  while ( (iNextToken < (iNumArgs-2)) && (pcStart < pcEndOfString) ) {
    /* check for a group */
    pcGrouper = strpbrk( pcStart, pcGroupingSet );

    if ( pcGrouper == NULL ) {
      /* we just carrry on like in strsplit because there are no groups */

#ifdef DEBUG
      fprintf( stderr, "No more groups\n" );
#endif

      /* initial call to strtok */
      pcRetVal = strtok( pcStart, pcDelimiters );
      
      while ( pcRetVal != NULL && iNextToken < (iNumArgs - 2) ) {
	
	pcRest = pcRetVal + strlen(pcRetVal) + 1;
	pcRest += strspn( pcRest, pcDelimiters );
	if ( pcRest >= pcEndOfString ) pcRest = NULL;

	/* get the memory address of the Small string size reference */
	err = amx_GetAddr( amx, params[iNextToken+1], &ptCell );
	if ( err != AMX_ERR_NONE ) {
	  RAISE_ERROR(err);
	  return Return(AMXFAIL);
	}  // if
	
#ifdef DEBUG
	fprintf( stderr, "token %d: %s\n", iNumTokens, pcRetVal );
	fprintf( stderr, "set_string( amx, params[%d]=%d, %s, *params[%d]=%d\n", iNextToken,  params[iNextToken], pcRetVal, iNextToken+1, (int)*ptCell );
#endif
	
	/* set string to token but with maximum length -1 (for '\0') */
	set_string( amx, params[iNextToken], pcRetVal, ((size_t)*ptCell)-1 );
	iNextToken += 2;
	iNumTokens++;
	/* subsequent call to strtok */
	if ( iNextToken < (iNumArgs - 2) ) {
	  pcRetVal = strtok( NULL, pcDelimiters );
	}  // if

      } // while
      

      /* get the memory address of the Small string size reference */
      err = amx_GetAddr( amx, params[iNumArgs], &ptCell );
      if ( err != AMX_ERR_NONE ) {
	RAISE_ERROR(err);
	return Return(AMXFAIL);
      }  // if 
      /* return the rest of the string */
      if ( pcRest != NULL ) {	
	set_string( amx, params[iNumArgs-1], pcRest, ((size_t)*ptCell)-1 );
	pcRest = NULL;
      } else {
	set_string( amx, params[iNumArgs-1], "", ((size_t)*ptCell)-1 );
      }  // if-else

      return Return(iNumTokens);

    }
    if ( pcGrouper == pcStart ) {
      
#ifdef DEBUG
      fprintf( stderr, "Start of group\n" );
#endif

	    /* a group starts */
	    cInGroup = 1;

	    /* set the start to the start of the group */
	    pcStart++;

	    /* get the end of the group */
	    pcGrouper = strpbrk( pcStart, pcGroupingSet );
	    if ( pcGrouper == NULL ) {
		    pcGrouper = pcEndOfString;
		    pcRest = NULL;
	    } else {
		    cLastGrouper = *pcGrouper;
		    *pcGrouper = 0;
		    pcRest = pcGrouper + 1 ;
	    }  // if-else
      

	    /* get the memory address of the Small string size reference */
	    err = amx_GetAddr( amx, params[iNextToken+1], &ptCell );
	    if ( err != AMX_ERR_NONE ) {
		    RAISE_ERROR(err);
		    return Return(AMXFAIL);
	    }  // if
	
#ifdef DEBUG
	fprintf( stderr, "token %d: %s\n", iNumTokens, pcStart );
	fprintf( stderr, "set_string( amx, params[%d]=%d, %s, *params[%d]=%d\n", iNextToken,  params[iNextToken], pcStart, iNextToken+1, (int)*ptCell );
#endif
	
	    /* set the Small string */
	    set_string( amx, params[iNextToken], pcStart, ((size_t)*ptCell)-1 );
	    iNextToken += 2;
	    iNumTokens++;
	
	    if ( pcGrouper != pcEndOfString ) {
		    *pcGrouper = cLastGrouper;
	    }  // if
	    pcStart = pcGrouper + 1;

	    if ( pcStart >= pcEndOfString ) {
		    /* get the memory address of the Small string size reference */
		    err = amx_GetAddr( amx, params[iNumArgs], &ptCell );
		    if ( err != AMX_ERR_NONE ) {
			    RAISE_ERROR(err);
			    return Return(AMXFAIL);
		    }  // if 
		    /* return the rest of the string */
		    if ( pcRest != NULL ) {	
			    set_string( amx, params[iNumArgs-1], pcRest, ((size_t)*ptCell)-1 );
			    pcRest = NULL;
		    } else {
			    set_string( amx, params[iNumArgs-1], "", ((size_t)*ptCell)-1 );
		    }  // if-else

		    return Return(iNumTokens);
	    }  // if
	
    } else if ( pcGrouper > pcStart ) {  
      
#ifdef DEBUG
      fprintf( stderr, "Tokenizing before group\n" );
#endif

	    /* tokenize the part before the group */
	    cLastGrouper = *pcGrouper;
	    *pcGrouper = 0;
	    /* initial call to strtok */
	    pcRetVal = strtok( pcStart, pcDelimiters );
      
	    while ( pcRetVal != NULL && iNextToken < (iNumArgs - 2) ) {
	
		    pcRest = pcRetVal + strlen(pcRetVal) + 1;
		    pcRest += strspn( pcRest, pcDelimiters );
		    if ( pcRest >= pcEndOfString ) pcRest = NULL;

		    /* get the memory address of the Small string size reference */
		    err = amx_GetAddr( amx, params[iNextToken+1], &ptCell );
		    if ( err != AMX_ERR_NONE ) {
			    RAISE_ERROR(err);
			    return Return(AMXFAIL);
		    }  // if 
	
#ifdef DEBUG
	fprintf( stderr, "token %d: %s\n", iNumTokens, pcRetVal );
	fprintf( stderr, "set_string( amx, params[%d]=%d, %s, *params[%d]=%d\n", iNextToken,  params[iNextToken], pcRetVal, iNextToken+1, (int)*ptCell );
#endif
	
		    set_string( amx, params[iNextToken], pcRetVal, ((size_t)*ptCell)-1 );
		    iNextToken += 2;
		    iNumTokens++;
		    /* subsequent call to strtok */
		    if ( iNextToken < (iNumArgs - 2) ) {
			    pcRetVal = strtok( NULL, pcDelimiters );
		    }  // if

	    } // while
      
	    /* carry on with the group */
	    *pcGrouper = cLastGrouper;
	    pcStart = pcGrouper;
	    if ( pcStart >= pcEndOfString ) {
		    /* get the memory address of the Small string size reference */
		    err = amx_GetAddr( amx, params[iNumArgs], &ptCell );
		    if ( err != AMX_ERR_NONE ) {
			    RAISE_ERROR(err);
			    return Return(AMXFAIL);
		    }  // if 
		    /* return the rest of the string */
		    if ( pcRest != NULL ) {	
			    set_string( amx, params[iNumArgs-1], pcRest, ((size_t)*ptCell)-1 );
			    pcRest = NULL;
		    } else {
			    set_string( amx, params[iNumArgs-1], "", ((size_t)*ptCell)-1 );
		    }  // if-else

		    return Return(iNumTokens);
	    }  // if

    } else {
	    /* this case should not occur */
	    return Return(iNumTokens);
    }
    // if-else
    
  }  // while
  

  /* get the memory address of the Small string size reference */
  err = amx_GetAddr( amx, params[iNumArgs], &ptCell );
  if ( err != AMX_ERR_NONE ) {
    RAISE_ERROR(err);
    return Return(AMXFAIL);
  }  // if 
  /* return the rest of the string */
  if ( pcRest != NULL ) {	
    set_string( amx, params[iNumArgs-1], pcRest, ((size_t)*ptCell)-1 );
    pcRest = NULL;
  } else {
    set_string( amx, params[iNumArgs-1], "", ((size_t)*ptCell)-1 );
  }  // if-else

  return Return(iNumTokens);
}  // amx_strgsep()





/* amx_strcount ( string, char ) 
 *
 * Counts how often a character char occurs in a string
 *
 * string   : string to search in
 * character: character to search for in string
 *
 * returns number of occurences of character in string 
 * or 0 if the character is not found in the string
 *
 */

static cell AMX_NATIVE_CALL amx_strcount( AMX* amx, cell* params )
{

  int err = 0;
  const int iNumArgs = params[0] / sizeof(cell);
  size_t iCount = 0;
  int iChar = 0;
  int iRetVal = 0;

  
  char* pcString = NULL;
  const char* pcEndOfString = NULL;
  const char* pcRetVal = NULL;

  const int REQNUMARGS = 2;

  if ( iNumArgs != REQNUMARGS ) {
    NATIVE_ERROR;
    return Return(AMXFAIL);
  }  // if

  /* first get the source!! */
  err = get_string( amx, &pcString, params[1], &iCount, 0L );
  if ( err != AMX_ERR_NONE ) {
    RAISE_ERROR(err);
    return Return(AMXFAIL);
  }  // if

  iChar = params[2];
  pcEndOfString = pcString + iCount;

  iCount = 0;
  pcRetVal = strchr( pcString, iChar );
  while ( pcRetVal != NULL && (++pcRetVal <= pcEndOfString) ) {
    iCount++;
    pcRetVal = strchr( pcRetVal, iChar );
  }  // while
  
  iRetVal = iCount;
  return Return(iRetVal);

}  // amx_strcount()




/* amx_strtrim ( string, trim, where = 2 ) 
 *
 * Deletes characters from the set trim from the beginning (where=0),
 * end (where=1) or both (where=2) of a string.
 *
 * string : string to be trimmed
 * trim   : set of characters to trim from string
 * where  : specifies on which end the trimming should take place (0|1|2)
 *
 * returns number of characters deleted
 *
 */

static cell AMX_NATIVE_CALL amx_strtrim( AMX* amx, cell* params )
{

  int err = 0;
  const int iNumArgs = params[0] / sizeof(cell);
  size_t iRetVal = 0;
  size_t tiCharsTrimmed = 0;
  size_t tiStringLength = 0;
  size_t tiNumTrims = 0;

  char* pcString = NULL;
  char* pcTrim = NULL;
  char* pcPos = NULL;

  const int REQNUMARGS = 3;

  if ( iNumArgs != REQNUMARGS ) {
    NATIVE_ERROR;
    return Return(AMXFAIL);
  }  // if

  /* first get the string */
  err = get_string( amx, &pcString, params[1], &tiStringLength, 0L );
  if ( err != AMX_ERR_NONE ) {
    RAISE_ERROR(err);
    return Return(AMXFAIL);
  }  // if

  /* get the trim set */
  err = get_string( amx, &pcTrim, params[2], &tiNumTrims, 0L );
  if ( err != AMX_ERR_NONE ) {
    RAISE_ERROR(err);
    return Return(AMXFAIL);
  }  // if


  /* trim the end */
  if ( params[3] > 0 ) {
    pcPos = pcString + tiStringLength;
    while ( *pcPos == 0 && --pcPos >= pcString ) {
      for ( err = 0; err < tiNumTrims; err++ ) {
	if ( *pcPos == pcTrim[err] ) {
	  *pcPos = 0;
	  tiCharsTrimmed++;
	  break;
	} // if
      }  // for
    }  // while
  }  // if


  /* trim the front */
  if ( params[3] == 0 || params[3] == 2 ) {
    iRetVal = strspn( pcString, pcTrim );
    memmove( pcString, (pcString + iRetVal), (tiStringLength - iRetVal) );
    pcString[tiStringLength-iRetVal] = 0;
    tiCharsTrimmed += iRetVal;
  }  // if



  /* set the string in the VM */
  err = set_string( amx, params[1], pcString, 0L );
  if ( err != AMX_ERR_NONE ) {
    RAISE_ERROR(err);
    return Return(AMXFAIL);
  }  // if
  

  return Return(tiCharsTrimmed);

}  // amx_strtrim()



/* amx_strsubst ( string, subst, with, maxlen ) 
 *
 * Substitues all occurences of substring 'subst' with
 * substring 'with' in string 'string'. 
 *
 * string : string to undergo substitution
 * subst  : substring to be subtituted
 * with   : substitute for subst
 * maxlen : maximum length of string if it grows by the substitution
 *
 * returns number substitutions
 *
 */

static cell AMX_NATIVE_CALL amx_strsubst( AMX* amx, cell* params )
{

  int err = 0;
  const int iNumArgs = params[0] / sizeof(cell);
  size_t tiStringLen = 0;
  size_t iSearchLen = 0;
  size_t iSubstLen  = 0;
  int iDiffLen = 0;
  int iNewLen  = 0;
  size_t iCopy = 0;
  int iReplaced = 0;
  const size_t iMaxLen = (size_t)params[4] - 1;

  char* pcString = NULL;
  char* pcSearch = NULL;
  char* pcSubst = NULL;
  char* pcNew = NULL;
  const char* pcStart = NULL;
  char* pcAdd = NULL;
  char* pcEOS = NULL;
  char* pcPos = NULL;
  
  const int REQNUMARGS = 4;

  if ( iNumArgs != REQNUMARGS ) {
    NATIVE_ERROR;
    return Return(AMXFAIL);
  }  // if

  /* first get the string */
  err = get_string( amx, &pcString, params[1], &tiStringLen, 0L );
  if ( err != AMX_ERR_NONE ) {
    RAISE_ERROR(err);
    return Return(AMXFAIL);
  }  // if

  pcPos = pcString;
  pcStart = pcString;
  pcEOS = pcString + tiStringLen;

  /* get the search string */
  err = get_string( amx, &pcSearch, params[2], &iSearchLen, 0L );
  if ( err != AMX_ERR_NONE ) {
    RAISE_ERROR(err);
    return Return(AMXFAIL);
  }  // if

  /* ignore empty search string or we end up in an infinite loop */
  if ( iSearchLen == 0 ) {
    return Return(0);
  }  // if

  /* get the substitution string */
  err = get_string( amx, &pcSubst, params[3], &iSubstLen, 0L );
  if ( err != AMX_ERR_NONE ) {
    RAISE_ERROR(err);
    return Return(AMXFAIL);
  }  // if

  // calculate length of new string
  iDiffLen = iSubstLen - iSearchLen;
  iNewLen = tiStringLen;
  pcPos = strstr( pcString, pcSearch );
  while ( pcPos != 0 ) {
    iNewLen += iDiffLen;
    pcPos = strstr( ++pcPos, pcSearch );
  }  // while

  /*
  if ( iNewLen > iMaxLen ) {
    NATIVE_ERROR;
    return Return((cell)AMXFAIL);
  }  // if
  */

  /* get space for the new string */
  err = bind_string( amx, &pcNew, params[1], NULL, params[4] );
  if ( err != AMX_ERR_NONE ) {
    RAISE_ERROR(err);
    return Return(AMXFAIL);
  }  // if

  /* write the new string */
  pcAdd = pcNew;
  while ( pcStart < pcEOS ) {
    pcPos = strstr( pcStart, pcSearch );
    if ( pcPos == NULL ) pcPos = pcEOS; 
    iCopy = pcPos - pcStart;
    memcpy( pcAdd, pcStart, iCopy );
    pcAdd += iCopy;
    if ( pcPos != pcEOS ) {
      memcpy( pcAdd, pcSubst, iSubstLen );
      pcAdd += iSubstLen;
      iReplaced++;
    }  // if
    pcStart = pcPos + iSearchLen;
  }  // while
  
  


  /* set the string in the VM */
  err = set_string( amx, params[1], pcNew, iMaxLen );
  if ( err != AMX_ERR_NONE ) {
    RAISE_ERROR(err);
    return Return(AMXFAIL);
  }  // if
  

  return Return(iReplaced);

}  // amx_strsubst()
















/* amx_snprintf ( string, maxsize, format, ... ) 
 *
 * implements snprintf( char* str, size_t size, const char* format, ...) 
 * from stdoi.h
 *
 * string : the string tp print to
 * maxsize: maximum size of string including \0
 * format : format string
 *
 * returns the number of characters written
 * or AMXFAIL if an error occurs
 *
 */

static cell AMX_NATIVE_CALL amx_snprintf( AMX* amx, cell* params )
{

  int err = 0;
  const int iNumArgs = params[0] / sizeof(cell);
  int iRetVal = 0;
  int iNextParam = 0;
  int iNumParams = 0;
  size_t iMaxLen = 0;

  size_t tStrLen = 0;	
  size_t tCharsWritten = 0;
  size_t tCharsLeft = 0;
  size_t tSkip = 0;
  size_t tCopy = 0;

  char cInFormat = 0;
  char cTemp = 0;

  char* pcString = NULL;
  char* pcStart = NULL;
  char* pcEndOfString = NULL;
  char* pcEnd = NULL;
  char* pcOutput = NULL;
  char* pcFormat = NULL;
  char* pcStringArg = NULL;
  const char* pcRetVal = NULL;

  cell* ptCell = NULL;
  const cell* vlist = params + 3;
  

  const int REQNUMARGS = 3;

  if ( iNumArgs < REQNUMARGS ) {
    NATIVE_ERROR;
    return Return(AMXFAIL);
  }  // if

  iMaxLen = (size_t)(params[2] - 1);
  iNumParams = iNumArgs - 3;


  /* get the format string */
  err = get_string( amx, &pcFormat, params[3], &tStrLen, 0L );
  if ( err != AMX_ERR_NONE ) {
    RAISE_ERROR(err);
    return Return(AMXFAIL);
  }  // if
  
  pcEndOfString = pcFormat + tStrLen;

  /* get a string for the output */
  err = bind_string( amx, &pcString, params[1], NULL, iMaxLen );
  if ( err != AMX_ERR_NONE ) {
    RAISE_ERROR(err);
    return Return(AMXFAIL);
  }  // if

  pcStart = pcFormat;
  pcOutput = pcString;
  tCharsLeft = iMaxLen;
  pcEnd = strchr( pcStart, '%' );

  /* check if we start with a directive */
  if ( pcEnd == pcStart && pcEnd[1] != '%' ) {
    cInFormat = 1;
    pcEnd++;
  }  // if 

  /* process the format string */
  while ( (tCharsLeft > 0)  && (pcStart < pcEndOfString) ) {
    
#ifdef DEBUG
    fprintf( stderr, "\nbefoh: cLeft %d, nextParam %d, pcStart %p, pcEOS %p, informat %d\n", tCharsLeft, iNextParam, pcStart, pcEndOfString, cInFormat );
    fprintf( stderr, ">%s:-:%s\n", pcString, pcStart );
#endif

    if ( cInFormat == 1 ) {
      /* we process a format directive */

	tSkip = strspn( pcEnd, "#0+-.123456789lh" );
	pcEnd += tSkip;

	/* check if we have another parameter */
	iNextParam++;
	if ( iNextParam > iNumParams || pcEnd >= pcEndOfString ) {
	  /* simply print the directive */
	  *pcOutput = '%';
	  tCharsWritten++;
	  tCharsLeft--;
	  pcOutput++;
	  pcStart++;
	  cInFormat = 0;
	  continue;
	}  // if
	  
	switch ( *pcEnd ) {
	case 's':
	  /* string */

	  err = get_string( amx, &pcStringArg, vlist[iNextParam], NULL, 0L );
	  if ( err != AMX_ERR_NONE ) {
	    RAISE_ERROR(err);
	    return Return(AMXFAIL);
	  }  // if
	  
	  pcEnd++;
	  cTemp = *pcEnd;
	  *pcEnd = 0;

	  iRetVal = snprintf( pcOutput, (tCharsLeft+1), pcStart, pcStringArg );
	  if ( iRetVal > -1 && iRetVal < (int)tCharsLeft ) {
	    tCopy = (size_t)iRetVal;
	  } else {
	    tCopy = tCharsLeft;
	  } // if-else

	  tCharsWritten += tCopy;
	  tCharsLeft -= tCopy;
	  pcRetVal = pcOutput;
	  *pcEnd = cTemp;
	  pcOutput += tCopy;
	  pcStart = pcEnd;
	  break;

	case 'c':
	  /* character */
	case 'd':
	case 'i':
	  /* number */
	case 'x':
	  /* hex number */
	case 'o':
	  /* octal number */

	  err = amx_GetAddr( amx, vlist[iNextParam], &ptCell );
	  if ( err != AMX_ERR_NONE ) {
	    RAISE_ERROR(err);
	    return Return(AMXFAIL);
	  }  // if

	  pcEnd++;
	  cTemp = *pcEnd;
	  *pcEnd = 0;

	  iRetVal = snprintf( pcOutput, (tCharsLeft+1), pcStart, *ptCell );
	  if ( iRetVal > -1 && iRetVal < (int)tCharsLeft ) {
	    tCopy = (size_t)iRetVal;
	  } else {
	    tCopy = tCharsLeft;
	  } // if-else

	  tCharsWritten += tCopy;
	  tCharsLeft -= tCopy;
	  pcRetVal = pcOutput;
	  *pcEnd = cTemp;
	  pcOutput += tCopy;
	  pcStart = pcEnd;
	  break;

	default:
	  /* wrong format. abort */
	  NATIVE_ERROR;
	  return Return(AMXFAIL);
	}  // switch

      cInFormat = 0;

    } else {
      /* we copy the format verbously */

      pcEnd = strchr( pcStart, '%' );
      if ( pcEnd == NULL ) { /* no more directives */
        pcEnd = pcEndOfString;
      }  // if
      tCopy = min( tCharsLeft, (pcEnd-pcStart) );
      memcpy( pcOutput, pcStart, tCopy );
      tCharsWritten += tCopy;
      tCharsLeft -= tCopy;
      pcOutput += tCopy;
      pcStart = pcEnd;

      if ( pcEnd < pcEndOfString ) {
        pcEnd = pcStart + 1;
        if ( *pcEnd == '%' ) {
          *pcOutput = '%';
          tCharsWritten++;
          tCharsLeft--;
          pcOutput++;
          pcEnd++;
          pcStart = pcEnd;
        } else {
          cInFormat = 1;
        }  // if-else
      }  // if
    }  // if-else

#ifdef DEBUG
    fprintf( stderr, "aftuh: cLeft %d, nextParam %d, pcStart %p, pcEOS %p, informat %d\n", tCharsLeft, iNextParam, pcStart, pcEndOfString, cInFormat );
    fprintf( stderr, ">%s:-:%s\n", pcString, pcStart );
#endif
  }  // while
  
  /* set the string in the AMX */
  err = set_string( amx, params[1], pcString, iMaxLen );
  if ( err != AMX_ERR_NONE ) {
    RAISE_ERROR(err);
    return Return(AMXFAIL);
  }  // if

  return Return(tCharsWritten);

}  // amx_snprintf()





/* declare functions to be registered with the virtual machine */
AMX_NATIVE_INFO string_Natives [] = {
  { "strcpy", amx_strcpy },
  { "strncpy", amx_strncpy },
  { "strcat", amx_strcat },
  { "strncat", amx_strncat },
  { "strcmp", amx_strcmp },
  { "strncmp", amx_strncmp },
  { "strcasecmp", amx_strcasecmp },
  { "strncasecmp", amx_strncasecmp },
  { "strchr", amx_strchr },
  { "index", amx_strchr },
  { "strrchr", amx_strrchr },
  { "rindex", amx_strrchr },
  { "strstr", amx_strstr },
  { "strstrx", amx_strstrx },
  { "strcasestr", amx_strcasestr },
  { "strcasestrx", amx_strcasestrx },
  { "strspn", amx_strspn },
  { "strcspn", amx_strcspn },
  { "strtok", amx_strtok },
  { "strtokrest", amx_strtokrest },
  { "strgtok", amx_strgtok },
  { "strgtokrest", amx_strgtokrest },
  { "strsplit", amx_strsplit },
  { "strgsplit", amx_strgsplit },
  { "strsep", amx_strsep },
  { "strgsep", amx_strgsep },
  { "strcount", amx_strcount },
  { "strtrim", amx_strtrim },
  { "strsubst", amx_strsubst },
  { "snprintf", amx_snprintf },
  { 0, 0 }
};
