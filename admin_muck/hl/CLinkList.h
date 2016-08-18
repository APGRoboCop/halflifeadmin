/* $Id: CLinkList.h,v 1.3 2001/05/28 10:23:36 darope Exp $ */

/* A simple linked list class. Not much in the way of
comments needed here.  The CLinkList class holds a linked
list of CLinkItem classes, which contain void* data.  The
class is nice enough to delete() the void* data when the
links are destroyed. */

#include <stdlib.h>
#include <stdio.h>

template<class T, bool isArray = false> class CLinkItem {

private :
	CLinkItem<T,isArray>* m_pNext;
	T* m_pData;

public:
	CLinkItem(){ m_pNext = 0; m_pData = 0;};
	~CLinkItem(){
       if (m_pData != 0) {
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
	int AddLink(T* pData, CLinkItem<T,isArray>* pInsertBefore = NULL);
	int Count();
	int DeleteLink(CLinkItem<T,isArray>* pLink);
	CLinkItem<T,isArray>* FindLink(T* pData);
	CLinkItem<T,isArray>* FirstLink();
	void Init();
};
