/*
 * ===========================================================================
 *
 * $Id: events.cpp,v 1.11 2003/11/08 10:21:23 darope Exp $
 *
 *
 * Copyright (c) 1999-2003 Alfred Reynolds, Florian Zschocke, Magua
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
 * This file contains the implementations for CMessageOption, CMessageElement,
 * CMessage and CEventDispatcher. Rather than going class by class it first
 * contains all the method for reading the XML configuraiton, then methods
 * for plugin's registering events, then methods for the engine firing events
 *
 */

#include <limits.h>
#include "events.h"
#define SDK_UTIL_H
#include "meta_api.h"
#include "CPlugin.h"


#ifndef _CVTBUFSIZE
#  define _CVTBUFSIZE 255
#endif


CEventDispatcher g_EventDispatcher;

bool ParseError(CXMLNode* pNode, const char* szError) {
	string strXML;
	pNode->Value(strXML);
	UTIL_LogPrintf("[ADMIN] ERROR: %s - %s\n",szError,strXML.c_str());
	return false;
}


//////////////////////////////////////////////////////////////////////////////////
// XML Configuration methods

bool GetType(CXMLNode* pNode,MessageElementType* pType) {
	string strType;
	if (!pNode->GetAttribute("type",strType)) {
		return ParseError(pNode,"tag in event map has no type");
	}
	if (strType == "string") {
		*pType = meString;
	}
	else if (strType == "integer") {
		*pType = meInteger;
	}
	else if (strType == "float") {
		*pType = meFloat;
	}
	else {
		return ParseError(pNode,"tag in event map has unknown type");
		return false;
	}
	return true;
}

void CEventDispatcher::Configure() {

	Clear();
	CXMLParser xml;
	CXMLStream* pStream = xml.CreateStream();
	pStream->Options(poTrimWhitespace); //Have the parser trim all text and remove whitespace between tags
	pStream->Filter(ntText+ntTag+ntAttribute+ntCData); //Have the parser filter out processing instructions and comments

	//Read eventmap file path from cvar
	char strFilename[PATH_MAX];
	char strPathname[PATH_MAX];	
	strcpy(strFilename,(char*)CVAR_GET_STRING( "eventmap_file" ));
	if (strcmp(strFilename,"0")==0) {
		UTIL_LogPrintf("[ADMIN] WARNING: The event-handling system has been disabled by setting eventmaps_file to 0. Any scripts that use events may fail\n");
		return;
	}
	if (strlen(strFilename)==0) {
		//If cvar is not set, use the default
		strcpy(strFilename,"addons/adminmod/eventmaps/");
		strcat(strFilename,GetModDir());
		strcat(strFilename,".xml");
	}

	UTIL_LogPrintf("[ADMIN] Starting event-handling system\n");

	if (get_file_path(strPathname, strFilename, PATH_MAX,NULL) ==0) {
		UTIL_LogPrintf("[ADMIN] ERROR: Could not find the event map file %s\n",strFilename);
		return;
	}



	try {
		pStream->Load(strPathname);
		CXMLNodes* pDocument = xml.CreateNodes(CXMLNodeSpec(pStream));
		CXMLNode* pEventMap= pDocument->FindNode(ntTag,"eventmap",false);
		if (!pEventMap) {
			UTIL_LogPrintf("[ADMIN] ERROR: Could not find <eventmap> tag in %s\n",strPathname);
			return;
		}
		string strEventMapGame;
		if (pEventMap->GetAttribute("game",strEventMapGame)) {
			//Event map specifies a particular mod
			if ( strcmp(GetModDir(), strEventMapGame.c_str()) != 0 ) {
				UTIL_LogPrintf("[ADMIN] ERROR: Event map file %s is for game %s, not %s\n",strPathname,strEventMapGame.c_str(),GetModDir());
				return;
			}
		}

		m_fLogMessages = false;
		string strLog;
		if (pEventMap->GetAttribute("log",strLog)) {
			if (strLog=="true") m_fLogMessages = true;
		}

	  	if (!ConfigureVariables(pEventMap)) {
		  	return;
	  	}

		unsigned long i;
		for(i=0;i<pEventMap->Children()->Count();i++) {
			CXMLNode* pMessage = pEventMap->Children()->Item(i);
			if (pMessage->Type() == ntTag) {
				if (pMessage->NodeName()=="message") {
					ConfigureMessage(pMessage);
				}
			}
		}

		UTIL_LogPrintf("[ADMIN] Event-handling system has been configured sucessfully\n");
	}
	catch(errOpenFile&) {
		UTIL_LogPrintf("[ADMIN] ERROR: Could not open event map file %s\n",strPathname);
	}
	catch(errEndTagMismatch&) {
		UTIL_LogPrintf("[ADMIN] ERROR: End tag name did not match opening tag in %s\n",strPathname);
	}
	catch(errInvalidTagName&) {
		UTIL_LogPrintf("[ADMIN] ERROR: Invalid tag name in %s\n",strPathname);
	}
	catch(errInvalidAttributeSyntax&) {
		UTIL_LogPrintf("[ADMIN] ERROR: Invalid attribute syntax in %s\n",strPathname);
	}
	catch(errUnclosedQuote&) {
		UTIL_LogPrintf("[ADMIN] ERROR: Unclosed quote in %s\n",strPathname);
	}
	catch(errStreamInput&) {
		UTIL_LogPrintf("[ADMIN] ERROR: Disk error whilst reading from file %s\n",strPathname);
	}
	catch(errUnknown&) {
		UTIL_LogPrintf("[ADMIN] ERROR: Unexpected error whilst processing file %s\n",strPathname);
	}
	catch(...) {
		UTIL_LogPrintf("[ADMIN] ERROR: Unexpected fatal error whilst processing file %s\n",strPathname);
	}
}

bool CEventDispatcher::ConfigureVariables(CXMLNode* pEventMap) {
	for(int i=0;i<pEventMap->Children()->Count();i++) {
		CXMLNode* pChildNode = pEventMap->Children()->Item(i);
		if (pChildNode->Type() == ntTag) {
			if (pChildNode->NodeName()=="variable") {
				string strName;
				if (pChildNode->GetAttribute("name",strName)) {
					if (LookupVariable(strName)!=NULL) {
						return ParseError(pChildNode,"variable tag in event map has duplicate name");
					}
					CMessageVariable* pVariable = new CMessageVariable();
					pVariable->m_strName = strName;
					if (!pVariable->Configure(pChildNode)) return false;
					m_Variables.push_back(pVariable);
				}
				else {
	                return ParseError(pChildNode,"variable tag in event map has no name attribute");
				}
			}
			if (pChildNode->NodeName()=="event") {
				string strName;
				if (pChildNode->GetAttribute("name",strName)) {
					if (LookupEvent(strName)!=NULL) {
						return ParseError(pChildNode,"event tag in event map has duplicate name");
					}
					CEvent* pEvent = new CEvent();
					pEvent->Name(strName.c_str());
					if (!pEvent->Configure(pChildNode)) return false;
					m_Events[strName]=pEvent;
				}
				else {
	                return ParseError(pChildNode,"event tag in event map has no name attribute");
				}
			}
		}
	}
	return true;
}

bool CMessageVariable::Configure(CXMLNode* pNode) {
	if (!GetType(pNode,&m_Type)) return false;

	switch(m_Type) {
		case meString:
			pNode->GetAttribute("value",m_strValue);
			break;
		case meInteger:
			pNode->GetAttribute("value",m_nValue);
			break;
		case meFloat:
			pNode->GetAttribute("value",m_fValue);
	}
	return true;
}

CEventArgument* CEventDispatcher::LookupVariable(const string& strName) {
	int nPos=0;
	MessageVariableVector::const_iterator it;
	for(it=m_Variables.begin();it!=m_Variables.end();it++) {
		if (strName == (*it)->m_strName) return (*it);
	}
	return NULL;
}


bool CEventDispatcher::ConfigureMessage(CXMLNode* pMessageNode) {
	string strName;
	int   lMessageID;
	int   lMessageType;
	bool  fHasType;

	if (!pMessageNode->GetAttribute("id",lMessageID)) {
		if (pMessageNode->GetAttribute("name",strName)) {
			lMessageID= GET_USER_MSG_ID(PLID, strName.c_str(), 0);
			if(lMessageID<=0) {
                return ParseError(pMessageNode,"message in event map has unrecognised name");
			}
		}
		else {
			return ParseError(pMessageNode,"message in event map has no id or name specified");
		}
	}

	fHasType=pMessageNode->GetAttribute("type",lMessageType);

	if ((lMessageID < 0) || (lMessageID > MAX_MESSAGEID)) {
		return ParseError(pMessageNode,"message in event map has illegal");		
	}
	if (fHasType && ((lMessageType < 0) || (lMessageType > MAX_MESSAGETYPE))) {
		return ParseError(pMessageNode,"message in event map has illegal type");
	}

	CMessage* pMessage = new CMessage();
	if (pMessage->Configure(pMessageNode)) {

		if (fHasType) {
			if (!AddMessage(pMessage,lMessageID,lMessageType)) {
				delete pMessage;
				return ParseError(pMessageNode,"duplicate message in event map");				
			}
		}
		else {
			if (!AddMessage(pMessage,lMessageID)) {
				delete pMessage;
				return ParseError(pMessageNode,"duplicate message in event map");				
			}
		}
		m_List.push_back(pMessage);
		return true;
	}
	else {
		return false;
	}
}

bool CMessage::Configure(CXMLNode* pMessageNode) {

	if (!ConfigureEvent(pMessageNode)) return false;
	if (!ConfigureVariables(pMessageNode)) return false;

	
	string strLog;
	m_fLog=true;
	if (pMessageNode->GetAttribute("log",strLog)) {
		m_fLog=(strLog=="true");
	}

	for (int i=0;i<pMessageNode->Children()->Count();i++) {
		CXMLNode* pChildNode=pMessageNode->Children()->Item(i);
		if (pChildNode->Type() == ntTag) {
			if (pChildNode->NodeName()=="element") {
				CMessageElement* pElement = new CMessageElement();
				if (!pElement->Configure(pChildNode,this)) {
					return false;
				}
				m_Elements.push_back(pElement);
			}
			else if (pChildNode->NodeName() == "copy") {
				CMessageCopy* pArgument = new CMessageCopy();
				if (!pArgument->Configure(pChildNode,this,m_Event)) {
					return false;
				}
				m_Children.push_back(pArgument);
			}
			else if (pChildNode->NodeName() == "switch") {
				CMessageSwitch* pSwitch = new CMessageSwitch();
				if (!pSwitch->Configure(pChildNode,this)) {
					return false;
				}
				m_Children.push_back(pSwitch);
			}
		}
	}

	return true;
}

bool CMessage::ConfigureVariables(CXMLNode* pMessage) {
	for(int i=0;i<pMessage->Children()->Count();i++) {
		CXMLNode* pChildNode = pMessage->Children()->Item(i);
		if (pChildNode->Type() == ntTag) {
			if (pChildNode->NodeName()=="variable") {
				string strName;
				if (pChildNode->GetAttribute("name",strName)) {
					if (LookupVariable(strName)!=NULL) {
						return ParseError(pChildNode,"variable tag in message has duplicate name");
					}
					CMessageVariable* pVariable = new CMessageVariable();
					pVariable->m_strName = strName;
					if (!pVariable->Configure(pChildNode)) return false;
					m_Variables.push_back(pVariable);
				}
				else {
	                return ParseError(pChildNode,"variable tag in message has no name attribute");
				}
			}
		}
	}
	return true;
}

CEventArgument* CMessage::LookupVariable(const string& strName) {
	int nPos=0;
	MessageVariableVector::const_iterator it;
	for(it=m_Variables.begin();it!=m_Variables.end();it++) {
		if (strName == (*it)->m_strName) return (*it);
	}
	return NULL;
}

bool CMessageElement::Configure(CXMLNode* pElementNode,CMessage* pMessage) {
	if (!GetType(pElementNode,&m_Type)) return false;

	if (!ConfigureEvent(pElementNode)) return false;

	CEvent* pEvent = m_Event;
	if (pEvent==NULL) pEvent=pMessage->Event();

	for (int i=0;i<pElementNode->Children()->Count();i++) {
		CXMLNode* pChildNode=pElementNode->Children()->Item(i);
		if (pChildNode->Type() == ntTag) {
			if (pChildNode->NodeName() == "option") {
				CMessageOption* pOption = new CMessageOption();
				if (!pOption->Configure(pChildNode,pMessage,this,m_Type)) {
					return false;
				}
				m_Children.push_back(pOption);
			}
			else if (pChildNode->NodeName() == "copy") {
				CMessageCopy* pArgument = new CMessageCopy();
				if (!pArgument->Configure(pChildNode,pMessage,pEvent)) {
					return false;
				}
				m_Children.push_back(pArgument);
			}
			else if (pChildNode->NodeName() == "switch") {
				CMessageSwitch* pSwitch = new CMessageSwitch();
				if (!pSwitch->Configure(pChildNode,pMessage)) {
					return false;
				}
				m_Children.push_back(pSwitch);
			}
		}
	}


	return true;
}

bool CMessageSwitch::Configure(CXMLNode* pSwitchNode,CMessage* pMessage) {
	if (!GetType(pSwitchNode,&m_Type)) return false;

	int nSourceCount=0;
	int nPos;
	bool fReadArgument=pSwitchNode->GetAttribute("readargument",nPos);
	if (fReadArgument) {
		nSourceCount++;
		m_nReadArgument = nPos;
	}

	string strVariable;
	bool fReadVariable=pSwitchNode->GetAttribute("readvariable",strVariable);
	if (fReadVariable) {
		nSourceCount++;
		m_pReadVariable = pMessage->LookupVariable(strVariable);
		if (m_pReadVariable==NULL) {
			m_pReadVariable = g_EventDispatcher.LookupVariable(strVariable);
		}
		if (m_pReadVariable==NULL) {
			return ParseError(pSwitchNode,"switch tag in event map has unknown variable name in readvariable attribute");
		}
	}

	if (nSourceCount!=1) {
		return ParseError(pSwitchNode,"switch tag in event map must have either a readargument or a readvariable attribute");
	}

	for (int i=0;i<pSwitchNode->Children()->Count();i++) {
		CXMLNode* pChildNode=pSwitchNode->Children()->Item(i);
		if (pChildNode->Type() == ntTag) {
			if (pChildNode->NodeName() == "option") {
				CMessageOption* pOption = new CMessageOption();
				if (!pOption->Configure(pChildNode,pMessage,NULL,m_Type)) {
					return false;
				}
				m_Children.push_back(pOption);
			}
		}
	}
	return true;
}


bool CMessageOption::Configure(CXMLNode* pOptionNode,CMessage* pMessage, CEventModifier* pParent,MessageElementType m_Type) {

	bool fHasValue;
	switch(m_Type) {
		case meString:
			fHasValue=pOptionNode->GetAttribute("value",m_strValue);
			break;
		case meInteger:
			fHasValue=pOptionNode->GetAttribute("value",m_nValue);
			break;
		case meFloat:
			fHasValue=pOptionNode->GetAttribute("value",m_fValue);
	}

	if (!ConfigureEvent(pOptionNode)) return false;
	CEvent* pEvent=m_Event;
	if (pParent) {
		if (pEvent == NULL) pEvent=pParent->Event();
	}
	if (pEvent == NULL) pEvent=pMessage->Event();

	for (int i=0;i<pOptionNode->Children()->Count();i++) {
		CXMLNode* pChildNode=pOptionNode->Children()->Item(i);
		if (pChildNode->Type() == ntTag) {
			if (pChildNode->NodeName() == "copy") {
				CMessageCopy* pArgument = new CMessageCopy();
				if (!pArgument->Configure(pChildNode,pMessage,pEvent)) {
					return false;
				}
				m_Children.push_back(pArgument);
			}
			else if (pChildNode->NodeName() == "switch") {
				CMessageSwitch* pSwitch = new CMessageSwitch();
				if (!pSwitch->Configure(pChildNode,pMessage)) {
					return false;
				}
				m_Children.push_back(pSwitch);
			}

		}
	}


	if (!fHasValue) {
		return ParseError(pOptionNode,"option in event map has no value attribute specified");
	}
	return fHasValue;
}

bool CMessageCopy::Configure(CXMLNode* pCopyNode,CMessage* pMessage,CEvent* pEvent) {
	if (!GetType(pCopyNode,&m_Type)) return false;
	switch(m_Type) {
		case meString:
			m_fHasValue=pCopyNode->GetAttribute("value",m_strValue);
			break;
		case meInteger:
			m_fHasValue=pCopyNode->GetAttribute("value",m_nValue);
			break;
		case meFloat:
			m_fHasValue=pCopyNode->GetAttribute("value",m_fValue);
	}

	string strArgument;
	int nPos=-1;
	bool fLookup=false;
	//If an event is supplied, check for writeargument containing a string
	//that is the name of an event argument
	if (pCopyNode->GetAttribute("writeargument",strArgument)) {
		if (pEvent) {
			nPos = pEvent->LookupArgument(strArgument.c_str());
		}
	}
	
	//If that didn't work try treating it as a number
	bool fWriteArgument=(nPos>-1);
	fLookup=(nPos>-1);
	if (!fWriteArgument) {
		fWriteArgument = pCopyNode->GetAttribute("writeargument",nPos);
	}
	if (fWriteArgument) {
		if ((nPos==0) && (strArgument != "0") && !fLookup) {
			return ParseError(pCopyNode,"writeargument attribute contains unresolved argument name");
		}
		m_nWriteArgument=nPos;
	}

	string strVariable;
	bool fWriteGlobalVariable=false;
	bool fWriteVariable=pCopyNode->GetAttribute("writevariable",strVariable);
	if (fWriteVariable) {
		m_pWriteVariable=pMessage->LookupVariable(strVariable);
		if (m_pWriteVariable==NULL) {
			m_pWriteVariable=g_EventDispatcher.LookupVariable(strVariable);
			if (m_pWriteVariable!=NULL) {
				fWriteGlobalVariable=true;
			}
		}
		if (m_pWriteVariable==NULL) {
			return ParseError(pCopyNode,"copy tag in event map has unknown variable name in writevariable attribute");			
		}
	}


	if (!fWriteArgument && !fWriteVariable) {
		return ParseError(pCopyNode,"copy tag in event map has no writeargument or writevariable attribute");
	}

	int nSourceCount=0;
	if (m_fHasValue) nSourceCount=1;

	//If an event is supplied, check for readargument containing a string
	//that is the name of an event argument
	nPos=-1;
	fLookup=false;
	if (pCopyNode->GetAttribute("readargument",strArgument)) {
		if (pEvent) {		
			nPos = pEvent->LookupArgument(strArgument.c_str());
		}
	}
	
	//If that didn't work try treating it as a number
	bool fReadArgument=(nPos>-1);
	fLookup=(nPos>-1);
	if(!fReadArgument) {
		fReadArgument=pCopyNode->GetAttribute("readargument",nPos);
	}
	if (fReadArgument) {
		if ((nPos==0) && (strArgument != "0") && !fLookup) {
			return ParseError(pCopyNode,"readargument attribute contains unresolved argument name");
		}
		nSourceCount++;
		m_nReadArgument = nPos;
	}

	bool fReadVariable=pCopyNode->GetAttribute("readVariable",strVariable);
	if (fReadVariable) {
		nSourceCount++;
		m_pReadVariable = pMessage->LookupVariable(strVariable);
		if (m_pReadVariable==NULL) {
			m_pReadVariable = g_EventDispatcher.LookupVariable(strVariable);
		}
		if (m_pReadVariable==NULL) {
			return ParseError(pCopyNode,"copy tag in event map has unknown variable name in readvariable attribute");
		}
	}
	string strLocation;
	bool fReadLocation=pCopyNode->GetAttribute("readlocation",strLocation);
	if (fReadLocation) {
		nSourceCount++;
		strVariable=strLocation;
		strVariable.append("X");
		m_pReadVariable = pMessage->LookupVariable(strVariable);
		if (m_pReadVariable==NULL) {
			m_pReadVariable = g_EventDispatcher.LookupVariable(strVariable);
		}
		if (m_pReadVariable==NULL) {
			return ParseError(pCopyNode,"copy tag in event map has unknown X variable name in readlocation attribute");
		}
		strVariable=strLocation;
		strVariable.append("Y");
		m_pReadVariableY = pMessage->LookupVariable(strVariable);
		if (m_pReadVariableY==NULL) {
			m_pReadVariableY = g_EventDispatcher.LookupVariable(strVariable);
		}
		if (m_pReadVariableY==NULL) {
			return ParseError(pCopyNode,"copy tag in event map has unknown Y variable name in readlocation attribute");
		}
		strVariable=strLocation;
		strVariable.append("Z");
		m_pReadVariableZ = pMessage->LookupVariable(strVariable);
		if (m_pReadVariableZ==NULL) {
			m_pReadVariableZ = g_EventDispatcher.LookupVariable(strVariable);
		}
		if (m_pReadVariableZ==NULL) {
			return ParseError(pCopyNode,"copy tag in event map has unknown Z variable name in readlocation attribute");
		}
	}

	if (nSourceCount>1) {
		return ParseError(pCopyNode,"copy tag in event map has more than one of the readargument, readvariable and value attributes");
	}
	if (m_nWriteArgument > MAX_EVENTARGUMENTS) {
		return ParseError(pCopyNode,"copy tag in event map has illegal writeargument");		
	}
	if (m_nReadArgument > MAX_EVENTARGUMENTS) {
		return ParseError(pCopyNode,"copy tag in event map has illegal readargument");
	}

	if (fWriteGlobalVariable) {
		pMessage->SetMemorises();
	}
	return true;
}

bool CEventDispatcher::AddMessage(CMessage* pMessage,int nMessageID, int nMessageType) {
	if (nMessageType==-1) {
		//If no message type was specified, register for all message types
		for (int i=0;i<=MAX_MESSAGETYPE;i++) {
			if (m_Map[i][nMessageID]==0) {
				m_Map[i][nMessageID] = pMessage;
			}
			else {
				return false;
			}
		}
		return true;
	}
	else {
		if (m_Map[nMessageType][nMessageID]==0) {
			m_Map[nMessageType][nMessageID] = pMessage;
			return true;
		}
		else {
			return false;
		}
	}
}

bool CEventModifier::ConfigureEvent(CXMLNode* pNode) {
	string strEvent;
	m_Event = 0;
	m_EventAction=eaIgnore;
	if (pNode->GetAttribute("event",strEvent)) {
		if (strEvent == "stop") {
			m_EventAction=eaStop;
		} else if (strEvent == "clear") {
			m_EventAction=eaClear;
		} else {
			m_Event = g_EventDispatcher.LookupEvent(strEvent);
			if (m_Event == NULL) {
				return ParseError(pNode,"event attribute references undefined event");
			}
			m_EventAction = eaChange;
		}
	}
	return true;
}

CEvent* CEventDispatcher::LookupEvent(string &strName) {
	CEventTree::iterator itTree = m_Events.find(strName);
	CEvent* pEvent;
	if (itTree==m_Events.end()) {
		pEvent=NULL;
	}
	else {
		pEvent = itTree->second;
	}
	return pEvent;
}

bool CEvent::Configure(CXMLNode* pEventNode) {
	for (int i=0;i<pEventNode->Children()->Count();i++) {
		CXMLNode* pChildNode=pEventNode->Children()->Item(i);
		if (pChildNode->Type() == ntTag) {
			if (pChildNode->NodeName() == "argument") {
				CEventArgumentTemplate* pArgument = new CEventArgumentTemplate();
				if (!pArgument->Configure(pChildNode)) {
					return false;
				}
				m_Arguments.push_back(pArgument);
			}
		}
	}
	return true;
}

bool CEventArgumentTemplate::Configure(CXMLNode* pEventNode) {
	if (!GetType(pEventNode,&m_Type)) return false;

	if (!pEventNode->GetAttribute("name",m_strName)) {
		return ParseError(pEventNode,"argument tag in event map has no name");
	}

	return true;
}

int CEvent::LookupArgument(const char* szName) const {
	EventArgumentTemplateVector::const_iterator it;
	int nPos=0;
	for(it=m_Arguments.begin();it!=m_Arguments.end();it++) {
		if ((*it)->m_strName == szName) {
			return nPos;
		}
		nPos++;
	}
	return -1;
}

//////////////////////////////////////////////////////////////////////////////////
// Cleanup when changing map / shutting down

void CEventDispatcher::Clear() {
	WipeMap();
	MessageList::iterator it;
	for(it= m_List.begin();it!=m_List.end();it++) {
		delete (*it);
	}
	m_List.clear();

	vector<CMessageVariable*>::iterator it2;
	for(it2= m_Variables.begin();it2!=m_Variables.end();it2++) {
		delete (*it2);
	}
	m_Variables.clear();


	while (m_Events.size() >0) {
		CEvent* pEvent= m_Events.begin()->second;
		m_Events.erase(m_Events.begin());
		delete pEvent;
	}
}

void CEventDispatcher::WipeMap() {
	for (int i=0;i<=MAX_MESSAGETYPE;i++) {
		for (int j=0;j<=MAX_MESSAGEID;j++) {
			m_Map[i][j]=0;
		}
	}
}

CEvent::~CEvent() {
	while (m_Registrations.size() >0) {
		delete m_Registrations.front();
		m_Registrations.pop_front();
	}
}

CMessage::~CMessage() {
	MessageProcessorVector::iterator it;
	for(it= m_Elements.begin();it!=m_Elements.end();it++) {
		delete (*it);
	}
	MessageProcessorVector::iterator itArg;
	for(itArg= m_Children.begin();itArg!=m_Children.end();itArg++) {
		delete (*itArg);
	}
}

CMessageProcessor::~CMessageProcessor() {
	MessageProcessorVector::iterator it;
	for(it= m_Children.begin();it!=m_Children.end();it++) {
		delete (*it);
	}
}

//////////////////////////////////////////////////////////////////////////////////
// Plugin registration of event handlers

bool CEventDispatcher::Register(const char* szName, CCommandHandler* pHandler) {
	std::string strEvent = szName;
	CEventTree::iterator itTree = m_Events.find(szName);
	CEvent* pEvent;

	if (itTree==m_Events.end()) {
		//A plugin cannot register an event unless it is the event map already
		UTIL_LogPrintf("[ADMIN] ERROR: A plugin is attempting to register a handler for non-existant event %s\n",szName);
		return false;
	}

	//Put the event handlers in plugin priority order - i.e. plugins at the
	//top of plugin.ini go first in the list even if they register 2nd
	//
	pEvent = itTree->second;
	if (pEvent->Register(pHandler)) {
		//This is the first regitered event handler
		//tell the messages that can generate this
		//event so they can activate themselves
		MessageList::iterator itMessages;
		for (itMessages=m_List.begin();itMessages!=m_List.end();itMessages++) {
			(*itMessages)->RegistrationOccurred(pEvent);
		}
	}

	return true;
}

bool CEvent::Register(CCommandHandler* pHandler) {
	CCommandList::iterator itList;
	for (itList=m_Registrations.begin(); itList!=m_Registrations.end() ; itList++) {
		if ((*itList)->Priority() > pHandler->Priority()) {
			m_Registrations.insert(itList,pHandler);
			return true;
		}
	}
	m_Registrations.push_back(pHandler);
	return m_Registrations.size() ==1;
}


void CMessage::RegistrationOccurred(CEvent* pEvent) {
	if (pEvent == m_Event) {
		m_fSubscribed=true;
		return;
	}
	MessageProcessorVector::const_iterator it;
	for(it= m_Elements.begin();it!=m_Elements.end();it++) {
		CMessageProcessor* pChild = dynamic_cast<CMessageProcessor*>(*it);
		if (pChild) {
			if (pChild->RegistrationOccurred(pEvent)) {
			m_fSubscribed=true;
			return;
			}
		}
	}

	for(it= m_Children.begin();it!=m_Children.end();it++) {
		CMessageProcessor* pChild = dynamic_cast<CMessageProcessor*>(*it);
		if (pChild) {
			if (pChild->RegistrationOccurred(pEvent)) {
			m_fSubscribed=true;
			return;
			}
		}
	}
}

bool CMessageProcessor::RegistrationOccurred(CEvent* pEvent) const {

	const CEventModifier* pMod = dynamic_cast<const CEventModifier*>(this);
	if (pMod) {
		if (pMod->Event() == pEvent) return true;
	}

	MessageProcessorVector::const_iterator it;
	for(it= m_Children.begin();it!=m_Children.end();it++) {
		const CMessageProcessor* pChild = dynamic_cast<const CMessageProcessor*>(*it);
		if (pChild) {
			//Child is another message processor, not a message processor base
			//so it all needs to check for registrations
			if (pChild->RegistrationOccurred(pEvent)) {
				return true;
			}
		}
	}
	return false;
}





//////////////////////////////////////////////////////////////////////////////////
// Engine firing events

void CEventDispatcher::EventStart(int nMessageID, int nMessageType, edict_t* pOrigin) {

	//Bounds check the values
	if (nMessageID<0) return;
	if (nMessageType<0) return;
	if (nMessageID>MAX_MESSAGEID) return;
	if (nMessageType>MAX_MESSAGETYPE) return;

	//Record m_nCurrentMessageID purely for debug logging
	m_nCurrentMessageID= nMessageID;

	//Clear current status for start of new message
	m_nCurrentElement=0;
	m_nCurrentArgumentCount=0;
	m_pCurrentOrigin = pOrigin;

	//Check if we implement a CMessage object for this message
	m_pCurrentMessage=m_Map[nMessageType][nMessageID];
	m_fLogCurrent=m_fLogMessages;

	if (m_pCurrentMessage) {
		//Check if it supresses logging
		if (!m_pCurrentMessage->Log()) {
			m_fLogCurrent=false;
		}

		//Check if any plugin has subscribed to it
		//if not, there is no point in us wasteing time
		//processing it
		if (!m_pCurrentMessage->Active()) {
			m_pCurrentMessage=NULL;
		}
	}

	if (m_fLogCurrent) {
		UTIL_LogPrintf("[EVENT] %i: Start of Event - Type %i, Entity Index %i\n",nMessageID,nMessageType, ENTINDEX(pOrigin));
	}

	if (m_pCurrentMessage) {
		//There is a message to process so clear out
		//the current event argument structures
        m_pCurrentEvent=m_pCurrentMessage->Event();
		for(int i=0;i<MAX_EVENTARGUMENTS;i++) {
			m_CurrentArguments[i].Clear();
		}
		m_pCurrentMessage->InitEventData(ENTINDEX(pOrigin));

	}

}

void CEventDispatcher::EventData(CMessageData* pData) {
	if (m_nCurrentMessageID!=0) {
		if (m_fLogCurrent) {
			switch(pData->m_Type) {
			case meInteger:
				UTIL_LogPrintf("[EVENT] %i/%i i: %i\n",m_nCurrentMessageID,m_nCurrentElement,pData->m_nValue);
				break;
			case meFloat:
				UTIL_LogPrintf("[EVENT] %i/%i f: %f\n",m_nCurrentMessageID,m_nCurrentElement,pData->m_fValue);
				break;
			case meString:
				UTIL_LogPrintf("[EVENT] %i/%i s: %s\n",m_nCurrentMessageID,m_nCurrentElement,pData->m_szValue);
			}
		}
	}
	if (m_pCurrentMessage) {
		m_pCurrentMessage->EventData(pData,m_nCurrentElement);
	}
	m_nCurrentElement++;
}

bool CEventModifier::ChangeCurrentEvent() const {
	if (m_EventAction==eaChange) {
		g_EventDispatcher.ChangeCurrentEvent(m_Event);
		return true;
	}

	g_EventDispatcher.ChangeCurrentEvent(NULL);
	if (m_EventAction==eaStop) {
		g_EventDispatcher.m_pCurrentMessage=NULL;
		return false;
	}
	else {
		return true;
	}
	
}


void CEventDispatcher::EventEnd()	{
	if (m_pCurrentMessage) {
		//Check if processing the message actually resulted in an event
		if (m_pCurrentEvent) {
			//Dispatch the event to all plugins that have registered a handler
			if (m_fLogCurrent) {
				UTIL_LogPrintf("[EVENT] %i: Dispatching %s\n",m_nCurrentMessageID,m_pCurrentEvent->Name().c_str());
			}
			m_pCurrentEvent->Dispatch();
		}
		//No longer processing a message - the rest of the current status
		//can safely remain "dirty" until the start of the next message - no
		//point in cleaning it twice
		m_pCurrentMessage=NULL;
	}

	if (m_nCurrentMessageID!=0) {
		if (m_fLogCurrent) {
			UTIL_LogPrintf("[EVENT] %i: End of Event\n",m_nCurrentMessageID);
		}
	}
	m_nCurrentMessageID=0;
}

void CEvent::Dispatch() const {
	plugin_result nRet=PLUGIN_CONTINUE;
	CCommandList::const_iterator it;
	for(it=m_Registrations.begin();it!=m_Registrations.end();it++) {
		if ((*it)->Dispatch(NULL,"event","")==PLUGIN_HANDLED) {
			break;
		}
	}
}


void CMessageElement::EventData(CMessageData* pData) const {
	if (pData->m_Type==m_Type) {
		MessageProcessorVector::const_iterator itArg;
		for(itArg=m_Children.begin();itArg!=m_Children.end();itArg++) {
			(*itArg)->EventData(pData);
		}
	}
	else {
		DEBUG_LOG(2,("[ADMIN] ERROR: Actual message contained an element of the wrong data type"));
	}
}

void CMessageSwitch::EventData(CMessageData* pData) const {
	MessageProcessorVector::const_iterator itArg;

	//Setup a new message data to contain the value
	//read or the hardcoded value
	CMessageData data;
	data.m_Type = m_Type;
	if (m_pReadVariable) {
		switch (m_Type) {
		case meInteger:
			data.m_nValue = m_pReadVariable->m_nValue;
			break;
		case meFloat:
			data.m_fValue = m_pReadVariable->m_fValue;
			break;
		case meString:
			data.m_szValue = m_pReadVariable->m_strValue.c_str();
			break;
		}
	}
	else {
		switch (m_Type) {
		case meInteger:
			data.m_nValue = g_EventDispatcher.m_Variables[m_nReadArgument]->m_nValue;
			break;
		case meFloat:
			data.m_fValue = g_EventDispatcher.m_Variables[m_nReadArgument]->m_fValue;
			break;
		case meString:
			data.m_szValue = g_EventDispatcher.m_Variables[m_nReadArgument]->m_strValue.c_str();
			break;
		}
	}

	for(itArg= m_Children.begin();itArg!=m_Children.end();itArg++) {
		dynamic_cast<CMessageOption*>(*itArg)->EventMatch(&data,pData);
	}
}

void CMessageOption::EventMatch(CMessageData* pMatch,CMessageData* pData) const {
	if (*this == *pMatch) {
		if (m_EventAction!=eaIgnore) {
			if (!ChangeCurrentEvent()) return;
		}
		MessageProcessorVector::const_iterator itArg;
		for(itArg= m_Children.begin();itArg!=m_Children.end();itArg++) {
			(*itArg)->EventData(pData);
		}
	}
}

void CMessageOption::EventData(CMessageData* pData) const {
	if (*this == *pData) {
		if (m_EventAction!=eaIgnore) {
			if (!ChangeCurrentEvent()) return;
		}
		MessageProcessorVector::const_iterator itArg;
		for(itArg= m_Children.begin();itArg!=m_Children.end();itArg++) {
			(*itArg)->EventData(pData);
		}
	}
}

bool CMessageOption::operator==(const CMessageData& pData) const {
	switch (pData.m_Type) {
	case meInteger:
		return m_nValue==pData.m_nValue;
	case meFloat:
		return abs(pData.m_fValue-m_fValue)<0.0001;
	case meString:
		return m_strValue == pData.m_szValue;
	}
	return false;
}

CEventArgument* CMessageCopy::GetDestination() const {
	CEventArgument* pOutput;

	if (m_pWriteVariable) {
		pOutput= m_pWriteVariable;
	}
	else {
		if (g_EventDispatcher.m_nCurrentArgumentCount<m_nWriteArgument+1) g_EventDispatcher.m_nCurrentArgumentCount=m_nWriteArgument+1;
		pOutput = &(g_EventDispatcher.m_CurrentArguments[m_nWriteArgument]);
	}

	pOutput->m_Type = m_Type;

	//Check if we should be using an alternate source rather
	//than the data from the message being handled
	if (m_fHasValue) {
		pOutput->CopyValue(this);
		return 0;
	}
	else if (m_pReadVariable) {
		if (m_pReadVariableY) {
			pOutput->LookupPlayer(m_pReadVariable->m_fValue,m_pReadVariableY->m_fValue,m_pReadVariableZ->m_fValue);
		}
		else {
			pOutput->CopyValue(m_pReadVariable);
		}		
		return 0;
	}
	else if (m_nReadArgument>=0) {
		pOutput->CopyValue(&(g_EventDispatcher.m_CurrentArguments[m_nReadArgument]));
		return 0;
	}

	//No alternate source found so return the output event argument
	//so the caller can copy the current data into it
	return pOutput;
}

void CMessageCopy::EventData(CMessageData* pData) const {
	CEventArgument* pOutput = GetDestination();
	if (pOutput) {
		switch (pData->m_Type) {
		case meInteger:
			switch (m_Type) {
			case meString:
				char buf[_CVTBUFSIZE];
				pOutput->m_strValue=itoa(pData->m_nValue,buf,10);
				break;
			case meInteger:
				pOutput->m_nValue=pData->m_nValue;
				break;
			case meFloat:
				pOutput->m_fValue=pData->m_nValue;
			}
			break;
		case meFloat:
			switch (m_Type) {
			case meString:
				char buf[_CVTBUFSIZE];
				pOutput->m_strValue=gcvt(pData->m_fValue,_CVTBUFSIZE-2,buf);
				break;
			case meInteger:
				pOutput->m_nValue=pData->m_fValue;
				break;
			case meFloat:
				pOutput->m_fValue=pData->m_fValue;
			}
			break;
		case meString:
			switch (m_Type) {
			case meString:
				pOutput->m_strValue=pData->m_szValue;
				break;
			case meInteger:
				pOutput->m_nValue=atoi(pData->m_szValue);
				break;
			case meFloat:
				pOutput->m_fValue=atof(pData->m_szValue);
				break;
			}
		}
	}
}


void CEventArgument::CopyValue(const CEventArgument* pInput) {
	char buf[_CVTBUFSIZE];

	switch (m_Type) {
	case meString:
		switch (pInput->m_Type) {
		case meString:
			m_strValue=pInput->m_strValue;
			break;
		case meFloat:
			m_strValue=gcvt(pInput->m_fValue,_CVTBUFSIZE-2,buf);;
			break;
		case meInteger:
			m_strValue=itoa(pInput->m_nValue,buf,10);
		}
		break;
	case meFloat:
		switch (pInput->m_Type) {
		case meString:
			m_fValue=atof(pInput->m_strValue.c_str());
			break;
		case meFloat:
			m_fValue=pInput->m_fValue;
			break;
		case meInteger:
			m_fValue=pInput->m_nValue;
		}
		break;
	case meInteger:
		switch (pInput->m_Type) {
		case meString:
			m_nValue=atoi(pInput->m_strValue.c_str());
			break;
		case meFloat:
			m_nValue=pInput->m_fValue;
			break;
		case meInteger:
			m_nValue=pInput->m_nValue;
		}
	}
}

void CEventArgument::LookupPlayer(const float X,const float Y, const float Z) {
	m_nValue=0;
	for(int i=1;i<=gpGlobals->maxClients;i++) {
		edict_t* pEnt=INDEXENT(i);
		if (pEnt) {
			if (abs(pEnt->v.origin.x - X) + abs(pEnt->v.origin.y - Y) + abs(pEnt->v.origin.z - Z) < 20) {
				m_nValue=i;
				return;
			}
		}
	}
}

//Hack function to return the arguments in a form that can be passed to AMX
//which expects a bunch of cells
cell CEventArgument::AMXArgument() const {
	switch(m_Type) {
		case meString:
			return (cell)(m_strValue.c_str());
		case meInteger:
			return (cell)(m_nValue);
		case meFloat:
			return (cell)(m_fValue);
	}
	return (cell)0;
}

plugin_result CAMXEventHandler::Dispatch(edict_t* pEntity, char* szCmd, char* szData) const
{
		return m_pPlugin->HandleEvent(this,pEntity);
}

