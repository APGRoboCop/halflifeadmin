/*
 * ===========================================================================
 *
 * $Id: version.h,v 1.2 2003/03/26 20:44:15 darope Exp $
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
 * Comments:  This file provides version information for Adminmod
 *
 */

#ifndef _VERSION_H_
#define _VERSION_H_

/*
 * Build type, either debugging or optimized.
 * This is set by the Makefile. Provide a default
 * in case is isn't defined by the Makefile
 *
 */
#ifndef OPT_TYPE
#if defined(_DEBUG)
#define OPT_TYPE "debugging"
#elif defined(_NDEBUG)
#define OPT_TYPE "optimized"
#else
#define OPT_TYPE "default"
#endif /* _MSC_VER */
#endif

/*
 * Version number.
 * This is also defined by the Makefile.
 * If not, we provide it here.
 */
#ifndef MOD_VERSION
#ifdef USE_METAMOD
#define MOD_VERSION "2.50 (MM)"
#else
#define MOD_VERSION "2.50"
#endif
#endif

/*
 * We keep the compile time and date in a static string.
 * This info gets updated on every link, indicating the
 * latest time and date the dll was compiled and linked.
 */
extern char* COMPILE_DTTM;
extern char* COMPILE_DATE;

/*
 * We can also provide the timezone. It gets set in the
 * Makefile. If not, we can provide it here.
 */
#ifndef TZONE
#define TZONE ""
#endif

/*
 * This info is used as Plugin info by Metamod
 */
#define VDATE COMPILE_DATE
#define VNAME "Admin Mod"
#define VAUTHOR "Alfred Reynolds <alfred@valvesoftware.com>"
#define VURL "http://www.adminmod.org/"

/*
// Various strings for the Windows DLL Resources in res_meta.rc
#define RC_COMMENTS		"Adminmod provides extended possibilities to Half-Life server administrators"
#define RC_DESC			"Adminmod Half-Life MOD DLL"
#define RC_FILENAME		"ADMINMOD.DLL"
#define RC_INTERNAL		"ADMINMOD"
#define RC_COPYRIGHT	"Copyright© 2000,2001, Alfred Reynolds; under the GPL"
*/

#endif /* _VERSION_H_ */
