#include <stdio.h>			// vsnprintf, etc
#include <list>
#include <vector>

#include "extdll.h"
#include "amutil.h"

#include "commandhandler.h"
#include "XMLMarkup.h"

#define MAX_MESSAGETYPE 9
#define MAX_MESSAGEID 255
#define MAX_EVENTARGUMENTS 10

class CMessage;

typedef enum tagMessageElementType {
	meString,
	meInteger,
	meFloat
} MessageElementType;


typedef enum tagEventAction {
	eaIgnore,
	eaChange,
	eaClear,
	eaStop
} EventAction;

////////////////////////////////////////////////////////////////////////
// Class heirachy: (with some nice multiple inheritence)
//
// CMessageData = Data received from metamod about one message element
//
// CEventArgument = An argument to an AMX event
//
// CEventModifier = An xml tag that can change the "current" event
//
// CMessageProcessorBase = An xml tag that interprets messages
//
// CMessageProcessor = An xml tag that interprets messages and passes them to child tags
//    -> CMessageProcessorBase
//
// CMessageVariable = A storage location for temporary data
//    -> CEventArgument
//
// CMessageCopy = An xml tag to move data between message,variables and arguments
//    -> CEventArgument
//    -> CMessageProcessorBase
//
// CMessageOption = An xml tag marking a possible choice to match against
//    -> CMessageProcessor
//         -> CMessageProcessorBase
//    -> CEventModifier
//
// CMessageElement = An xml tag representing one element of message written by the game mod
//    -> CMessageProcessor
//         -> CMessageProcessorBase
//    -> CEventModifier
//
// CMessageSwitch  = An xml tag representing a decision point (like a switch statement in C++)
//	  -> CMessageProcessor
//         -> CMessageProcessorBase
//
// CMessage = An xml tag representing one type of game mod message to match against
//	  -> CEventModifier
//
// CAMXEventHandler = A registration of an event by a plugin
//    -> CAMXCommandHandler (from commandhandler.h)
//
// CEvent = A defined event that is fired to plugins
//
// CEventDispatcher = Global object to manage configuraiton, trapping and dispatch of events

////////////////////////////////////////////////////////////////////////
// CAMXEventHandler
//
// A command handler function for events

class CAMXEventHandler : public CAMXCommandHandler {
public:
	CAMXEventHandler(CPlugin* pPlugin, int iIndex) : CAMXCommandHandler(pPlugin,iIndex,0) {}
	plugin_result Dispatch(edict_t* pEntity, char* szCmd, char* szData) const;
};

////////////////////////////////////////////////////////////////////////
// CEvent
//
// An event that can be fired to plugins

class CEventArgumentTemplate {
public:
	string m_strName;
	MessageElementType m_Type;

	bool Configure(CXMLNode* pNode);
};

typedef vector<CEventArgumentTemplate*> EventArgumentTemplateVector;

class CEvent {
private:
	CCommandList m_Registrations;
	EventArgumentTemplateVector m_Arguments;
	string m_strName;	
public:
	~CEvent();
	const string& Name() const { return m_strName; }
	void Name(const char* szName) { m_strName=szName; }
	bool Configure(CXMLNode* pEventNode);
	bool Register(CCommandHandler* pHandler);
	void Dispatch() const;
	int LookupArgument(const char* szName) const;
};

typedef map<string,CEvent*> CEventTree;

////////////////////////////////////////////////////////////////////////
// CMessageData
//
// Holds the actual data presented to us by metamod for a message
// element that we need to look at.

class CMessageData {
public:
	MessageElementType m_Type;
	int m_nValue;
	float m_fValue;
	const char* m_szValue;
};

////////////////////////////////////////////////////////////////////////
// CEventArgument
//
// Represents an argument to an amx event callback. Is used in
// g_EventDispatcher.m_CurrentArguments and represents the arguments about to be fired
// at the next messsge end.
//
// It is also used in g_EventDispatch.m_Variable to hold memorised argument values


class CEventArgument {
public:
	MessageElementType m_Type;
	int m_nValue;
	float m_fValue;
	string m_strValue;

	CEventArgument() { Clear(); }

	void Clear() {
		m_Type=meInteger;
		m_nValue=0;
	}

	//Copies the vale from another event argument, coercing to the this argument's datatype
	void CopyValue(const CEventArgument* pInput);
	void LookupPlayer(const float X, const float Y, const float Z);

	//Returns the argument value as an AMX cell.
	cell AMXArgument() const;
};

typedef class vector<CEventArgument*> EventArgumentVector;


////////////////////////////////////////////////////////////////////////
// CMessageVariable
//
// Represents a variable declared in the XML file
//

class CMessageVariable : public CEventArgument
{
	friend class CEventDispatcher;
	friend class CMessage;
	string m_strName;
public:
	bool Configure(CXMLNode* pNode);
};

typedef vector<CMessageVariable*> MessageVariableVector;

////////////////////////////////////////////////////////////////////////
// CMessageProcessorBase
//
// A base class for each item in the event processing tree that can
// read event data and react to it

class CMessageProcessorBase {
public:
	virtual void EventData(CMessageData* pData) const =0;
};

////////////////////////////////////////////////////////////////////////
// CEventModifier
//
// A base class for each item in the event processing tree that can
// modify the current event - i.e. has an event attribute

class CEventModifier {
protected:
	CEvent* m_Event;
	EventAction m_EventAction;

public:
	CEventModifier() { m_EventAction=eaIgnore; }
	CEvent* Event() const { return m_Event; }
	bool ConfigureEvent(CXMLNode* pNode);
	bool ChangeCurrentEvent() const;
};



typedef class vector<CMessageProcessorBase*> MessageProcessorVector;

//A derive class for any message processor that has a collection
//of child message processors
class CMessageProcessor : public CMessageProcessorBase {
protected:
	MessageProcessorVector m_Children;
public:
	~CMessageProcessor();
	bool RegistrationOccurred(CEvent* pEvent) const;
};

////////////////////////////////////////////////////////////////////////
// CMessageCopy
//
// Represents an configuration in a message/messageelement/messageoption to
// record a value and generate an argument
//
// - in CMessage.m_Arguments it representes the arguments that are to be set when
//   the message begins.
//
// - in CMessageElement.m_Arguments it representes the arguments that are to be set when
//   the message element is encountered.
//
// - in CMessageOption.m_Arguments it representes the arguments that are to be set when
//   the message element is encountered and has this option value
//
// A message argument can one of 3 things
//
// - It can write the value matched into an event argument in a specific position
// - It can write a fixed value into an event argument in a specific position
// - It can write the value matched into a "Variable" slot
// - It can write a fixed value into a "Variable" slot
// - It can read a value from a Variable slot into an event argument in a specific position
// - It can read a value from a Variable slot into another Variable slot

//Inherits from CMesssageProcessorBase rather than CMessageProcessor as an argument tag
//cannot have children.
class CMessageCopy: public CEventArgument, public CMessageProcessorBase {
private:
	bool m_fHasValue;
	int m_nReadArgument;
	int m_nWriteArgument;
	const CEventArgument* m_pReadVariable;
	const CEventArgument* m_pReadVariableY;
	const CEventArgument* m_pReadVariableZ;

	CEventArgument* m_pWriteVariable;
public:
	CMessageCopy() : CEventArgument() {
		m_Type=meInteger;
		m_nValue=0;
		m_fHasValue=false;
		m_nReadArgument=-1;
		m_nWriteArgument=-1;
		m_pReadVariable=NULL;
		m_pWriteVariable=NULL;
	}
	bool Configure(CXMLNode* pArgumentNode,CMessage* pMessage,CEvent* pEvent);
	void EventData(CMessageData* pData) const;
	CEventArgument* GetDestination() const;
};

////////////////////////////////////////////////////////////////////////
// CMessageOption
//
// Represents one of the possible values to match against
// the contents of a message element
//
class CMessageOption : public CMessageProcessor, public CEventModifier {
private:
	int m_nValue;
	float m_fValue;
	string m_strValue;
public:
	CMessageOption() : CEventModifier()  {}
	bool Configure(CXMLNode* pElementNode,CMessage* pMessage, CEventModifier* pParent,MessageElementType Type);
	void EventData(CMessageData* pData) const;
	void EventMatch(CMessageData* pMatch,CMessageData* pData) const;
	bool operator==(const CMessageData& data) const;
};

////////////////////////////////////////////////////////////////////////
// CMessageSwitch
//
// Represents a decision point base on an existing argument or a variable
//
class CMessageSwitch : public CMessageProcessor {
private:
	int m_nReadArgument;
	const CEventArgument* m_pReadVariable;
	MessageElementType m_Type;
public:

	bool Configure(CXMLNode* pElementNode,CMessage* pMessage);
	void EventData(CMessageData* pData) const;
};

////////////////////////////////////////////////////////////////////////
// CMessageElement
//
// Represents one string/integer/float in a message to be intercepted
//

class CMessageElement : public CMessageProcessor, public CEventModifier {
private:
	MessageElementType m_Type;
public:
	CMessageElement() : CEventModifier() {}
	bool Configure(CXMLNode* pElementNode,CMessage* pMessage);
	void EventData(CMessageData* pData) const;
};

////////////////////////////////////////////////////////////////////////
// CMessage
//
// Represents one type of message that is to be intercepted
//

class CMessage : public CEventModifier {
private:
	MessageProcessorVector m_Elements; //Element of this message to be matched
	MessageProcessorVector m_Children;
	MessageVariableVector m_Variables;
	bool m_fSubscribed;		//True is something is subscribed to this event
	bool m_fMemorises;		//True if this event writes to event Variable
	bool m_fLog;			//True if this message should be logged when debugging
public:
	CMessage() : CEventModifier() {		
		m_fSubscribed=false;
		m_fMemorises=false;
	}

	~CMessage();

	bool Configure(CXMLNode* pMessageNode);
	bool ConfigureVariables(CXMLNode* pMessageNode);
	CEventArgument* LookupVariable(const string& strName);

	bool Log() const { return m_fLog; }
	bool Active() const { return m_fSubscribed || m_fMemorises; }
	void SetMemorises() { m_fMemorises=true; }

	void InitEventData(int nData) const {
		CMessageData data;
		data.m_nValue = nData;
		data.m_Type = meInteger;

		MessageProcessorVector::const_iterator itArg;
		for(itArg= m_Children.begin();itArg!=m_Children.end();itArg++) {
			(*itArg)->EventData(&data);
		}
	}

	void EventData(CMessageData* pData, int nElement) const {
		if (nElement<m_Elements.size()) {
			m_Elements[nElement]->EventData(pData);
		}
	}

	void RegistrationOccurred(CEvent* pEvent);
};

typedef list<CMessage*> MessageList;

////////////////////////////////////////////////////////////////////////
// CEventDisptcher
//
// A container for all the messages to be intercepted and all the event
// handle commands registerd by plugins to receive the events generated
// from processing a message
//

class CEventDispatcher {
	friend class CMessageOption;
	friend class CMessage;
	friend class CMessageCopy;
	friend class CMessageSwitch;
	friend class CEventModifier;

public:

	CEventDispatcher() {
		WipeMap();
	}

	~CEventDispatcher() {
		m_Variables.clear();
	}



	//////////////////////////////////////////////////////////////////////////
	//Configuration-time functions
	//
public:

	//Reads the configuration file and populates the data structures
	//in this object
	void Configure();

private:
	//Reads any <variable> tags and initialises the variables vector
	bool ConfigureVariables(CXMLNode* pEventMap);

	//Reads one <message> element from the configuration file
	bool ConfigureMessage(CXMLNode* pMessageNode);

	//Find a variable with a specific name
	CEventArgument* LookupVariable(const string& strName);

	//Wipes the stored configuration
	void Clear();

	//After processing a message and building a CMessage object, this
	//adds it to the data structures
	bool AddMessage(CMessage* pMessage,int nMessageID, int nMessageType=-1);

	//Searches for / creates a CommandList for an event name found in
	//the configuration file
	CEvent* LookupEvent(string &strName);

	//////////////////////////////////////////////////////////////////////////
	//Plugin registration of events
	//
public:
	bool Register(const char* szName, CCommandHandler* pHandler);

	//////////////////////////////////////////////////////////////////////////
	//Event firing-time functions
	//
public:

	void EventStart(int nMessageID, int nMessageType, edict_t* pOrigin);
	void EventData(CMessageData* pData);
	void EventEnd();

	//////////////////////////////////////////////////////////////////////////
	//Plugin reading event arguments for current event fired
	//
	int EventArgumentCount() const { return m_nCurrentArgumentCount; }
	cell EventArgument(int i) const { return m_CurrentArguments[i].AMXArgument(); }
private:

	//Configuration members
	MessageList m_List; //A list of all messages in the configuration file - kept soley for Variable management purposes
	CMessage* m_Map[MAX_MESSAGETYPE+1][MAX_MESSAGEID+1]; //A map of the registered message handlers
	CEventTree m_Events;	//A tree of all registered event handlers

	//Current event matching members
	CMessage* m_pCurrentMessage;
	int m_nCurrentElement;
	CEvent* m_pCurrentEvent;
	edict_t* m_pCurrentOrigin;
	int m_nCurrentArgumentCount;
	int m_nCurrentMessageID;
	CEventArgument m_CurrentArguments[MAX_EVENTARGUMENTS];
	MessageVariableVector m_Variables;
	bool m_fLogMessages;
	bool m_fLogCurrent;

	//Called by friend class CMessageOption when the option wants to override
	//the default event for this type of message with a specific one
	void ChangeCurrentEvent(CEvent* pEvent) { m_pCurrentEvent = pEvent; }


	void WipeMap();

};

extern CEventDispatcher g_EventDispatcher;
