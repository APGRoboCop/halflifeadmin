/*
 * ===========================================================================
 *
 * $Id: CLinkList.h,v 1.2 2003/03/26 20:43:56 darope Exp $
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
 * A simple linked list class. Not much in the way of
 * comments needed here.  The CLinkList class holds a linked
 * list of CLinkItem classes, which contain void* data.  The
 * class is nice enough to delete() the void* data when the
 *links are destroyed. 
 *
 */

#pragma once
#include <cstdlib>
#include <cstdio>

template<class T, bool isArray = false> class CLinkItem {

private :
	CLinkItem<T,isArray>* m_pNext;
	T* m_pData;

public:
	CLinkItem(){ m_pNext = 0; m_pData = 0;};
	~CLinkItem(){
       if (m_pData != nullptr) {
         if ( isArray ) {
           delete[] m_pData;
		 } else {
           delete(m_pData);
		 }
	   }
       m_pData = 0; 
	};
	T* Data();
	void SetData(T* pData);
	CLinkItem<T,isArray>* NextLink();
	void SetNextLink(CLinkItem<T,isArray>* pNextLink);
};

template<class T, bool isArray = false> class CLinkList {

private:
	CLinkItem<T,isArray>* m_pFirst;

public:
	CLinkList(){ m_pFirst = 0; };
	~CLinkList(){ Init(); };
	int AddLink(T* pData, CLinkItem<T,isArray>* pInsertBefore = nullptr);
	int Count();
	int DeleteLink(CLinkItem<T,isArray>* pLink);
	CLinkItem<T,isArray>* FindLink(T* pData);
	CLinkItem<T,isArray>* FirstLink();
	void Init();
};
