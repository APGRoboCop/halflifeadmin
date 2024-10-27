/*
 * ===========================================================================
 *
 * $Id: authid.cpp,v 1.8 2004/05/07 16:53:23 darope Exp $
 *
 *
 * Copyright (c) 2002-2003 Florian Zschocke, Alfred Reynolds
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
 *   This file defines the AMAuthId class which is used to represent AuthIds.
 *   An AuthId can be either a WonId or a SteamId. 
 */


#include <cstdlib>
//#include <cerrno>
#include "authid.h"
#include "amlibc.h"

using namespace nsAuthid;

char AMAuthId::m_acString[STATIC_LEN] = "";

void AMAuthId::f_parse_id( const char* _pcID ) {
	constexpr int BOT = 0x00544f42;

	if ( _pcID == nullptr ) return;

	m_bIdIsSet = false;

	// skip leading blanks
	while ( *_pcID == ' ' || *_pcID == '\t' ) ++_pcID;

	if ( *_pcID == '\0' ) { 
		m_uiWonid = 0;
		m_uiAuthid64 = 0;
		m_cAuthidType = undef;
		m_bIsWonid = false;
	}  // if

	if ( BOT == *(reinterpret_cast<const int*>(_pcID)) ) {

		// a bot
		m_bIsWonid = true;
		m_uiWonid = BOT_ID;
		m_uiAuthid64 = INVAL_ID_L;
		m_bIdIsSet = true;

	} else if ( AMAuthId::is_loopid(_pcID) ) {

		// a loopback id, i.e. listenserver user
		m_bIsWonid = false;
		m_uiWonid = LOOP_ID;
		m_uiAuthid64 = INVAL_ID_L;
		m_bIdIsSet = true;
		if ( _pcID[0] == 'S' && _pcID[1] == 'T' && _pcID[2] == 'E' ) {
			m_cAuthidType = steam;
		} else {
			m_cAuthidType = valve;
		}

	} else if ( AMAuthId::is_lanid(_pcID) ) {

		// a LAN id, i.e. sv_lan == 1
		m_bIsWonid = false;
		m_uiWonid = LAN_ID;
		m_uiAuthid64 = INVAL_ID_L;
		m_bIdIsSet = true;
		if ( _pcID[0] == 'S' && _pcID[1] == 'T' && _pcID[2] == 'E' ) {
			m_cAuthidType = steam;
		} else {
			m_cAuthidType = valve;
		}
	
	} else if ( _pcID[0] == 'S' && _pcID[1] == 'T' && _pcID[2] == 'E' && 
		 _pcID[3] == 'A' && _pcID[4] == 'M' && _pcID[5] == '_' ) {

		// it is a steam id
		m_bIsWonid = false;
		m_cAuthidType = steam;

		// check if steamid is still pending
		if ( _pcID[6] == 'I' && _pcID[7] == 'D' ) {
			m_uiWonid = INVAL_ID_S;
			m_uiAuthid64 = INVAL_ID_L;
			return;
		}  // if

		char* pcEndptr = nullptr;
		// parse first number
		m_uiWonid = strtoul( _pcID+6, &pcEndptr, 10 );

		// check if string was valid steamid format
		if ( *pcEndptr != ':' ) {
			// invalid steam id format
			m_cAuthidType = undef;
			m_uiWonid = INVAL_ID_S;
			m_uiAuthid64 = INVAL_ID_L;
			return;
		}  // if


		// parse second number
		_pcID = pcEndptr + 1;
		m_uiAuthid64 = am_strtoui64( _pcID, &pcEndptr, 10 );

		// check if string is a steamid2 format
		if ( *pcEndptr == ':' ) {
			// parse second number
			_pcID = pcEndptr + 1;
			// shift Authid64 up 32 bits since we just read the HIGH two words
			m_uiAuthid64 <<= 32 ;
			const uint32_t uiAuLow = strtoul( _pcID, &pcEndptr, 10 );
			// join the high and low words
			m_uiAuthid64 |= uiAuLow;

			m_cAuthidType = steam2;
		}

		//-- Check if the steamid string was valid.
		//-- Skip following blanks.
		while ( *pcEndptr == ' ' || *pcEndptr == '\t' ) ++pcEndptr;
		if ( *pcEndptr != '\0' ) {
			m_cAuthidType = undef;
			m_uiWonid = INVAL_ID_S;
			m_uiAuthid64 = INVAL_ID_L;
			return;
		}  // if

		m_bIdIsSet = true;

	} else if ( _pcID[0] == 'V' && _pcID[1] == 'A' && _pcID[2] == 'L' && 
		 _pcID[3] == 'V' && _pcID[4] == 'E' && _pcID[5] == '_' ) {

		// it is a valve id
		m_bIsWonid = false;
		m_cAuthidType = valve;

		// check if valveid is still pending
		if ( _pcID[6] == 'I' && _pcID[7] == 'D' ) {
			m_uiWonid = INVAL_ID_S;
			m_uiAuthid64 = INVAL_ID_L;
			return;
		}  // if

		char* pcEndptr = nullptr;
		// parse first number
		m_uiWonid = strtoul( _pcID+6, &pcEndptr, 10 );

		// check if string was valid valveid format
		if ( *pcEndptr != ':' ) {
			// invalid valveid format
			m_uiWonid = INVAL_ID_S;
			m_uiAuthid64 = INVAL_ID_L;
			m_cAuthidType = undef;
			return;
		}  // if


		// parse second number
		_pcID = pcEndptr + 1;
		m_uiAuthid64 = am_strtoui64( _pcID, &pcEndptr, 10 );

		// check if string is a valveid2 format
		if ( *pcEndptr == ':' ) {
			// parse second number
			_pcID = pcEndptr + 1;
			// shift Authid64 up 32 bits since we just read the HIGH two words
			m_uiAuthid64 <<= 32 ;
			const uint32_t uiAuLow = strtoul( _pcID, &pcEndptr, 10 );
			// join the high and low words
			m_uiAuthid64 |= uiAuLow;

			m_cAuthidType = valve2;
		}

		//-- Check if the valveid string was valid.
		//-- Skip following blanks.
		while ( *pcEndptr == ' ' || *pcEndptr == '\t' ) ++pcEndptr;
		if ( *pcEndptr != '\0' ) {
			m_uiWonid = INVAL_ID_S;
			m_uiAuthid64 = INVAL_ID_L;
			m_cAuthidType = undef;
			return;
		}  // if

		m_bIdIsSet = true;

	} else {

		// it is a won id
		char* pcEndptr = nullptr;
		m_uiWonid = strtoul( _pcID, &pcEndptr, 10 );
		//-- Check if the wonid string was valid.
		//-- Skip following blanks.
		while ( *pcEndptr == ' ' || *pcEndptr == '\t' ) ++pcEndptr;
		if ( *pcEndptr != '\0' ) {
			m_uiWonid = INVAL_ID_S;
			m_uiAuthid64 = INVAL_ID_L;
			m_cAuthidType = undef;
			return;
		}  // if


		m_bIsWonid = true;
		m_bIdIsSet = true;
	}  // if-else
}



