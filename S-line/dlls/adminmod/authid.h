/*
 * ===========================================================================
 *
 * $Id: authid.h,v 1.12 2004/05/07 16:53:23 darope Exp $
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
 *   This file declares the AMAuthId class which is used to represent AuthIds.
 *   An AuthId can be either a WonId or a SteamId. 
 */

#ifndef AM_AUTHID_H
#define AM_AUTHID_H

#ifdef LINUX
#  include <stdint.h>  /* uintXX_T */
#else
#  if defined(_MSC_VER)
typedef __int32 int32_t;
typedef unsigned __int32 uint32_t;
typedef __int64 int64_t;
typedef unsigned __int64 uint64_t;
#  else
typedef long int32_t;
typedef unsigned long uint32_t;
typedef long long int64_t;
typedef unsigned long long uint64_t;
#  endif
//#  define snprintf _snprintf
#endif  // LINUX

#include <cstdio>  /* snprintf */

namespace nsAuthid {

	const char STATIC_LEN = 40;
	const uint32_t BOT_ID = 0;
    const uint32_t LAN_ID = ~0;
    const uint32_t LOOP_ID = 1;
#ifdef LINUX
    const uint32_t INVAL_ID_S = 0xfffffffeU;
	const uint64_t INVAL_ID_L = 0xfffffffffffffffeULL;
#else
    const uint32_t INVAL_ID_S = 0xfffffffe;
	const uint64_t INVAL_ID_L = 0xfffffffffffffffe;
#endif

	const char HIGH = 1;
	const char LOW  = 0;
}



class AMAuthId {

 private:
	enum { undef, steam, steam2, valve, valve2 };
	
 public:
	// Constructors
	AMAuthId() : m_bIdIsSet(false), m_bIsWonid(true), m_cAuthidType(undef), m_uiWonid(nsAuthid::INVAL_ID_S), m_uiAuthid64(nsAuthid::INVAL_ID_L)  {};
	AMAuthId( uint32_t w) : m_bIdIsSet(true), m_bIsWonid(true), m_cAuthidType(undef), m_uiWonid(w), m_uiAuthid64(nsAuthid::INVAL_ID_L)    {};
	AMAuthId( uint32_t w, uint64_t s) : m_bIdIsSet(true), m_bIsWonid(false), m_cAuthidType(undef), m_uiWonid(w), m_uiAuthid64(s)   {};
	AMAuthId( uint32_t w, uint32_t s1, uint32_t s2) : m_bIdIsSet(true), m_bIsWonid(false), m_cAuthidType(undef), m_uiWonid(w) 
	{
		m_uiAuthid32[nsAuthid::HIGH] = s1;
		m_uiAuthid32[nsAuthid::LOW]  = s2;
	};
	AMAuthId( const unsigned char* _pcID ) { f_parse_id( reinterpret_cast<const char*>(_pcID) ); };
	AMAuthId( const char* _pcID )          { f_parse_id( _pcID ); };



	// Copy constructor
	AMAuthId( const AMAuthId& rhs ) {
		m_uiWonid = rhs.m_uiWonid;
		m_uiAuthid64 = rhs.m_uiAuthid64;
		m_bIsWonid = rhs.m_bIsWonid;
		m_cAuthidType = rhs.m_cAuthidType;
		m_bIdIsSet = rhs.m_bIdIsSet;
	}



	// Destructor
	~AMAuthId() {};



	// Assignment operators
	AMAuthId& operator=( const AMAuthId& rhs ) {
		if ( &rhs == this ) return *this;  // prevent self-assignment
		m_uiWonid = rhs.m_uiWonid;
		m_uiAuthid64 = rhs.m_uiAuthid64;
		m_bIsWonid = rhs.m_bIsWonid;
		m_cAuthidType = rhs.m_cAuthidType;
		m_bIdIsSet = rhs.m_bIdIsSet;
		return *this;
	}

	AMAuthId& operator=( const int rhs ) {
		m_uiWonid = rhs;
		m_uiAuthid64 = nsAuthid::INVAL_ID_L;
		m_cAuthidType = undef;
		m_bIsWonid = true;
		m_bIdIsSet = true;
		return *this;
	}

	AMAuthId& operator=( const unsigned int rhs ) {
		m_uiWonid = rhs;
		m_uiAuthid64 = nsAuthid::INVAL_ID_L;
		m_cAuthidType = undef;
		m_bIsWonid = true;
		m_bIdIsSet = true;
		return *this;
	}

#if !(defined LINUX) && (_MSC_VER < 1300)
	AMAuthId& operator=( const int32_t rhs ) {
		m_uiWonid = rhs;
		m_uiAuthid64 = nsAuthid::INVAL_ID_L;
		m_cAuthidType = undef;
		m_bIsWonid = true;
		m_bIdIsSet = true;
		return *this;
	}

	AMAuthId& operator=( const uint32_t rhs ) {
		m_uiWonid = rhs;
		m_uiAuthid64 = nsAuthid::INVAL_ID_L;
		m_cAuthidType = undef;
		m_bIsWonid = true;
		m_bIdIsSet = true;
		return *this;
	}
#endif

	AMAuthId& operator=( const unsigned char* _pcID ) { f_parse_id( reinterpret_cast<const char*>(_pcID) ); return *this; };
	AMAuthId& operator=( const char* _pcID )          { f_parse_id(_pcID ); return *this; };


	// Conversions
	operator int() const               { return static_cast<int>(m_uiWonid); };
	operator unsigned int() const      { return m_uiWonid; };
	operator long int() const          { return static_cast<long int>(m_uiWonid); };
	operator unsigned long int() const { return m_uiWonid; };
	operator bool() const { 
		if ( !m_bIdIsSet ) return false;
		if ( m_bIsWonid ) return ( m_uiWonid != 0 );
		else return ( m_uiWonid != 0 && m_uiAuthid64 != 0 && m_cAuthidType != undef );
	}
	operator const char*() const;


	// Comparison operators
	friend bool operator==( const AMAuthId& lhs, const AMAuthId& rhs );
	friend bool operator==(uint32_t  lhs, const AMAuthId& rhs );
	friend bool operator==( const AMAuthId& lhs, uint32_t  rhs );
	friend bool operator==(int32_t   lhs, const AMAuthId& rhs );
	friend bool operator==( const AMAuthId& lhs, int32_t   rhs );
	friend bool operator==( const char*     lhs, const AMAuthId& rhs );
	friend bool operator==( const AMAuthId& lhs, const char*     rhs );

	friend bool operator!=( const AMAuthId& lhs, const AMAuthId& rhs );
	friend bool operator!=(uint32_t  lhs, const AMAuthId& rhs );
	friend bool operator!=( const AMAuthId& lhs, uint32_t  rhs );
	friend bool operator!=(int32_t   lhs, const AMAuthId& rhs );
	friend bool operator!=( const AMAuthId& lhs, int32_t   rhs );
	friend bool operator!=( const char*     lhs, const AMAuthId& rhs );
	friend bool operator!=( const AMAuthId& lhs, const char*     rhs );

	friend bool operator>=( const AMAuthId& lhs, const AMAuthId& rhs );
	friend bool operator>=(uint32_t  lhs, const AMAuthId& rhs );
	friend bool operator>=( const AMAuthId& lhs, uint32_t  rhs );
	friend bool operator>=(int32_t   lhs, const AMAuthId& rhs );
	friend bool operator>=( const AMAuthId& lhs, int32_t   rhs );
	friend bool operator>=( const char*     lhs, const AMAuthId& rhs );
	friend bool operator>=( const AMAuthId& lhs, const char*     rhs );

	friend bool operator<=( const AMAuthId& lhs, const AMAuthId& rhs );
	friend bool operator<=(uint32_t  lhs, const AMAuthId& rhs );
	friend bool operator<=( const AMAuthId& lhs, uint32_t  rhs );
	friend bool operator<=(int32_t   lhs, const AMAuthId& rhs );
	friend bool operator<=( const AMAuthId& lhs, int32_t   rhs );
	friend bool operator<=( const char*     lhs, const AMAuthId& rhs );
	friend bool operator<=( const AMAuthId& lhs, const char*     rhs );

	friend bool operator> ( const AMAuthId& lhs, const AMAuthId& rhs ) ;
	friend bool operator> (uint32_t  lhs, const AMAuthId& rhs ) ;
	friend bool operator> ( const AMAuthId& lhs, uint32_t  rhs ) ;
	friend bool operator> (int32_t   lhs, const AMAuthId& rhs ) ;
	friend bool operator> ( const AMAuthId& lhs, int32_t   rhs ) ;
	friend bool operator> ( const char*     lhs, const AMAuthId& rhs ) ;
	friend bool operator> ( const AMAuthId& lhs, const char*     rhs ) ;

	friend bool operator< ( const AMAuthId& lhs, const AMAuthId& rhs ) ;
	friend bool operator< (uint32_t  lhs, const AMAuthId& rhs ) ;
	friend bool operator< ( const AMAuthId& lhs, uint32_t  rhs ) ;
	friend bool operator< (int32_t   lhs, const AMAuthId& rhs ) ;
	friend bool operator< ( const AMAuthId& lhs, int32_t   rhs ) ;
	friend bool operator< ( const char*     lhs, const AMAuthId& rhs ) ;
	friend bool operator< ( const AMAuthId& lhs, const char*     rhs ) ;



	// Read access functions
	uint32_t wonid() const    { return m_uiWonid; }
	uint32_t authid_a() const { return m_uiWonid; }
	uint64_t authid_b() const { return m_uiAuthid64; }

	uint32_t authid_x() const { return m_uiWonid; }
	uint32_t authid_y() const { return m_uiAuthid32[nsAuthid::HIGH]; }
	uint32_t authid_z() const { return m_uiAuthid32[nsAuthid::LOW];  }

	bool is_wonid() const   { return ( m_bIdIsSet &&  m_bIsWonid ); };
	bool is_authid() const  { return ( m_bIdIsSet && !m_bIsWonid ); };
	bool is_steamid() const { return ( m_bIdIsSet && !m_bIsWonid && (m_cAuthidType == steam || m_cAuthidType == steam2) ); };
	bool is_valveid() const { return ( m_bIdIsSet && !m_bIsWonid && (m_cAuthidType == valve || m_cAuthidType == valve2) ); };

	bool is_set() const    { return m_bIdIsSet; };
	bool is_botid() const  { return (  m_bIsWonid && m_uiWonid == nsAuthid::BOT_ID  ); };
	bool is_lanid() const  { return (  m_bIsWonid && m_uiWonid == nsAuthid::LAN_ID  ); };
	bool is_loopid() const { return ( !m_bIsWonid && m_uiWonid == nsAuthid::LOOP_ID ); };

	
	// utility functions
	static bool is_authid( const char* _pcID ) {
		if ( _pcID == nullptr || *_pcID == '\0' ) return false;
		return ( ((_pcID[0] == 'S' && _pcID[1] == 'T' && _pcID[2] == 'E' && 
				   _pcID[3] == 'A' && _pcID[4] == 'M')
				  ||	
		          (_pcID[0] == 'V' && _pcID[1] == 'A' && _pcID[2] == 'L' && 
				   _pcID[3] == 'V' && _pcID[4] == 'E')) 
				  && _pcID[5] == '_' && _pcID[6] >= 0x30 && _pcID[6] <= 0x39 );
	};

	static bool is_steamid( const char* _pcID ) {
		if ( _pcID == nullptr || *_pcID == '\0' ) return false;
		return ( _pcID[0] == 'S' && _pcID[1] == 'T' && _pcID[2] == 'E' && 
				 _pcID[3] == 'A' && _pcID[4] == 'M' && _pcID[5] == '_' && 
				 _pcID[6] >= 0x30 && _pcID[6] <= 0x39 );
	};

	static bool is_valveid( const char* _pcID ) {
		if ( _pcID == nullptr || *_pcID == '\0' ) return false;
		return ( _pcID[0] == 'V' && _pcID[1] == 'A' && _pcID[2] == 'L' && 
				 _pcID[3] == 'V' && _pcID[4] == 'E' && _pcID[5] == '_' && 
				 _pcID[6] >= 0x30 && _pcID[6] <= 0x39 );
	};

	static bool is_botid( const char* _pcID ) {
		const int BOT = 0x00544f42;
		if ( _pcID == nullptr || *_pcID == '\0' ) return false;
		return ( BOT == *(reinterpret_cast<const int*>(_pcID)) );
	};

	// utility functions
	static bool is_pending( const char* _pcID ) {
		if ( _pcID == nullptr || *_pcID == '\0' ) return false;
		const int STEA = 0x41455453;
		const int M_ID = 0x44495f4d;
		const int VALV = 0x564c4156;
		const int E_ID = 0x44495f45;
		const int uPEN = 0x4e45505f;
		const int DING = 0x474e4944;
		const int uLAN = 0x4e414c5f;

		const int* pcI = reinterpret_cast<const int*>(_pcID);
		return (   ( (*pcI == VALV && *(pcI+1) == E_ID) || (*pcI == STEA && *(pcI+1) == M_ID)     ) 
				&& (  *(pcI+2) == uLAN                  || (*(pcI+2) == uPEN && *(pcI+3) == DING) ) );
	};


	static bool is_loopid( const char* _pcID ) {
		if ( _pcID == nullptr || *_pcID == '\0' ) return false;
		const int STEA = 0x41455453;
		const int M_ID = 0x44495f4d;
		const int VALV = 0x564c4156;
		const int E_ID = 0x44495f45;
		const int uLOO = 0x4f4f4c5f;
		const int PBAC = 0x43414250;

		const int* pcI = reinterpret_cast<const int*>(_pcID);
		return (   ( (*pcI == VALV && *(pcI+1) == E_ID) || (*pcI == STEA && *(pcI+1) == M_ID) ) 
				&& (*(pcI+2) == uLOO) && (*(pcI+3) == PBAC) );
	};


	static bool is_lanid( const char* _pcID ) {
		if ( _pcID == nullptr || *_pcID == '\0' ) return false;
		const int STEA = 0x41455453;
		const int M_ID = 0x44495f4d;
		const int VALV = 0x564c4156;
		const int E_ID = 0x44495f45;
		const int uLAN = 0x4e414c5f;

		const int* pcI = reinterpret_cast<const int*>(_pcID);
		return (   ( (*pcI == VALV && *(pcI+1) == E_ID) || (*pcI == STEA && *(pcI+1) == M_ID) ) 
				&& (*(pcI+2) == uLAN) );
	};


 protected:	
	// utility function
	static bool same_id_type_class ( const AMAuthId& lhs, const AMAuthId& rhs ) {
		return (    ( lhs.m_cAuthidType == rhs.m_cAuthidType )
				 || ( lhs.m_cAuthidType == steam  && rhs.m_cAuthidType == steam2 )
				 || ( lhs.m_cAuthidType == steam2 && rhs.m_cAuthidType == steam  )
				 || ( lhs.m_cAuthidType == valve  && rhs.m_cAuthidType == valve2 )
				 || ( lhs.m_cAuthidType == valve2 && rhs.m_cAuthidType == valve  ) );
	};


 private:

	// members
	bool     m_bIdIsSet;
	bool     m_bIsWonid;
	char     m_cAuthidType;
	uint32_t m_uiWonid;
	union {
		uint64_t m_uiAuthid64;
		uint32_t m_uiAuthid32[2];
		unsigned char m_uiAuthid8[8];
	};

	static char m_acString[nsAuthid::STATIC_LEN];

	void f_parse_id( const char* _pcID );
	void f_assemble_string() const;

};

//
// f_assemble_string()
//
// We need this first so that we can inline it in the const char* operator.
//
inline void AMAuthId::f_assemble_string() const {

	if ( !m_bIdIsSet ) {
		m_acString[0] = '\0';

	} else if ( m_bIsWonid ) {
		snprintf( m_acString, nsAuthid::STATIC_LEN, "%u", m_uiWonid );

	} else {
		if ( m_uiWonid == nsAuthid::LOOP_ID && m_uiAuthid64 == nsAuthid::INVAL_ID_L ) {
		    if ( m_cAuthidType == valve ) {
			    snprintf( m_acString, nsAuthid::STATIC_LEN, "VALVE_ID_LOOPBACK" );
			} else {
			    snprintf( m_acString, nsAuthid::STATIC_LEN, "STEAM_ID_LOOPBACK" );
			}
		} else if ( m_uiWonid == nsAuthid::LAN_ID && m_uiAuthid64 == nsAuthid::INVAL_ID_L ) {
		    if ( m_cAuthidType == valve ) {
			    snprintf( m_acString, nsAuthid::STATIC_LEN, "VALVE_ID_LAN" );
			} else {
			    snprintf( m_acString, nsAuthid::STATIC_LEN, "STEAM_ID_LAN" );
			}
		} else if ( m_uiWonid == nsAuthid::INVAL_ID_S || m_uiAuthid64 == nsAuthid::INVAL_ID_L ) {
			snprintf( m_acString, nsAuthid::STATIC_LEN, "AUTH_INVAL:INVAL" );
		} else if ( m_cAuthidType == steam ) {
			snprintf( m_acString, nsAuthid::STATIC_LEN, "STEAM_%u:%llu", m_uiWonid, m_uiAuthid64 );
		} else if ( m_cAuthidType == steam2 ) {
			snprintf( m_acString, nsAuthid::STATIC_LEN, "STEAM_%u:%lu:%lu", m_uiWonid, 
							m_uiAuthid32[nsAuthid::HIGH], m_uiAuthid32[nsAuthid::LOW] );
		} else if ( m_cAuthidType == valve ) {
			snprintf( m_acString, nsAuthid::STATIC_LEN, "VALVE_%u:%llu", m_uiWonid, m_uiAuthid64 );
		} else if ( m_cAuthidType == valve2 ) {
			snprintf( m_acString, nsAuthid::STATIC_LEN, "VALVE_%u:%lu:%lu", m_uiWonid, 
							m_uiAuthid32[nsAuthid::HIGH], m_uiAuthid32[nsAuthid::LOW] );
		} else {
			snprintf( m_acString, nsAuthid::STATIC_LEN, "undef" );
		}

	}  // if-else
}


// 
// operator const char*()
//

inline AMAuthId::operator const char*() const {
	f_assemble_string();
	return m_acString;
}


//
// operator==
//
inline  bool operator==( const AMAuthId& lhs, const AMAuthId& rhs ) {

	if ( !lhs.m_bIdIsSet || !rhs.m_bIdIsSet ) return false;
	if ( lhs.m_bIsWonid ) {
		if ( !rhs.m_bIsWonid ) return false;
		else return (lhs.m_uiWonid == rhs.m_uiWonid);
	} else {
		if ( rhs.m_bIsWonid ) return false;
		else return ( (lhs.m_uiWonid == rhs.m_uiWonid) && (lhs.m_uiAuthid64 == rhs.m_uiAuthid64) && AMAuthId::same_id_type_class(lhs, rhs) );
	}  // if

}

inline  bool operator==( const uint32_t lhs, const AMAuthId& rhs ) {

	if ( !rhs.m_bIdIsSet || !rhs.m_bIsWonid ) return false;
	else return ( lhs == rhs.m_uiWonid );
}

inline  bool operator==( const AMAuthId& lhs, const uint32_t rhs ) {

	if ( !lhs.m_bIdIsSet || !lhs.m_bIsWonid ) return false;
	else return ( lhs.m_uiWonid == rhs );
}

inline  bool operator==( const int32_t lhs, const AMAuthId& rhs ) {

	if ( !rhs.m_bIdIsSet || !rhs.m_bIsWonid ) return false;
	else return ( static_cast<uint32_t>(lhs) == rhs.m_uiWonid );
}

inline  bool operator==( const AMAuthId& lhs, const int32_t rhs ) {

	if ( !lhs.m_bIdIsSet || !lhs.m_bIsWonid ) return false;
	else return ( lhs.m_uiWonid == static_cast<uint32_t>(rhs) );
}

inline  bool operator==( const char* pcLhs, const AMAuthId& rhs ) {

	if ( !rhs.m_bIdIsSet ) return false;
	else {
		const AMAuthId tmp( pcLhs );
		return ( tmp == rhs );
	}  // else
}

inline  bool operator==( const AMAuthId& lhs, const char* pcRhs ) {

	if ( !lhs.m_bIdIsSet ) return false;
	else {
		const AMAuthId tmp( pcRhs );
		return ( lhs == tmp );
	}  // else
}




//
// operator!=
//
inline  bool operator!=( const AMAuthId& lhs, const AMAuthId& rhs ) {

	if ( !lhs.m_bIdIsSet || !rhs.m_bIdIsSet ) return true;
	if ( lhs.m_bIsWonid ) {
		if ( !rhs.m_bIsWonid ) return true;
		else return (lhs.m_uiWonid != rhs.m_uiWonid);
	} else {
		if ( rhs.m_bIsWonid ) return true;
		else return ( (lhs.m_uiWonid != rhs.m_uiWonid) || (lhs.m_uiAuthid64 != rhs.m_uiAuthid64) || (!AMAuthId::same_id_type_class(lhs, rhs)) );
	}  // if

}

inline  bool operator!=( const uint32_t lhs, const AMAuthId& rhs ) {

	if ( !rhs.m_bIdIsSet || !rhs.m_bIsWonid ) return true;
	else return ( lhs != rhs.m_uiWonid );
}

inline  bool operator!=( const AMAuthId& lhs, const uint32_t rhs ) {

	if ( !lhs.m_bIdIsSet || !lhs.m_bIsWonid ) return true;
	else return ( lhs.m_uiWonid != rhs );
}

inline  bool operator!=( const int32_t lhs, const AMAuthId& rhs ) {

	if ( !rhs.m_bIdIsSet || !rhs.m_bIsWonid ) return true;
	else return ( static_cast<uint32_t>(lhs) != rhs.m_uiWonid );
}

inline  bool operator!=( const AMAuthId& lhs, const int32_t rhs ) {

	if ( !lhs.m_bIdIsSet || !lhs.m_bIsWonid ) return true;
	else return ( lhs.m_uiWonid != static_cast<uint32_t>(rhs) );
}

inline  bool operator!=( const char* pcLhs, const AMAuthId& rhs ) {

	if ( !rhs.m_bIdIsSet ) return true;
	else {
		const AMAuthId tmp( pcLhs );
		return ( tmp != rhs );
	}  // else
}

inline  bool operator!=( const AMAuthId& lhs, const char* pcRhs ) {

	if ( !lhs.m_bIdIsSet ) return true;
	else {
		const AMAuthId tmp( pcRhs );
		return ( lhs != tmp );
	}  // else
}



//
// operator>=
//
inline  bool operator>=( const AMAuthId& lhs, const AMAuthId& rhs ) {

	if ( !lhs.m_bIdIsSet || !rhs.m_bIdIsSet ) return false;
	if ( lhs.m_bIsWonid ) {
		if ( !rhs.m_bIsWonid ) return false;
		else return (lhs.m_uiWonid >= rhs.m_uiWonid);
	} else {
		if ( rhs.m_bIsWonid ) return false;
		else if ( ! AMAuthId::same_id_type_class(lhs, rhs) ) return false;
		else return ( (lhs.m_uiWonid >= rhs.m_uiWonid) && (lhs.m_uiAuthid64 >= rhs.m_uiAuthid64) );
	}  // if

}

inline  bool operator>=( const uint32_t lhs, const AMAuthId& rhs ) {

	if ( !rhs.m_bIdIsSet || !rhs.m_bIsWonid ) return false;
	else return ( lhs >= rhs.m_uiWonid );
}

inline  bool operator>=( const AMAuthId& lhs, const uint32_t rhs ) {

	if ( !lhs.m_bIdIsSet || !lhs.m_bIsWonid ) return false;
	else return ( lhs.m_uiWonid >= rhs );
}

inline  bool operator>=( const int32_t lhs, const AMAuthId& rhs ) {

	if ( !rhs.m_bIdIsSet || !rhs.m_bIsWonid ) return false;
	else return ( static_cast<uint32_t>(lhs) >= rhs.m_uiWonid );
}

inline  bool operator>=( const AMAuthId& lhs, const int32_t rhs ) {

	if ( !lhs.m_bIdIsSet || !lhs.m_bIsWonid ) return false;
	else return ( lhs.m_uiWonid >= static_cast<uint32_t>(rhs) );
}

inline  bool operator>=( const char* pcLhs, const AMAuthId& rhs ) {

	if ( !rhs.m_bIdIsSet ) return false;
	else {
		const AMAuthId tmp( pcLhs );
		return ( tmp >= rhs );
	}  // else
}

inline  bool operator>=( const AMAuthId& lhs, const char* pcRhs ) {

	if ( !lhs.m_bIdIsSet ) return false;
	else {
		const AMAuthId tmp( pcRhs );
		return ( lhs >= tmp );
	}  // else
}



//
// operator<=
//
inline  bool operator<=( const AMAuthId& lhs, const AMAuthId& rhs ) {

	if ( !lhs.m_bIdIsSet || !rhs.m_bIdIsSet ) return false;
	if ( lhs.m_bIsWonid ) {
		if ( !rhs.m_bIsWonid ) return false;
		else return (lhs.m_uiWonid <= rhs.m_uiWonid);
	} else {
		if ( rhs.m_bIsWonid ) return false;
		else if ( ! AMAuthId::same_id_type_class(lhs, rhs) ) return false;
		else return ( (lhs.m_uiWonid <= rhs.m_uiWonid) && (lhs.m_uiAuthid64 <= rhs.m_uiAuthid64) );
	}  // if

}

inline  bool operator<=( const uint32_t lhs, const AMAuthId& rhs ) {

	if ( !rhs.m_bIdIsSet || !rhs.m_bIsWonid ) return false;
	else return ( lhs <= rhs.m_uiWonid );
}

inline  bool operator<=( const AMAuthId& lhs, const uint32_t rhs ) {

	if ( !lhs.m_bIdIsSet || !lhs.m_bIsWonid ) return false;
	else return ( lhs.m_uiWonid <= rhs );
}

inline  bool operator<=( const int32_t lhs, const AMAuthId& rhs ) {

	if ( !rhs.m_bIdIsSet || !rhs.m_bIsWonid ) return false;
	else return ( static_cast<uint32_t>(lhs) <= rhs.m_uiWonid );
}

inline  bool operator<=( const AMAuthId& lhs, const int32_t rhs ) {

	if ( !lhs.m_bIdIsSet || !lhs.m_bIsWonid ) return false;
	else return ( lhs.m_uiWonid <= static_cast<uint32_t>(rhs) );
}

inline  bool operator<=( const char* pcLhs, const AMAuthId& rhs ) {

	if ( !rhs.m_bIdIsSet ) return false;
	else {
		const AMAuthId tmp( pcLhs );
		return ( tmp <= rhs );
	}  // else
}

inline  bool operator<=( const AMAuthId& lhs, const char* pcRhs ) {

	if ( !lhs.m_bIdIsSet ) return false;
	else {
		const AMAuthId tmp( pcRhs );
		return ( lhs <= tmp );
	}  // else
}



//
// operator>
//
inline  bool operator>( const AMAuthId& lhs, const AMAuthId& rhs ) {

	if ( !lhs.m_bIdIsSet || !rhs.m_bIdIsSet ) return false;
	if ( lhs.m_bIsWonid ) {
		if ( !rhs.m_bIsWonid ) return false;
		else return (lhs.m_uiWonid > rhs.m_uiWonid);
	} else {
		if ( rhs.m_bIsWonid ) return false;
		else if ( ! AMAuthId::same_id_type_class(lhs, rhs) ) return false;
		else return ( (lhs.m_uiWonid > rhs.m_uiWonid) && (lhs.m_uiAuthid64 > rhs.m_uiAuthid64) );
	}  // if

}

inline  bool operator>( const uint32_t lhs, const AMAuthId& rhs ) {

	if ( !rhs.m_bIdIsSet || !rhs.m_bIsWonid ) return false;
	else return ( lhs > rhs.m_uiWonid );
}

inline  bool operator>( const AMAuthId& lhs, const uint32_t rhs ) {

	if ( !lhs.m_bIdIsSet || !lhs.m_bIsWonid ) return false;
	else return ( lhs.m_uiWonid > rhs );
}

inline  bool operator>( const int32_t lhs, const AMAuthId& rhs ) {

	if ( !rhs.m_bIdIsSet || !rhs.m_bIsWonid ) return false;
	else return ( static_cast<uint32_t>(lhs) > rhs.m_uiWonid );
}

inline  bool operator>( const AMAuthId& lhs, const int32_t rhs ) {

	if ( !lhs.m_bIdIsSet || !lhs.m_bIsWonid ) return false;
	else return ( lhs.m_uiWonid > static_cast<uint32_t>(rhs) );
}

inline  bool operator>( const char* pcLhs, const AMAuthId& rhs ) {

	if ( !rhs.m_bIdIsSet ) return false;
	else {
		const AMAuthId tmp( pcLhs );
		return ( tmp > rhs );
	}  // else
}

inline  bool operator>( const AMAuthId& lhs, const char* pcRhs ) {

	if ( !lhs.m_bIdIsSet ) return false;
	else {
		const AMAuthId tmp( pcRhs );
		return ( lhs > tmp );
	}  // else
}



//
// operator<
//
inline  bool operator<( const AMAuthId& lhs, const AMAuthId& rhs ) {

	if ( !lhs.m_bIdIsSet || !rhs.m_bIdIsSet ) return false;
	if ( lhs.m_bIsWonid ) {
		if ( !rhs.m_bIsWonid ) return false;
		else return (lhs.m_uiWonid < rhs.m_uiWonid);
	} else {
		if ( rhs.m_bIsWonid ) return false;
		else if ( ! AMAuthId::same_id_type_class(lhs, rhs) ) return false;
		else return ( (lhs.m_uiWonid < rhs.m_uiWonid) && (lhs.m_uiAuthid64 < rhs.m_uiAuthid64) );
	}  // if

}

inline  bool operator<( const uint32_t lhs, const AMAuthId& rhs ) {

	if ( !rhs.m_bIdIsSet || !rhs.m_bIsWonid ) return false;
	else return ( lhs < rhs.m_uiWonid );
}

inline  bool operator<( const AMAuthId& lhs, const uint32_t rhs ) {

	if ( !lhs.m_bIdIsSet || !lhs.m_bIsWonid ) return false;
	else return ( lhs.m_uiWonid < rhs );
}

inline  bool operator<( const int32_t lhs, const AMAuthId& rhs ) {

	if ( !rhs.m_bIdIsSet || !rhs.m_bIsWonid ) return false;
	else return ( static_cast<uint32_t>(lhs) < rhs.m_uiWonid );
}

inline  bool operator<( const AMAuthId& lhs, const int32_t rhs ) {

	if ( !lhs.m_bIdIsSet || !lhs.m_bIsWonid ) return false;
	else return ( lhs.m_uiWonid < static_cast<uint32_t>(rhs) );
}

inline  bool operator<( const char* pcLhs, const AMAuthId& rhs ) {

	if ( !rhs.m_bIdIsSet ) return false;
	else {
		const AMAuthId tmp( pcLhs );
		return ( tmp < rhs );
	}  // else
}

inline  bool operator<( const AMAuthId& lhs, const char* pcRhs ) {

	if ( !lhs.m_bIdIsSet ) return false;
	else {
		const AMAuthId tmp( pcRhs );
		return ( lhs < tmp );
	}  // else
}






#endif /* AM_AUTHID_H */
