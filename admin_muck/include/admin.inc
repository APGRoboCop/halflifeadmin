/*
 * Main adminmod functions
 *
 * $Id: admin.inc,v 1.13 2001/06/23 10:52:53 darope Exp $
 *
 */
 
#if defined _admin_included
  #endinput
#endif
#define _admin_included

native access( iAccess, sName[] = "" );
native auth( sTarget[] );
native ban( sPlayer[], iTime );
native censor_words( sString[] );
native centersay( sText[], iTime, iRed, iGreen, iBlue );
native changelevel( sMap[] );
native check_user( sPlayer[] );
native check_words( sPlayer[] );
native consgreet( sMessage[] );
native convert_string( iHLString, sSmallString[], iMaxLength );
native currentmap( iMap[], iMaxLength );
native exec( sFilename[] );
native execclient( sPlayer[], sCommand[] );
native get_serverinfo( sKey[], sValue[], iMaxLength ); 
native get_userindex( sPlayer[], &iIndex );
native get_userinfo( sPlayer[], sKey[], sInfo[], iMaxLength );   
native get_username( sPlayer[], sName[], iMaxLength );
native get_userorigin( sPlayer[], &iX, &iY, &iZ );
native get_userSessionID( sPlayer[], &iSessionID );
native get_userWONID( sPlayer[], &iWONID );
native get_vaultdata( sKey[], sData[], iMaxLength );
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
native messageex( sTarget[],sMessage[], iMessageType );
native nextmap( sMap[], iMaxLength );
native noclip( sPlayer[], iOnOff );
native playercount( );
native playerinfo( iPlayerIndex, sName[], iLength, &iUserID = 0, &iWONID = 0, &iTeam = 0, &iDead = 0 );
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
native slap( sPlayer[] );
native slay( sPlayer[] );
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
native filesize( sFilename[], iUnit = 1 );
native readfile( sFilename[], sLine[], iLineNum, iMaxLength );
native writefile( sFilename[], sLine[], iLineNum = -1 );
native resetfile( sFilename[] );
native deletefile( sFilename[] );

native plugin_exec( sCommand[], sArguments[] );
native plugin_message( sMessage[]);
native plugin_registercmd( Command[], Function[], RequiredAccess, HelpMessage[] = "" );
native plugin_registerhelp( Command[], RequiredAccess, HelpMessage[] );
native plugin_registerinfo( Name[], Description[], Version[]);


enum PRINT_TYPE {
  print_console=0,
  print_center,
  print_chat,
  print_tty
};

enum FSIZE_UNIT {
  bytes = 0,
  lines
};


/* Defined as '+ 1' so when we make an array[MAXPLAYERS], we get the range from 0 - 32. */
#define MAX_PLAYERS 32 + 1 

#define MAX_NUMBER_LENGTH 20
#define MAX_COMMAND_LENGTH 30
#define MAX_NAME_LENGTH 50
#define MAX_DATA_LENGTH 200
#define MAX_TEXT_LENGTH 200

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
