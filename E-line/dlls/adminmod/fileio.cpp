/*
 * ===========================================================================
 *
 * $Id: fileio.cpp,v 1.4 2004/01/08 20:07:50 darope Exp $
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


#include "fileio.h"
#include "amutil.h"
#include "users.h"

#include <stdio.h>

CHandles g_Handles;

const int c_LineLen = LARGE_BUF_SIZE;

/* Read a line by searching for an existing handle to the file. If none
 * is found then open a new one and add it to the front of the list */
bool CHandles::ReadLine(char* strFileName, int n, char* strLine) {

	char szFilePath[PATH_MAX];
	if ( get_file_path(szFilePath, strFileName, PATH_MAX, "file_access_read") <= 0 ) {
		return false;
	}

	m_fDirty=true;
	FlushWrite(szFilePath);

	readhandlelist::const_iterator it;
	for(it=m_ReadHandles.begin();it!=m_ReadHandles.end();it++) {
		if ((*it)->Name() == szFilePath) {
			return (*it)->ReadLine(n,strLine);
		}
	}

	CReadHandle* pHandle = new CReadHandle();
	if (pHandle->Open(szFilePath)) {
		m_ReadHandles.push_front(pHandle);
		return pHandle->ReadLine(n,strLine);
	}
	else {
		delete pHandle;
		return false;
	}
}

bool CHandles::WriteLine(char* strFileName, int n, const char* strLine)
{
	m_fDirty=true;

	char szFilePath[PATH_MAX];
	if ( get_file_path(szFilePath, strFileName, PATH_MAX, "file_access_write") <= 0 ) {
		return false;
	}
	FlushRead(szFilePath);

	writehandlelist::const_iterator it;
	bool fFlush=false;

	for(it=m_WriteHandles.begin();it!=m_WriteHandles.end();it++) {
		if ((*it)->Name() == szFilePath) {
			if (n>-1) {
				if ((*it)->AppendMode()) {
					//We are writing to a numbered line in a file we
					//have initially opened in appened mode
					fFlush=true;
					break;
				}
				if ((n-1)<=(*it)->Line()) {
					//We have already written past this line so we must close
					//the file and start again from the beginning
					fFlush=true;
					break;
				}
			}
			return (*it)->WriteLine(n,strLine);
		}
	}

	if (fFlush) {
		FlushWrite(szFilePath);
	}

	CWriteHandle* pHandle = new CWriteHandle();
	if (pHandle->Open(szFilePath,n,m_nFileIndex++)) {
		m_WriteHandles.push_front(pHandle);
		return pHandle->WriteLine(n,strLine);
	}
	else {
		delete pHandle;
		return false;
	}
}


/* Remove all open handles - called after running any AMX routine */
void CHandles::FlushAll() {
	readhandlelist::iterator itRead;
	for(itRead=m_ReadHandles.begin();itRead!=m_ReadHandles.end();itRead++) {
		delete *itRead;
	}
	m_ReadHandles.clear();

	writehandlelist::iterator itWrite;
	for(itWrite=m_WriteHandles.begin();itWrite!=m_WriteHandles.end();itWrite++) {
		delete *itWrite;
	}
	m_WriteHandles.clear();

	m_fDirty=false;
}

/* Remove a specific read handle - called before writing to
 * the same file */
void CHandles::FlushRead(const char* szFileName) {

	readhandlelist::iterator it;
	for (it=m_ReadHandles.begin();it!=m_ReadHandles.end();it++) {
		if ((*it)->Name() == szFileName) {
			delete *it;
			m_ReadHandles.remove(*it);
			return;
		}
	}

}

/* Remove a specific write handle - called before reading from
 * the same file or when writing to an earlier line than the last write*/
void CHandles::FlushWrite(const char* szFileName) {

	writehandlelist::iterator it;
	for (it=m_WriteHandles.begin();it!=m_WriteHandles.end();it++) {
		if ((*it)->Name() == szFileName) {
			delete *it;
			m_WriteHandles.remove(*it);
			return;
		}
	}

}


CReadHandle::CReadHandle() {
	m_nPos=0;
	m_fFile = NULL;
}

bool CReadHandle::Open(char* szFileName) {
	m_nPos=0;

	if( (m_fFile = fopen(szFileName,"r")) == NULL ) {
		return false;
	}
	m_strFileName = szFileName;
	return true;

}

bool CReadHandle::ReadLine(int n, char* strLine)
{
	if ( NULL == m_fFile ) return false;

	/* If caller is reading for a prior position in the file,
    jump to the start and reread */
	if (n<=m_nPos) {
		rewind(m_fFile);
		m_nPos=0;
	}

	for ( /*m_nPos is already at the correct starting index */; m_nPos < n; m_nPos++ ) {
		memset( strLine, 0x0, c_LineLen );
		strLine = fgets( strLine, c_LineLen, m_fFile );
		if( strLine == NULL ) {
			fclose( m_fFile );
			m_fFile = NULL;
			return false;
		}  // if
	}  // for

	char* pcLine = strrchr( strLine, '\n' );
	if ( pcLine ) *pcLine = 0;
	pcLine = strrchr( strLine, '\r' );
	if ( pcLine ) *pcLine = 0;

	return true;
}

CReadHandle::~CReadHandle()
{
	if (m_fFile) {
		fclose(m_fFile);
		m_fFile = NULL;
	}
}


CWriteHandle::CWriteHandle() {
	m_nPos=0;
	m_fFile = NULL;
	m_fTempFile = NULL;
	m_fAppend = false;
}

bool CWriteHandle::Open(char* szFileName,int nLineNo, int nFileIndex) {
	m_nPos=0;

	if ( NULL != m_fFile ) {
		fflush( m_fFile );
		fclose( m_fFile );
	}

	if( (m_fFile = fopen(szFileName,"a+")) == NULL ) {
		DEBUG_LOG( 2, ("ERROR:writefile: fopen failed on filename %s: %s.", szFileName, strerror(errno)) );
		return false;
	}


	if( nLineNo == -1 ) { // default is to just append
		m_fAppend=true;
	}
	else {
	    char szTempFilePath[PATH_MAX];
		sprintf(szTempFilePath,"%s.%i.tmp",szFileName,nFileIndex);

	    if( (m_fTempFile = fopen(szTempFilePath,"w+")) == NULL ) {
			DEBUG_LOG( 2, ("ERROR:writefile: failed to create temporary file.") );
			return false;
		}

		m_strTempFileName = szTempFilePath;
		m_fAppend=false;
	}

	m_nPos=0;
	m_strFileName = szFileName;
	return true;
}

bool CWriteHandle::WriteLine(int nLineNo,const char* strLine) {
	const char* pcEOL = "\n";

	if ( NULL == m_fFile ) return false;

	if (m_fAppend) {
		fprintf( m_fFile, "%s%s", strLine, pcEOL);
		return true;
	}

	char szLine[c_LineLen];
	nLineNo--; //Adminmod uses 1-based line numbers, C++ uses 0

	while (((m_nPos<nLineNo) || (nLineNo<0)) && !feof( m_fFile ) ) {
		memset( szLine, 0, c_LineLen );
		if ( fgets(szLine, c_LineLen, m_fFile) != NULL) {
			if (strchr(szLine,'\n')) {
				m_nPos++;
			}
			fputs( szLine, m_fTempFile );
		}
	}

	 fprintf( m_fTempFile, "%s%s", strLine, pcEOL);

	 /* Now skip over the line we have "replaced" in the file */
	 m_nPos++;
	 do {
		if (fgets(szLine, c_LineLen, m_fFile) == NULL) break;
	 } while (strchr(szLine,'\n')==NULL);
	 return true;
}


CWriteHandle::~CWriteHandle()
{
	if ((m_nPos>0 ) && (m_fAppend==0)) {
		//Temporary file has been used - copy any remaining
		//lines from the original
		char szLine[c_LineLen];
		while (!feof( m_fFile ) ) {
			memset( szLine, 0, c_LineLen );
			if ( fgets(szLine, c_LineLen, m_fFile) != NULL) {
				m_nPos++;
				fputs( szLine, m_fTempFile );
			}
		}
	}

	if (m_fFile) {
		fclose(m_fFile);
		m_fFile = NULL;
	}

	if (m_fTempFile) {
		fclose(m_fTempFile);
	}

	if ((m_nPos>0 ) && (m_fAppend==0)) {
		//Temporary file has been used - replace the
		//the original with it
		unlink(m_strFileName.c_str());
		rename(m_strTempFileName.c_str(),m_strFileName.c_str());
	}

}

