/*
 * ===========================================================================
 *
 * $Id: plugin.inc,v 1.3 2003/07/25 21:25:48 darope Exp $
 *
 *
 * Copyright (c) 2002-2003 Alfred Reynolds, Florian Zschocke, Magua
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
 *    Plugin system functions
 *
 */
#if defined _plugin_included
  #endinput
#endif
#define _plugin_included

//
// plugin_checkcommand(Command[], &Access)
// Checks if any plugin implements Command. Returns the commands access
// level in Access. Returns number of plugins that implement that command. 
//
native plugin_checkcommand( sCommand[], &iAccess = 0 );

//
// plugin_message(Text)
// Pretty useless test function that returns
// 'Text', much like message(), but identifies the plugin as well.
//
native plugin_message( sMessage[] );

/* Not implemented. Don't use!
native plugin_register( sCommand[], sFunction[] );
*/

/* Not implemented. Don't use!
native plugin_setdesc( sDescription[] );
*/

/* Not implemented. Don't use!
native plugin_setname( sName[] );
*/

/* Not implemented. Don't use!
native plugin_setversion( sVersion[] );
*/
