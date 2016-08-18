/*
 * ===========================================================================
 *
 * $Id: admin.inc,v 1.8 2003/11/08 10:21:29 darope Exp $
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
 * Comments:
 *
 *   Main adminmod functions
 *
 */

#if defined _admin_included
	#endinput
#endif
#define _admin_included

#pragma semicolon 1

#define ADMINMOD_VERSION 25102

enum print_type {
	print_console=0,
	print_center,
	print_chat,
	print_tty,
	print_pretty
};

enum fsize_unit {
	bytes = 0,
	lines
};

const bBanByID = 0;
const bBanByIP = 1;
const bBanBoth = 2;

enum uid {
	uid_none = 0,
	uid_invalid,
	uid_index,
	uid_sessionID,
	uid_wonID,
};

/* Defined as '+ 1' so when we make an array[MAXPLAYERS], we get the range from 0 - 32. */
#define MAX_PLAYERS 32 + 1
#define TEAM_PROXY  600
#define TEAM_SPECTATOR 500

#define MAX_NUMBER_LENGTH 20
#define MAX_COMMAND_LENGTH 30
#define MAX_NAME_LENGTH 33
#define MAX_DATA_LENGTH 200
#define MAX_TEXT_LENGTH 200
#define MAX_AUTHID_LENGTH 39

#define PLUGIN_HANDLED -1
#define PLUGIN_FAILURE 0
#define PLUGIN_CONTINUE 1

#define ACCESS_ALL 0
#define ACCESS_TIMELEFT 0
#define ACCESS_USERLIST 0
#define ACCESS_VERSION 0
#define ACCESS_VOTE_KICK 1
#define ACCESS_VOTE_MAP 1
#define ACCESS_FRAGLIMIT 2
#define ACCESS_MAP 2
#define ACCESS_TIMELIMIT 2
#define ACCESS_ABORT_VOTE 2
#define ACCESS_RELOAD 4
#define ACCESS_PAUSE 8
#define ACCESS_PASS 16
#define ACCESS_FRIENDLY_FIRE 32
#define ACCESS_GRAVITY 32
#define ACCESS_TEAMPLAY 32
#define ACCESS_CHAT 64
#define ACCESS_SAY 64
#define ACCESS_KICK 128
#define ACCESS_BAN 256
#define ACCESS_CONFIG 512
#define ACCESS_IMMUNITY 4096
#define ACCESS_RCON 65536 /* Define this to 512 to have Alfred's setting */
#define ACCESS_RESERVE_NICK 16384 /* This is never used in the script; just for reference */
#define ACCESS_RESERVE_SPOT 32768 /* This is never used in the script; just for reference */


forward plugin_init();
forward plugin_info(HLOldName, HLNewName, UserIndex);
forward plugin_connect(HLUserName, HLIP, UserIndex);
forward plugin_disconnect(HLUserName, UserIndex);
forward plugin_command(HLCommand, HLData, HLUserName, UserIndex);

native access( iAccess, sName[] = "" );
native auth( sName[] = "");
native ban( sPlayer[], iTime, bBanBy = bBanByID );
native censor_words( sString[] );
native centersay( sText[], iTime, iRed, iGreen, iBlue );
native centersayex( sUser[], sText[], iTime, iRed, iGreen, iBlue );
native changelevel( sMap[], iIntermissionPause = 0 );
native check_user( sPlayer[] );
native check_words( sPlayer[] );
native consgreet( sMessage[] );
native convert_string( iHLString, sSmallString[], iMaxLength );
native currentmap( iMap[], iMaxLength );
native directmessage( sMessage[], iUserID = -1, uid:tUidType = uid:uid_sessionID );
native exec( sCommand[], bWriteLogEntry = 1 );
native execclient( sPlayer[], sCommand[] );
native get_serverinfo( sKey[], sValue[], iMaxLength );
native get_userAuthID( sPlayer[], sAuthid[], iMaxLength = MAX_AUTHID_LENGTH );
native get_userindex( sPlayer[], &iIndex );
native get_userinfo( sPlayer[], sKey[], sInfo[], iMaxLength );
native get_userIP( sPlayer[], sIP[], iMaxLength, &iPort = 0 );
native get_username( sPlayer[], sName[], iMaxLength );
native get_userorigin( sPlayer[], &iX, &iY, &iZ );
native get_userSessionID( sPlayer[], &iSessionID );
native get_userWONID( sPlayer[], &iWONID );
native get_vaultdata( sKey[], sData[], iMaxLength );
native get_vaultnumdata( sKey[], &iData );
native getstrvar( sVarname[], sValue[], iMaxLength );
native gettarget( sPlayer[], sTargetName[], iMaxLength, iRange = 2048 );
native getteamcount( iTeam );
native getvar( sVarname[] );
native glow( sTarget[], iRed = 0, iGreen = 0, iBlue = 0 );
native godmode( sPlayer[], iOnOff );
native help( sKeyword[] );
native kick( sPlayer[] );
native kill_timer( iTimer );
native list_maps( );
native log( sLogEntry[] );
native look_in_dir( sDirectory[], sMatch[], sFilename [], iNumber );
native maptime( iWhichtime, iPrintToConsole = 0 );
native maxplayercount( );
native message( sTarget[], sMessage[] );
native messageex( sTarget[],sMessage[], print_type:iMessageType );
native nextmap( sMap[], iMaxLength );
native noclip( sPlayer[], iOnOff );
native playercount( );
native playerinfo( iPlayerIndex, sName[], iLength, &iUserID = 0, &iWONID = 0, &iTeam = 0, &iDead = 0, sAuthID[MAX_AUTHID_LENGTH] = "" );
native playsound( sTarget[], iSound[] );
native pointto( iRange = 2048 );
native rainbow( sText[], iTime, iRedStart, iGreenStart, iBlueStart, iRedEnd, iGreenEnd, iBlueEnd );
native reload( );
native say( sMessage[] );
native selfmessage( sMessage[] );
native servertime( sTimeString[], iMaxLen, sFormat[] = "none" );
native set_serverinfo( sKey[], sValue[] );
native set_timer( sFunction[], iWaitSeconds, iRepeatCount, sParameter[] = "" );
native set_vaultdata( sKey[], sData[] );
native set_vaultnumdata( sKey[], iData );
native slap( sPlayer[] );
native slay( sPlayer[] );
native speakto( sTarget[], iSentence[] );
native strtonum( sString[] );
native systemtime( );
native teleport( sPlayer[], iX, iY, iZ );
native timeleft( iPrintToConsole = 1 );
native typesay( sMessage[], iTime, iRed, iGreen, iBlue );
native unban( sWONID[] );
native userlist( sPattern[] = "" );
native valid_map( sMap[] );
native version( );
native vote( sVoteString[], ... );
native vote_allowed( );

native spawn( sClass[], iX, iY, iZ, iXAngle, iYAngle, iZAngle );
native listspawn( sClass[] );
native movespawn( iIdentity, iX, iY, iZ, iXAngle, iYAngle, iZAngle );
native removespawn( iIdentity );

native fileexists( sFilename[] );
native filesize( sFilename[], fsize_unit:Unit = 1 );
native readfile( sFilename[], sLine[], iLineNum, iMaxLength );
native writefile( sFilename[], sLine[], iLineNum = -1 );
native resetfile( sFilename[] );
native deletefile( sFilename[] );

native plugin_exec( sCommand[], sArguments[] );
native plugin_registercmd( Command[], Function[], RequiredAccess, HelpMessage[] = "" );
native plugin_registerhelp( Command[], RequiredAccess, HelpMessage[] );
native plugin_registerinfo( Name[], Description[], Version[]);
native plugin_registerevt( sCommand[], sFunction[] );

native setstrvar( cvar[], value[]);
native menu(username[],text[],keys,time=0);

native get_userFrags( sPlayer[], &frags );
native get_userHealth(sPlayer[], &health );
native get_userTeam( sPlayer[], &team );
native get_userArmor( sPlayer[], &armor );

native motd( sPlayer[], Text[] );
native plugin_reg_event( sEvent[], sFunction[] );
native event_getkey_string( Event, sKey[], sResult[], iLen );
// these return the value
native event_getkey_int( Event, sKey[] );
native event_getkey_float( Event, sKey[] );
