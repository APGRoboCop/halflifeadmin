/*
 * ===========================================================================
 *
 * $Id: fileio.h,v 1.3 2004/01/08 20:07:17 darope Exp $
 *
 *
 * Copyright (c) 1999-2003 Alfred Reynolds, Florian Zschocke, Paul Hickman
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
 */


#ifndef AM_FILEIO_H
#define AM_FILEIO_H

#include <string.h>
#include "extdll.h"
#include "amx.h"
#include <sys/types.h>
#include <sys/stat.h>
#ifndef WIN32
#  include <unistd.h>
#endif
#ifdef _WIN32
#  include <io.h>
#  include <direct.h>
#endif

//STL includes
#include <string>
#include <list>
#include <map>

class CReadHandle {

private:
	std::string m_strFileName;
	int m_nPos;
	FILE* m_fFile;

public:
	CReadHandle();
	~CReadHandle();
	bool Open(char* strFileName);
	const std::string& Name() const { return m_strFileName; }
	bool ReadLine(int n,char* strLine);
};

class CWriteHandle {

private:
	std::string m_strFileName;
	std::string m_strTempFileName;
	int m_nPos;
	FILE* m_fFile;
	FILE* m_fTempFile;
	bool m_fAppend;

public:
	CWriteHandle();
	~CWriteHandle();
	bool Open(char* strFileName,int nLineNo, int nFileIndex);
	const std::string& Name() const { return m_strFileName; }
	bool AppendMode() { return m_fAppend; }
	int Line() { return m_nPos; }
	bool WriteLine(int n,const char* strLine);
};



typedef std::list<CReadHandle*> readhandlelist;
typedef std::list<CWriteHandle*> writehandlelist;

class CHandles {

private:
	std::list<CReadHandle*> m_ReadHandles;
	std::list<CWriteHandle*> m_WriteHandles;
	int m_nFileIndex;
	bool m_fDirty;
	void FlushAll();

public:
	CHandles() { m_nFileIndex=0; m_fDirty=false;}
	bool ReadLine(char* strFileName, int n, char* strLine);
	bool WriteLine(char* strFileName, int n, const char* strline);

	void Flush() { if (m_fDirty) FlushAll(); }
	void FlushRead(const char* strFileName);
	void FlushWrite(const char* strFileName);
};

extern CHandles g_Handles;

#endif /* AM_FILEIO_H */

