#ifndef _AMUTIL_H_
#define _AMUTIL_H_
/* 
 * File: amutil.h
 * 
 * Description: This file declares various utility functions.
 *              It should be safely included in all AM source files.
 *              I got sick and tired of getting endless errors when 
 *              including users.h or util.h due to unknown requirements
 *              and dependencies.
 *
 *  Copyright (c) Alfred Reynolds, 2000
 *  This file is covered by the GPL.
 *
 * $Id: amutil.h,v 1.3 2001/09/19 22:29:24 darope Exp $
 *
 */

#include <cvardef.h>
#include <errno.h>

extern cvar_t* ptAM_debug;
extern cvar_t* ptAM_devel;
extern cvar_t* ptAM_botProtection;
extern cvar_t* ptAM_autoban;
extern cvar_t* ptAM_reserve_slots;
extern cvar_t* ptAM_hide_reserved_slots;


// some macros to be used around
#define DEBUG_LOG( level, args )   do{  if ( ptAM_debug && ptAM_debug->value >= level ) \
                 UTIL_LogPrintf( "[ADMIN] DEBUG: %s\n", UTIL_VarArgs args );} while(0)

#define DEVEL_LOG( level, args )   do{  if ( ptAM_devel && ptAM_devel->value >= level ) \
                 UTIL_LogPrintf( "[ADMIN] DEVEL(%i): %s\n", level, UTIL_VarArgs args );} while(0)



#endif /* _AMUTIL_H_ */
