/*
 * ===========================================================================
 *
 * $Id: AmFSNode.h,v 1.2 2003/09/28 18:10:35 darope Exp $
 *
 *
 * Copyright (c) 1999-2003 Alfred Reynolds, Florian Zschocke
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
 * This file defines a leaky abstraction of filesystem nodes. A node, when created
 * can be tested if it is a directory or a regular file. If it is a diretcory a 
 * directory object handle can be acquired. 
 * The directory object can be used to iterate over the contents of the directory.
 * If the sort() method is used on the directory the contents will be returned 
 * in an alphabetically sorted order.
 *
 */


#ifndef AM_AMNODE_H
#define AM_AMNODE_H


#include <sys/types.h>
#include <sys/stat.h>

#ifdef LINUX
#  include <unistd.h>
#  include <limits.h>
#  include <dirent.h>
#  include <malloc.h>
#endif

#ifdef WIN32
#  include <cstdlib>
#  include <io.h>
#  define PATH_MAX _MAX_PATH
#endif

class AmDir;

class AmFSNode {
public:
	AmFSNode();
	AmFSNode( const char* fullpath );
	AmFSNode( const char* path, const char* nodename );
	AmFSNode( int filedes );
	virtual ~AmFSNode();

	virtual void set( const char* fullpath );
	virtual void set( const char* path, const char* nodename );
	virtual void set( int filedes );
	
	bool is_directory() const;
	bool is_file() const;

	void name( char* name, int max ) const;
	const char* name() const;
	
	AmDir* get_directory_handle();


protected:
	char m_acFullpath[PATH_MAX];


private:

	void m_stat_node();
	
	bool m_bIsSet;
	int m_iFiledes;
	struct stat m_oStat;

	AmDir* m_poDirectoryObject;
	
};




class AmDir : public AmFSNode {
public:

	AmDir();
	AmDir( const char* fullpath );
	AmDir( const char* path, const char* dirname );
	~AmDir() override;

	void set( const char* fullpath ) override;
	void set( const char* path, const char* nodename ) override;

	const char* get_next_entry();
	const char* get_next_entry( AmFSNode& );
	void rewind();

	// Loads the directory content into memory and sorts it. It is the disconnected from the
	// actual directory content so that a change in the directory is not reflected.
	void sort();

	
protected:
	
	int m_next_entry();
	void m_free_dir_entries();
	const char* m_pcDirEntryName;
	
private:

	AmDir( int dirdes ) {}
	void set( int filedes ) override;

    int m_iNumDirEntries;
	int m_iCurrDirEntryNum;

#ifdef LINUX
	DIR* m_ptDirHandle;
	struct dirent* m_poDirEntry;
	struct dirent** m_ppoDirEntries;
#endif

#ifdef WIN32
    long int m_liDirHandle;
    struct _finddata_t  m_oDirEntry;
	struct _finddata_t* m_poDirEntries;
#endif

};


#ifdef LINUX
inline AmDir::AmDir() : AmFSNode() 
{
    m_ptDirHandle = nullptr; 
	m_pcDirEntryName = nullptr;
	m_ppoDirEntries = nullptr;
}


inline AmDir::AmDir( const char* fullpath ) : AmFSNode(fullpath) 
{
    m_ptDirHandle = nullptr; 
	m_pcDirEntryName = nullptr;
	m_ppoDirEntries = nullptr;
}


inline AmDir::AmDir( const char* path, const char* dirname ) : AmFSNode(path, dirname) 
{
    m_ptDirHandle = nullptr; 
	m_pcDirEntryName = nullptr;
	m_ppoDirEntries = nullptr;
}


inline AmDir::~AmDir() 
{
    if ( m_ptDirHandle != nullptr ) closedir( m_ptDirHandle ); 
	m_free_dir_entries();
}
#endif




#ifdef WIN32
inline AmDir::AmDir()
	:
	m_pcDirEntryName(nullptr),
	m_iNumDirEntries(0),
	m_iCurrDirEntryNum(0),
	m_liDirHandle(-1),
	m_oDirEntry(),
	m_poDirEntries(nullptr) // Assuming m_oDirEntry has a default constructor
{
}


inline AmDir::AmDir(const char* fullpath)
	: AmFSNode(fullpath),
	  m_pcDirEntryName(nullptr),
	  m_iNumDirEntries(0),
	  m_iCurrDirEntryNum(0),
	  m_liDirHandle(-1), // Initialize m_iNumDirEntries
	  m_oDirEntry(), // Initialize m_iCurrDirEntryNum
	  m_poDirEntries(nullptr) // Initialize m_oDirEntry (assuming it has a default constructor)
{
}


inline AmDir::AmDir(const char* path, const char* dirname)
	: AmFSNode(path, dirname),
	m_pcDirEntryName(nullptr),
	m_iNumDirEntries(0),
	m_iCurrDirEntryNum(0),
	m_liDirHandle(-1), // Initialize m_iNumDirEntries
	m_oDirEntry(), // Initialize m_iCurrDirEntryNum
	m_poDirEntries(nullptr) // Initialize m_oDirEntry (assuming it has a default constructor)
{
}

inline AmDir::~AmDir() 
{ 
    if ( m_liDirHandle >= 0 ) _findclose( m_liDirHandle ); 
	m_free_dir_entries();
}
#endif




#endif // AM_AMNODE_H
