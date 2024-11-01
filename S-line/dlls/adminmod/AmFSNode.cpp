/*
 * ===========================================================================
 *
 * $Id: AmFSNode.cpp,v 1.2 2003/09/28 18:10:35 darope Exp $
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
 * Implementation of the filesystem node and directory classes.
 * A note on directories. When a directory is sorted, the contents will be
 * loaded into memory and then sorted. That means that any changes to the
 * undelying directory will not be reflected when retrieving the directory
 * members with get_next_entry().
 * To resync with the directory on disk, the sort() or set() methods must be
 * called.
 */

#include <cstring>
#include <cstdio>
#include "AmFSNode.h"

#ifdef WIN32
 //#  define snprintf _snprintf
#  define S_ISDIR(a) ((a)&_S_IFDIR)
#  define S_ISREG(a) ((a)&_S_IFREG)
#endif

AmFSNode::AmFSNode()
	: m_acFullpath(""),
	  m_bIsSet(false),
	  m_iFiledes(0),
	  m_oStat(), // Assuming m_acFullpath is a string or similar type [APG]RoboCop[CL]
	  m_poDirectoryObject(nullptr) // Assuming m_oStat has a default constructor
{
}

AmFSNode::AmFSNode(const char* _fullpath)
{
	m_iFiledes = 0;
	strncpy(m_acFullpath, _fullpath, PATH_MAX);
	m_stat_node();
	m_poDirectoryObject = nullptr;
}

AmFSNode::AmFSNode(const char* _path, const char* _nodename)
{
	m_iFiledes = 0;
	snprintf(m_acFullpath, PATH_MAX, "%s/%s", _path, _nodename);
	m_stat_node();
	m_poDirectoryObject = nullptr;
}

AmFSNode::AmFSNode(int _filedes) : m_iFiledes(_filedes)
{
	m_stat_node();
	m_poDirectoryObject = nullptr;
}

AmFSNode::~AmFSNode()
{
	delete m_poDirectoryObject;
}

void AmFSNode::set(const char* _fullpath)
{
	m_iFiledes = 0;
	strncpy(m_acFullpath, _fullpath, PATH_MAX - 1); // Copy up to PATH_MAX - 1 characters [APG]RoboCop[CL]
	m_acFullpath[PATH_MAX - 1] = '\0'; // Null-terminate within bounds
	m_stat_node();
}

void AmFSNode::set(const char* _path, const char* _nodename)
{
	m_iFiledes = 0;
	snprintf(m_acFullpath, PATH_MAX, "%s/%s", _path, _nodename);
	m_stat_node();
}

void AmFSNode::set(int _filedes)
{
	m_iFiledes = _filedes;
	m_stat_node();
}

bool AmFSNode::is_directory() const
{
	if (m_bIsSet) {
		return (S_ISDIR(m_oStat.st_mode) ? true : false);
	}
	else {
		return false;
	}
}

bool AmFSNode::is_file() const
{
	if (m_bIsSet) {
		return (S_ISREG(m_oStat.st_mode) ? true : false);
	}
	else {
		return false;
	}
}

void AmFSNode::name(char* _name, int _max) const
{
	strncpy(_name, m_acFullpath, _max);
}

const char* AmFSNode::name() const
{
	return m_acFullpath;
}

AmDir* AmFSNode::get_directory_handle()
{
	if (!is_directory() || !m_bIsSet) return nullptr;

	if (m_poDirectoryObject != nullptr) {
		delete m_poDirectoryObject;
	}

	m_poDirectoryObject = new AmDir(m_acFullpath);

	return m_poDirectoryObject;
}

void AmFSNode::m_stat_node() {
	int retval;
	if (0 != m_iFiledes) {
		retval = fstat(m_iFiledes, &m_oStat);
	}
	else {
		retval = stat(m_acFullpath, &m_oStat);
	}

	if (0 == retval) {
		m_bIsSet = true;
	}
	else {
		m_bIsSet = false;
	}
}

const char* AmDir::get_next_entry(AmFSNode& _oNode)
{
	if (!is_directory()) return nullptr;

	if (m_next_entry() == 0) return nullptr;

	_oNode.set(m_acFullpath, m_pcDirEntryName);

	return m_pcDirEntryName;
}

const char* AmDir::get_next_entry()
{
	if (!is_directory()) return nullptr;

	if (m_next_entry() == 0) return nullptr;

	return m_pcDirEntryName;
}

#ifdef LINUX
void AmDir::set(const char* _fullpath)
{
	if (m_ptDirHandle != nullptr) closedir(m_ptDirHandle);
	m_free_dir_entries();
	AmFSNode::set(_fullpath);
}

void AmDir::set(const char* _path, const char* _nodename)
{
	if (m_ptDirHandle != nullptr) closedir(m_ptDirHandle);
	m_free_dir_entries();
	AmFSNode::set(_path, _nodename);
}

void AmDir::set(int filedes) {
}

void AmDir::sort()
{
	if (m_ptDirHandle != nullptr) closedir(m_ptDirHandle);
	m_free_dir_entries();

	m_iNumDirEntries = scandir(m_acFullpath, &m_ppoDirEntries, nullptr, alphasort);
	if (m_iNumDirEntries < 0) {
		m_ppoDirEntries = nullptr;
	}
	else {
		m_iCurrDirEntryNum = 0;
	}
}

void AmDir::rewind()
{
	if (m_ppoDirEntries == nullptr) {
		rewinddir(m_ptDirHandle);
	}
	else {
		m_iCurrDirEntryNum = 0;
	}
}

int AmDir::m_next_entry()
{
	// Check if we read from disk and have no directory open
	if (m_ppoDirEntries == nullptr && m_ptDirHandle == nullptr) {
		m_ptDirHandle = opendir(m_acFullpath);
		if (m_ptDirHandle == nullptr) return 0;
	}

readdiragain:
	if (m_ppoDirEntries == nullptr) {
		m_poDirEntry = readdir(m_ptDirHandle);
		if (m_poDirEntry == nullptr) {
			return 0;
		}
	}
	else {
		m_poDirEntry = m_ppoDirEntries[m_iCurrDirEntryNum];
		if (++m_iCurrDirEntryNum > m_iNumDirEntries) return 0;
	}

	if (m_poDirEntry->d_name[0] == '.' && m_poDirEntry->d_name[1] == '\0') goto readdiragain;
	if (m_poDirEntry->d_name[0] == '.' && m_poDirEntry->d_name[1] == '.' && m_poDirEntry->d_name[2] == '\0') goto readdiragain;

	m_pcDirEntryName = m_poDirEntry->d_name;

	return 1;
}

void AmDir::m_free_dir_entries()
{
	if (m_ppoDirEntries != nullptr) {
		while (m_iNumDirEntries--) {
			free(m_ppoDirEntries[m_iNumDirEntries]);
		}
		free(m_ppoDirEntries);
	}
	m_iNumDirEntries = 0;
	m_iCurrDirEntryNum = 0;
}
#endif

#ifdef WIN32
static int __cdecl alphasort_finddata(const void* elem1, const void* elem2)
{
	return _stricmp(((_finddata_t*)elem1)->name, ((_finddata_t*)elem2)->name);
}

void AmDir::set(const char* _fullpath)
{
	if (m_liDirHandle >= 0) _findclose(m_liDirHandle);
	m_free_dir_entries();
	AmFSNode::set(_fullpath);
}

void AmDir::set(const char* _path, const char* _nodename)
{
	if (m_liDirHandle >= 0) _findclose(m_liDirHandle);
	m_free_dir_entries();
	AmFSNode::set(_path, _nodename);
}

void AmDir::set(int filedes) {
	return;
}

void AmDir::sort()
{
	if (m_liDirHandle >= 0) {
		_findclose(m_liDirHandle);
		m_liDirHandle = -1;
	}
	m_free_dir_entries();

	char acGlob[PATH_MAX + 3];

	strcpy(acGlob, m_acFullpath);
	strcat(acGlob, "/*");

	long int liDirHandle = _findfirst(acGlob, &m_oDirEntry);
	if (liDirHandle < 0) return;

	m_iNumDirEntries = 1;

	// count number of entries
	while (_findnext(liDirHandle, &m_oDirEntry) == 0) {
		m_iNumDirEntries++;
	}
	_findclose(liDirHandle);


	if (m_iNumDirEntries == 0) {
		return;
	}

	// if we have any entries, load and save them
	// first, lets make an array
	m_poDirEntries = new _finddata_t[m_iNumDirEntries];
	if (m_poDirEntries == nullptr) {
		m_iNumDirEntries = 0;
		return;
	}

	// now read them all in
	liDirHandle = _findfirst(acGlob, &m_poDirEntries[0]);
	m_iCurrDirEntryNum = 1;
	while (_findnext(liDirHandle, &m_poDirEntries[m_iCurrDirEntryNum++]) == 0);
	_findclose(liDirHandle);

	// and now sort them
	qsort(m_poDirEntries, m_iNumDirEntries, sizeof(struct _finddata_t), alphasort_finddata);

	m_iCurrDirEntryNum = 0;
}

void AmDir::rewind()
{
	if (m_poDirEntries == nullptr) {
		if (m_liDirHandle >= 0) _findclose(m_liDirHandle);
		m_liDirHandle = -1;
	}
	else {
		m_iCurrDirEntryNum = 0;
	}
}

int AmDir::m_next_entry()
{
	struct _finddata_t* poDirEntry = &m_oDirEntry;

	if (m_poDirEntries == nullptr && m_liDirHandle < 0) {
		char acGlob[PATH_MAX + 3];

		strcpy(acGlob, m_acFullpath);
		strcat(acGlob, "/*");

		m_liDirHandle = _findfirst(acGlob, poDirEntry);
		if (m_liDirHandle < 0) return 0;

	}
	else {
	readdiragain:
		if (m_poDirEntries == nullptr) {
			if (_findnext(m_liDirHandle, poDirEntry) < 0) {
				_findclose(m_liDirHandle);
				m_liDirHandle = -1;
				return 0;
			}
		}
		else {
			poDirEntry = &m_poDirEntries[m_iCurrDirEntryNum];
			if (++m_iCurrDirEntryNum > m_iNumDirEntries) return 0;
		}
	}

	if (poDirEntry->name[0] == '.' && poDirEntry->name[1] == '\0') goto readdiragain;
	if (poDirEntry->name[0] == '.' && poDirEntry->name[1] == '.' && poDirEntry->name[2] == '\0') goto readdiragain;

	m_pcDirEntryName = poDirEntry->name;

	return 1;
}

void AmDir::m_free_dir_entries()
{
	if (m_poDirEntries != nullptr) {
		delete[] m_poDirEntries;
	}
	m_iNumDirEntries = 0;
	m_iCurrDirEntryNum = 0;
}

#endif
