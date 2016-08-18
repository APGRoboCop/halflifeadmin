/*
 * ===========================================================================
 *
 * $Id: adminlib.inc,v 1.5 2003/07/27 20:16:05 darope Exp $
 *
 *
 * Copyright (c) 2001-2003 Alfred Reynolds, Florian Zschocke, Magua
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
 *   adminmod helper function library
 *
 */
#if defined _adminlib_included
  #endinput
#endif
#define _adminlib_included

#include <admin>
#include <core>
#include <string>

#define CHAR_INVALID -1
#define FALSE 0
#define NULL_CHAR 0
#define PERIOD_CHAR 46
#define QUOTE_CHAR 34
#define TRUE 1

/* Returns 1 if the current user has the required auth level, 0 otherwise */
stock check_auth( iAuthLevel ) {
	new iResult = access(iAuthLevel,"");
	if (iResult!=0) iResult = 1;
	return iResult;
}

/* Returns the lesser of a and b */
stock min(a,b) {
	if (a<b) return a;
	else return b;
	return 1;
}

/* Returns the greater of a and b */
stock max(a,b) {
	if (a>b) return a;
	else return b;
	return 1;
}

/* Executes a command while providing the nicely formatted output */
stock execute_command(sUser[], sCommand[], sHalfLifeCmd[], sData[]) {
	new sRconCmd[MAX_DATA_LENGTH];
	
	say_command(sUser,sCommand,sData);
	snprintf(sRconCmd, MAX_DATA_LENGTH, "%s %s", sHalfLifeCmd, sData);
	exec(sRconCmd);
}

/* Nicely formats the current command */
stock format_command(sUser[],sCommand[],sData[],sText[]) {
	snprintf(sText, MAX_TEXT_LENGTH, "Command: %s used command %s %s", sUser, sCommand, sData);
}

/* Logs a command */
stock log_command(sUser[],sCommand[],sData[]) {
	new sText[MAX_TEXT_LENGTH];
	format_command(sUser,sCommand,sData,sText);
	log(sText);
}

/* NumToStr courtesy of Nathan O'Sullivan (http://nathan.qgl.org/halflifeadmin/numtostr.txt) */
stock numtostr(num,str[]) {
	new Base = 1;
	new Digits = 1;
	new i = 0;
	
	/* Special case: 0 */
	if (num == 0) {
		str[i++] = '0';
		str[i++] = NULL_CHAR;
	} else {
		/* If we've got a negative number, add a negative sign
		to the string, and multiply the number by -1 */
		if (num < 0) {
			str[i++] = '-';	
			num *= -1;
		}
	
		/* Ok.  We've got at least one digit.  Keep multiplying by
		10 till we get a higher number than what we've got.  Note 
		that this will leave Digits 1 higher than what we want (eg,
		if Num was 7, Digits will be 2 */
		while (Base <= num) {
			Base *= 10;
			Digits++;
		}
		
		/* Because Digits is higher, use --Digits rather than Digits--. 
		Also, divide Base before using it. */
		while (--Digits > 0) {
			Base /= 10;
			str[i++] = '0' + (num - (num % Base)) / Base;
			num = num % Base;
		}
		str[i++] = NULL_CHAR;
	}
}

/* Command to use when the person doesn't have the proper permissions. */
stock reject_message(iPublic = 0) {
	new sText[MAX_TEXT_LENGTH];
	
	getstrvar("admin_reject_msg",sText,MAX_TEXT_LENGTH);
	if(strlen(sText) < 2) 
		strcpy(sText, "You do not have access to this command.", MAX_TEXT_LENGTH);
	if (iPublic == 0) {
		selfmessage(sText);
	} else {
		say(sText);
	}
}

/* Results differ upon the setting of admin_quiet.  If admin_quiet
is 0, says who executed what command.  If admin_quiet is 1, says
'Admin' executed what command. Otherwise, simply logs. 
The override option allows one to ignore admin_quiet, and always
display a message.
*/
stock say_command(sUser[],sCommand[],sData[], iOverride = 0) {
	new iQuiet;
	new sText[MAX_TEXT_LENGTH];
	
	iQuiet = getvar("admin_quiet");
	if (iQuiet==0) {
		format_command(sUser,sCommand,sData,sText);
		say(sText);
	} else if (iQuiet==1 || iOverride==1) {
		format_command("Admin",sCommand,sData,sText);
		say(sText);
	} else {
		log_command(sUser,sCommand,sData);
	}
}



/* Given a string (str[]), this will attempt to break it apart at the first 
space that's not inside quotation marks.  Quotes at the beginning and end
will be stripped.  Ie,

new str[20] = "This is a test"
new first[20];
new second[20];
strbreak(str,first,second, 20);

first is now equal to "This", and second is now equal to "is a test".  If 
str[] had been "^"This is^" a test", first would be "This is" and second would 
be "a test". 
At maximum maxlen characters will be copied. */
stock strbreak(str[], first[], second[], maxlen, flen=sizeof first, slen=sizeof second ) {
	new i = 0;
	new j = 0;
	new NullPos = CHAR_INVALID;
	new Quote = FALSE;
	new SpacePos = CHAR_INVALID;
	new iFirstEnd;
	new iSecondEnd;

	if (maxlen == 0)
		maxlen = strlen(str);



	// Find the position of the first unquoted space and the terminating NULL character
	for(i=0; i<=strlen(str); i++) {
		if (str[i] == QUOTE_CHAR) {
			if (Quote==FALSE) {
				Quote = TRUE;
			} else {
				Quote = FALSE;
			}
		} else if (str[i] == ' ' && SpacePos == CHAR_INVALID && Quote == FALSE) {
			SpacePos = i;
		} else if (str[i] == NULL_CHAR) {
			NullPos = i;
			break;
		}	
	}
	
	if (NullPos == CHAR_INVALID) NullPos = maxlen + 1;


	if (SpacePos == CHAR_INVALID) {
		strcpy(first, str, flen);
		strinit(second);

	} else {		
		// Make sure we do not copy more characters into the second array
		// than would fit into it. If the second part of the source string
		// is longer than the length of the second array, truncate it to fit.
		new iSecondLen = NullPos - SpacePos -1;
		iSecondEnd = min( iSecondLen, (slen-1) );

		// If maxlen is set we want to copy only max maxlen characters.
		// Check if this is smaller than our current limit
		iSecondEnd = min( iSecondEnd, maxlen );

		// Make sure we do not copy more characters into the first array
		// than would fit into it. If the first part of the source string
		// is longer than the length of the first array, truncate it to fit.
		iFirstEnd = min( SpacePos, (flen-1) );

		// If maxlen is set we want to copy only max maxlen characters.
		// Check if this is smaller than our current limit
		iFirstEnd = min( iFirstEnd, maxlen );

	
		// Copy the first part of the string into the first array
		for( i = 0; i < iFirstEnd; i++)  {
			first[i] = str[i];
		}
		// Zero-terminate the first array
		first[iFirstEnd] = NULL_CHAR;
		

		// Copy the second part of the string into the second array
		for( i = SpacePos+1, j = 0; j < iSecondEnd; i++, j++) {
			second[j] = str[i];
		}
		// Zero-terminate the second array
		second[iSecondEnd] = NULL_CHAR;
	}

	
	/* Strip out the quotes of the return values. */
	strstripquotes(first);
	strstripquotes(second);
}


/* Counts the number of times searchchar appears in str[] */
/*
stock strcount(str[], searchchar) {
	new i = 0;
	new maxlen = strlen(str);
	new Count = 0;
	
	for(i = 0; i <= maxlen; i++) {
		if (str[i] == searchchar)
			Count++;
	}
	return Count;
}
*/

/* Returns 1 if two strings are exactly the same, including length, etc */
stock streq( strOne[], strTwo[] ) {
	if (strlen(strOne) != strlen(strTwo)) {
		return 0;
	} else if (strcasecmp(strOne, strTwo)==0) {
		return 1;
	}
	return 0;
}

/* Initialize a string */
stock strinit( sString[] ) {
	sString[0] = NULL_CHAR;
}

/* returns 1 if two strings are the same in the first Length (or less 
if one is smaller than Length) digits, 0 otherwise */
stock strmatch( sOne[], sTwo[], iLength) {
	if (strncasecmp(sOne, sTwo, iLength)==0) {
		return 1;
	}
	return 0;
}

/* Strips the quotes from the beginning and ending, if they exist. Ignores
those in the middle. */
stock strstripquotes(str[]) {
	new maxlen = strlen(str);
	new i;
	
	if(maxlen==0)
		return;
		
	if(str[maxlen - 1]==QUOTE_CHAR) 
		str[--maxlen] = NULL_CHAR;
		
	if(str[0] == QUOTE_CHAR) {
		for(i=0; i<=maxlen; i++)
			str[i] = str[i+1];
		str[i-2] = NULL_CHAR;
	}
}

/* I had this in the normal alphabetical order...but it caused
the Small compiler to barf.  So I moved it down here, and it's
fine.  That doesn't fill me with inspiration, but it seems
to work. */
/* Returns 1 if param is '1' or 'on'...0 for anything else. */
stock check_param( sParam[] ) {
	new iResult = 0;
	
	if(strlen(sParam) > 0) {
		if (streq(sParam, "on")==1) {
			iResult = 1;
		} else if (streq(sParam, "1")==1) {
			iResult = 1;
		}
	}
	return iResult;
}

/* Returns 1 if the target has immunity */
stock check_immunity( sTarget[] ) {
	/* If the server doesn't honor immunity, no one is immune. */
	if (getvar("admin_ignore_immunity")!=0) {
		return 0;
	/* Else, you're immune if you have that access level. */
	} else if (access(ACCESS_IMMUNITY,sTarget)!=0) {
		return 1;
	}
	return 0;
}


stock execclient_all( sCommand[] ) {

	new Target[MAX_NAME_LENGTH];
	new i;
	new iSessionID;
	new sSessionID[MAX_NUMBER_LENGTH];
	new maxplayers = maxplayercount();

	for(i=1; i<=maxplayers; i++) {
		strinit(Target);
		if(playerinfo(i,Target,MAX_NAME_LENGTH, iSessionID)==1) {
			numtostr( iSessionID, sSessionID );
			execclient(sSessionID,sCommand);

		}  // if
	}  // for

	return 1;
}  // execclient_all()


/**********************************  PUBLIC UTILITY FUNCTIONS  *****************************/

/* Called by changelevel() by a timer call. Do not remove! 
   Changes map to specified map by calling changelevel with
   specific parameters. */
public ChangeMap(Timer,Repeat,HLUser,HLParam) {
	new NewMap[MAX_NAME_LENGTH];
	
	convert_string(HLParam,NewMap,MAX_NAME_LENGTH);
	if(strlen(NewMap) != 0) {
		changelevel(NewMap, -1);
	}
}

