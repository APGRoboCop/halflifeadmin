/*
 * ===========================================================================
 *
 * $Id: CLinkList.cpp,v 1.2 2003/03/26 20:43:54 darope Exp $
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
 * links are destroyed. 
 *
 */

#include "CLinkList.h"

/* CLinkItem */

/* Constructor and Destructor moved to header file, blame MSVC.

template<class T, bool isArray> inline	CLinkItem<T,isArray>::CLinkItem(){ 
	m_pNext = 0; 
	m_pData = 0;
};

template<class T, bool isArray> inline CLinkItem<T,isArray>::~CLinkItem(){
	if (m_pData != 0) {
		if ( isArray ) {
			delete[] m_pData;
		 } else {
           delete(m_pData);
		 }
	}
    m_pData = 0; 
};

*/

template<class T, bool isArray> inline T* CLinkItem<T,isArray>::Data() {
  return m_pData;
}

template<class T, bool isArray> inline void CLinkItem<T,isArray>::SetData(T* pData) {
  m_pData = pData;
}

template<class T, bool isArray> inline CLinkItem<T,isArray>* CLinkItem<T,isArray>::NextLink() {
  return m_pNext;
}

template<class T, bool isArray> inline void CLinkItem<T,isArray>::SetNextLink(CLinkItem<T,isArray>* pNextLink) {
  m_pNext = pNextLink;
}

/* CLinkList */
/* Constructor and Destructor moved to header file. I hate you, Kenny.
template<class T, bool isArray> inline CLinkList<T,isArray>::CLinkList() {
  m_pFirst = 0;
}

template<class T, bool isArray> inline CLinkList<T,isArray>::~CLinkList() {
  Init();
}
*/

template<class T, bool isArray> int CLinkList<T,isArray>::AddLink(T* pData, CLinkItem<T,isArray>* pInsertBefore) {
  if (m_pFirst == NULL && pInsertBefore != NULL) {
    return 0;
  } else if ( (m_pFirst == NULL) || (m_pFirst == pInsertBefore) ) {
    CLinkItem<T,isArray>* pLink = new CLinkItem<T,isArray>();
    pLink->SetData(pData);
    pLink->SetNextLink(m_pFirst);
    m_pFirst = pLink;
  } else {
    CLinkItem<T,isArray>* pCurrent = m_pFirst;
    while (pCurrent != NULL && pCurrent->NextLink() != pInsertBefore && pCurrent->NextLink() != 0) {
      pCurrent = pCurrent->NextLink();
    }
    if (pCurrent == NULL) {
      return 0;
    } else if (pInsertBefore != NULL && pCurrent->NextLink() != pInsertBefore) {
      return 0;
    } else {
      CLinkItem<T,isArray>* pLink = new CLinkItem<T,isArray>();
      pLink->SetData(pData);
      pLink->SetNextLink(pCurrent->NextLink());
      pCurrent->SetNextLink(pLink);
    }
  }
  return 1;
}

template<class T, bool isArray> int CLinkList<T,isArray>::Count() {
  int iCount = 0;
  
  if (m_pFirst == NULL) 
    return 0;
  
  CLinkItem<T,isArray>* pCurrent = m_pFirst;
  while (pCurrent != NULL) {
    pCurrent = pCurrent->NextLink();
    iCount++;
  }
  return iCount;
}

template<class T, bool isArray> int CLinkList<T,isArray>::DeleteLink(CLinkItem<T,isArray>* pLink) {
  if (pLink == NULL || m_pFirst == NULL) {
    return 0;
  } else if (m_pFirst == pLink) {
    m_pFirst = m_pFirst->NextLink();
    delete(pLink);
    pLink = 0;
  } else {
    CLinkItem<T,isArray>* pCurrent = m_pFirst;
    while (pCurrent != NULL && pCurrent->NextLink() != pLink && pCurrent->NextLink() != 0) {
      pCurrent = pCurrent->NextLink();
    }
    if (pCurrent == NULL || pCurrent->NextLink() != pLink) {
      return 0;
    }
    pCurrent->SetNextLink(pLink->NextLink());
    delete(pLink);
    pLink = 0;
  }
  return 1;
}

template<class T, bool isArray> CLinkItem<T,isArray>* CLinkList<T,isArray>::FindLink(T* pData) {
  if (m_pFirst == NULL) {
    return NULL;
  }
  CLinkItem<T,isArray>* pCurrent = m_pFirst;
  while (pCurrent != NULL && pCurrent->Data() != pData) {
    pCurrent = pCurrent->NextLink();
  }
  if (pCurrent == NULL) {
    return NULL;
  }
  return pCurrent;
}

template<class T, bool isArray> inline CLinkItem<T,isArray>* CLinkList<T,isArray>::FirstLink() {
  return m_pFirst;
}

template<class T, bool isArray> void CLinkList<T,isArray>::Init() {
  if (m_pFirst == NULL) {
    return;
  }
  CLinkItem<T,isArray>* pLink = m_pFirst;
  CLinkItem<T,isArray>* pOldLink = NULL;
  m_pFirst = 0;
  while (pLink != NULL) {
    pOldLink = pLink;
    pLink = pLink->NextLink();
    pOldLink->SetNextLink(0);
    delete(pOldLink);
    pOldLink = 0;
  }
}
