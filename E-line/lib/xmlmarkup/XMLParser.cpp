// Parser.cpp : Implementation of CXMLParser
#include "XMLMarkup.h"

//ParseContext class is used by the parser
//to record its current environmen whilst
//it parses a stream.
//
//As the parser runs, it occassionaly detects
//a nested sub-stream (e.g. The attributes of
//a tag) and enters a nested ParseContent whilst
//it processes the sub-stream.

typedef enum tagParseContextType {
	pcUnknown,			            /* Unknown context */
	pcText,							        /* Ordinary text */
	pcTag,							        /* A tag name */
	pcProcessingInstruction,		/* The name of an XML <? ?> tag */
	pcComment,						      /* The body of a <!-- --> comment */
	pcStringSingle,					    /* Inside a ' ' string */
	pcStringDouble,             /* Inside a " " string */
	pcIgnoreSingle,					    /* Inside a ' ' string that will be removed */
	pcIgnoreDouble,             /* Inside a " " string that will be removed */
	pcAttributeName,            /* The name of an attribute */
	pcPostAttributeName,        /* After an attribute name */
	pcPreAttributeValue,        /* After an = in attributes */
	pcAttributeValue,           /* In an attribute value */
	pcCData                     /* The body of a <![CDATA[ ]]> block */
} ParseContextType;

class CParseContext {
private:
	ParseContextType m_Type;
	CParseContext* m_Previous;
	string m_CurrentData;
	bool m_PopData;
public:

	CParseContext()
	{
		m_Type = pcUnknown;
		m_Previous = 0;
		m_PopData = false;
	}

	~CParseContext()
	{
		if (m_Previous) {
			delete m_Previous;
		}
	}

	CParseContext& operator=(ParseContextType type)
	{
		m_Type = type;
		return *this;
	}

	bool operator==(ParseContextType type)
	{
		return m_Type ==type;
	}

	bool operator!=(ParseContextType type)
	{
		return m_Type !=type;
	}

	string& CurrentData()
	{
		return m_CurrentData;
	}

	void CurrentData(string data)
	{
		m_CurrentData = data;
	}

	void TrimCurrentData() {
		long nStart = m_CurrentData.find_first_not_of(" \xA0\t\n\r");
		if (nStart>=0) {
			long nEnd = m_CurrentData.find_last_not_of(" \xA0\t\n\r");
			if (nStart<=nEnd) {
				m_CurrentData = m_CurrentData.substr(nStart,nEnd-nStart+1);
			}
			else {
				//m_CurrentData.clear();
				m_CurrentData.erase();
			}
		}
		else {
			//m_CurrentData.clear();
			m_CurrentData.erase();
		}
	}

	void push(ParseContextType type,bool PushData)
	{
		CParseContext* pOld = new CParseContext;
		pOld->m_Previous = m_Previous;
		pOld->m_Type = m_Type;
		pOld->m_PopData = PushData;
		if (PushData) {
			pOld->m_CurrentData = m_CurrentData;
			m_CurrentData.erase();
		}
		m_Type = type;
		m_Previous = pOld;
	}

	void pop()
	{
		CParseContext* pOld = m_Previous;
		m_Type = pOld->m_Type;
		m_Previous = pOld->m_Previous;
		pOld->m_Previous = 0;
		if (pOld->m_PopData) {
			m_CurrentData = pOld->m_CurrentData;
		}
		delete pOld;
	}

	inline ParseContextType Type()
	{
		return m_Type;
	}

	inline bool Within(ParseContextType t)
	{
		if (m_Type == t) return true;
		if (m_Previous) return m_Previous->Within(t);
		return false;
	}

};

/////////////////////////////////////////////////////////////////////////////
// CXMLParser


//This method simply acts as a public constructor for the NodeStream
//class.  It is used rather than allowing CoCreateInstance to be used
//on the nodestream class as it may become necessary to initialise
//the NodeStream before it is passed the client - although currently,
//this isn't the case.
CXMLStream* CXMLParser::CreateStream()
{
	return new CXMLStream();
}

//This is the main function exposed by this library. It takes a
//variant that contains an XML/HTML string, a Node stream, or an
//existing INode or INodes and creates an INodes collection
//representing that XML/HTML
CXMLNodes* CXMLParser::CreateNodes(CXMLNodeSpec NodeSpec)
{
	return ParseNodeSpec(NodeSpec);
}

//This is the internal version of CreateNodes that allows the
//calls to specify if the input is markup or attributes and how
//tolerant it should be of parse errors.
//

CXMLNodes* CXMLParser::ParseNodeSpec(CXMLNodeSpec NodeSpec,InputType DefaultType)
{
	//NodeSpec is either:
	//
	//An INodes pointer - detatch all nodes and pass it back
	//An INode pointer - create an INodes and append it and detatch it
	//An INodeStream pointer - parse it
	//An integer / boolean / another object type - Convert to a string then continue
	//A string - Create a NodeStream and parse it

	CXMLStream* pStream;

	switch (NodeSpec.Type()) {
	case nstNodeStream:
		return ParseStream(NodeSpec.NodeStream());

	case nstNodes:
		if (DefaultType == itXMLAttributes)
		{
			if (!NodeSpec.Nodes()->AllAttributes()) throw errIllegalNodeType();
		}
		else {
			if (NodeSpec.Nodes()->AnyAttributes()) throw errIllegalNodeType();
		}
		return NodeSpec.Nodes();

	case nstNode:
		{
			NodeType nt=NodeSpec.Node()->Type();
			if (DefaultType == itXMLAttributes) {
				if (nt != ntAttribute) throw errIllegalNodeType();
			}

			CXMLNodes* pNodes = new CXMLNodes;
			pNodes->AppendNode(NodeSpec.Node());
			return pNodes;
		}
	case nstString:
		pStream = new CXMLStream();
		pStream->LoadXML(NodeSpec.String().c_str());
		pStream->Type(DefaultType);
		return ParseStream(pStream);
	}

	//Shouldn't be able to reach here anyway
	return 0;
}


CXMLNodes* CXMLParser::ParseStream(CXMLStream* Stream)
{
	if (Stream->Type() == itXML) {
		return ParseTagStream(Stream);
	}
	else {
		return ParseAttributeStream(Stream);
	}
}
//Here be dragons....
//
//This function does the dirty work or scanning the XML
//string and converting it into a tree of CXMLNodes and CXMLNode
//structures.
//
//It uses "context" as a stack of what state the stream
//of characters is in
CXMLNodes* CXMLParser::ParseTagStream(CXMLStream* pData)
{
	CXMLNodes* pNodes = new CXMLNodes;

	unsigned char c;
	CXMLNode* pNode=NULL;
	CParseContext context;

	while(!pData->Eof()) {
		c=pData->GetChar();
		switch(c) {
		case '<':
			// </
			if (pData->PeekWord("/")) {
				switch(context.Type())
				{
				case pcUnknown:
				case pcText:
					pData->GetChar();
					string endtag = pData->GetTagName();
					if (endtag.empty()) throw errInvalidTagName();
					pData->SkipToEndOfTag();

					//Search for a begin tag to match the end tag we have
					//
					CXMLNode* pAncestor = pNode;
					while (pAncestor) {
						if (pData->Type() == itXML) {
							if (pAncestor->NodeName() == endtag) break;
						}

						//If we are parsing XML, a missing end tag is an error
						if (pData->Type() == itXML) throw errEndTagMismatch();

						pAncestor = pAncestor->Parent()->Parent();
					}

					//If a match is found, process it, otherwise, ignore
					//the existence of the superflous end tag
					//
					if (pAncestor) {
						//If there is any outstanding text on the end
						//of the current node, add it as a text node
						//

						if (context == pcText)
						{
							CXMLNodes* pChildren;
							if (pNode) {
								//Already have a parent node
								pChildren=pNode->Children();
							}
							else {
								//No parent - append at top level
								pChildren = pNodes;;
							}
							if (pData->AcceptNodeType(ntText)) {
								if (pData->TrimWhitespace()) {
									context.TrimCurrentData();
								}
								if (context.CurrentData().size()>0) {
									CXMLNode* pChild = pChildren->AppendChild();
									pChild->Type(ntText);
									pChild->Text(context.CurrentData());
								}
							}
							context.CurrentData().erase();

						}

						//If any ancestors exist, flatten the tree
						//
						CXMLNode* pParent = pNode;
						while (pParent) {
							if (pData->Type() == itXML) {
								if (pParent->NodeName() == endtag) break;
							}
							pParent->Collapse();
							pParent=pParent->Parent()->Parent();
						}

						context = pcUnknown;
						pNode = pParent->Parent()->Parent();

					}
					else {
						if (pData->Type()==itXML)
						{
						throw errEndTagMismatch();
						}
					}
					continue;
				}


			}
      // <
			else {
				switch(context.Type()) {
				case pcUnknown:
					context.CurrentData().erase();
					if (pData->PeekWord("?")) {
						pData->GetChar();
						context.push(pcProcessingInstruction,false);
					}
					else if (pData->PeekWord("!--")) {
  						context.push(pcComment,true);
	  					pData->GetChar();
		  				pData->GetChar();
			  			pData->GetChar();
					}
					else if (pData->PeekWord("![CDATA[")) {
						context.push(pcCData, true);
	  					pData->GetChar();
	  					pData->GetChar();
	  					pData->GetChar();
	  					pData->GetChar();
	  					pData->GetChar();
	  					pData->GetChar();
	  					pData->GetChar();
	  					pData->GetChar();
					}
					else {
						context.push(pcTag,false);
					}
					continue;
				case pcText:
					{
						CXMLNodes* pChildren;
						if (pNode) {
							//Already have a parent node
							pChildren = pNode->Children();
						}
						else {
							//No parent - append at top level
							pChildren = pNodes;
						}
						if (pData->AcceptNodeType(ntText)) {
							if (pData->TrimWhitespace()) {
								context.TrimCurrentData();
							}
							if (context.CurrentData().size()>0) {
								CXMLNode* pChild = pChildren->AppendChild();
								pChild->Type(ntText);
								pChild->Text(context.CurrentData());
							}
						}
						context.CurrentData().erase();
						context = pcUnknown;
						if (pData->PeekWord("?")) {
							pData->GetChar();
							context.push(pcProcessingInstruction,false);
						}
						else if (pData->PeekWord("!--")) {
  							context.push(pcComment,true);
	  						pData->GetChar();
		  					pData->GetChar();
			  				pData->GetChar();
						}
						else if (pData->PeekWord("![CDATA[")) {
							context.push(pcCData, true);
	  						pData->GetChar();
	  						pData->GetChar();
	  						pData->GetChar();
	  						pData->GetChar();
	  						pData->GetChar();
	  						pData->GetChar();
	  						pData->GetChar();
	  						pData->GetChar();
						}
						else {
							context.push(pcTag,false);
						}
					}
					continue;
				case pcTag:
					continue;
				case pcProcessingInstruction:
					if (context == pcProcessingInstruction) {
							//ignore it if < is in a processing instruction
							continue;
					}
				case pcComment:
		        case pcCData:
				case pcStringSingle:
				case pcStringDouble:
					break;	//do nothing if < is in a string/comment/cdata
				}
			}
			break;




		case '>':
			switch (context.Type())
			{
			case pcTag:
				{
					CXMLNodes* pChildren;
					if (pNode) {
						//Already have a parent node
						pChildren = pNode->Children();
					}
					else {
						//No parent - append at top level
						pChildren = pNodes;
					}

					//Change current node to new child
					CXMLNode* pChild = pChildren->AppendChild();
					pChild->Type(ntTag);
					pChild->ParseAttributes(context.CurrentData(),pData);

					pNode=pChild;
					context.CurrentData().erase();
					context = pcUnknown;
				}
				continue;
			case pcProcessingInstruction:
				//Without a preceeding ? and unquoted, this
				//is an error - remove it
				continue;
			}
			break;




		case '?':
			//Check for end of Processing Instruction
			if (context == pcProcessingInstruction) {
				if (pData->PeekWord(">")) {
					CXMLNodes* pChildren;
					if (pNode) {
						//Already have a parent node
						pChildren = pNode->Children();
					}
					else {
						//No parent - append at top level
						pChildren = pNodes;
					}

					//Change current node to new child
					if (pData->AcceptNodeType(ntProcessingInstruction)) {
						CXMLNode* pChild = pChildren->AppendChild();
						pChild->Type(ntProcessingInstruction);
						pChild->ParseAttributes(context.CurrentData(),pData);
					}

					context.CurrentData().erase();
					context.pop();
					pData->GetChar();
					continue;
				}
			}
			break;


		case '-':
			//Check for end of comment
			if (context == pcComment) {
				if (pData->PeekWord("->")) {
					CXMLNodes* pChildren;
					if (pNode) {
						//Already have a parent node
						pChildren = pNode->Children();
					}
					else {
						//No parent - append at top level
						pChildren = pNodes;
					}
					//Change current node to new child
					if (pData->AcceptNodeType(ntComment)) {
						CXMLNode* pChild = pChildren->AppendChild();
						pChild->Type(ntComment);
						pChild->Text(context.CurrentData());
					}
					pData->GetChar();
					pData->GetChar();
					context.pop();
					continue;
				}
			}
			break;




		case ']':
			//Check for end of CDATA block
			if (context == pcCData) {
				if (pData->PeekWord("]>")) {
					CXMLNodes* pChildren;
					if (pNode) {
						//Already have a parent node
						pChildren = pNode->Children();
					}
					else {
						//No parent - append at top level
						pChildren = pNodes;
					}
					//Change current node to new child
					if (pData->AcceptNodeType(ntCData)) {
						CXMLNode* pChild = pChildren->AppendChild();
						pChild->Type(ntCData);
						pChild->Text(context.CurrentData());
					}
					pData->GetChar();
					pData->GetChar();
					context.pop();
					continue;
				}
			}
			break;



		case '/':
		{
			if (pcTag==context.Type())
			{
				if (pData->PeekWord(">"))
				{
					CXMLNodes* pChildren;
					if (pNode) {
						//Already have a parent node
						pChildren = pNode->Children();
					}
					else {
						//No parent - append at top level
						pChildren = pNodes;
					}

					//Change current node to new child
					CXMLNode* pChild = pChildren->AppendChild();
					pChild->Type(ntTag);
					pChild->ParseAttributes(context.CurrentData(),pData);

					context.CurrentData().erase();
					context = pcUnknown;
					pData->GetChar();
					continue;
				}
			}
			break;
		}



		case '"':
			//Double quotes in tag attributes, or
			//processing instruction attributes are
			//significant
			switch (context.Type()) {
			case pcStringSingle:
				break;	//Double quotes in a single quoted string are ordinary characters
			case pcStringDouble:
				context.pop();
				break;
			case pcProcessingInstruction:
			case pcTag:
				context.push(pcStringDouble,false);
			}
			break;


		case '\x27':
			//Single quotes in tag attributes, or
			//processing instruction attributes are
			//significant
			switch (context.Type()) {
			case pcStringDouble:
				break;	//Single quotes in a double quoted string are ordinary characters
			case pcStringSingle:
				context.pop();
				break;
			case pcProcessingInstruction:
			case pcTag:
				context.push(pcStringSingle,false);
			}
			break;


		default:
			if (context == pcUnknown) {
				context = pcText;
			}
		}

		context.CurrentData().append(1,c);
	}

	if ((context == pcStringDouble) || (context == pcStringSingle))
	{
		throw errUnclosedQuote();
		context.pop();
	}

	switch(context.Type()) {
	case pcUnknown:
		break;

	case pcProcessingInstruction:
	case pcComment:
	case pcCData:
	case pcTag:
		if (pData->Type() == itXML) throw errEndTagMismatch();
	case pcText:
		{
			CXMLNodes* pChildren;
			if (pNode) {
				//Already have a parent node
				pChildren = pNode->Children();
			}
			else {
				//No parent - append at top level
				pChildren = pNodes;
			}

			if (pData->AcceptNodeType(ntText)) {
				if (pData->TrimWhitespace()) {
					context.TrimCurrentData();
				}
				if (context.CurrentData().size()>0) {
					CXMLNode* pChild = pChildren->AppendChild();
					pChild->Type(ntText);
					pChild->Text(context.CurrentData());
				}
			}
		}
		break;
	}

	return pNodes;
}

// Attribute names should only contain characters permitted in tag names
// This function is not performing the necessary validation
// Look at CXMLNode::ParseAttributes for the valid characters
CXMLNodes* CXMLParser::ParseAttributeStream(CXMLStream* pData)
{
	CXMLNodes* pNodes = new CXMLNodes();
	CXMLNode* pNode=NULL;
	CParseContext context;
	unsigned char c;
	string currentname;

	while(!pData->Eof()) {
		c=pData->GetChar();
		switch(c) {
		case ' ':
		case '\t':
		case '\r':
		case '\n':
			switch(context.Type())
			{
			case pcUnknown:
			case pcPostAttributeName:
			case pcPreAttributeValue:
				//Ignore whitespace
				continue;
			case pcAttributeName:
				//Whitespace marks end of name
				context = pcPostAttributeName;
				currentname = context.CurrentData();
				context.CurrentData().erase();
				continue;
			case pcAttributeValue:
				//Attribute complete
				if (pData->AcceptNodeType(ntAttribute)) {
					pNode = pNodes->AppendChild();
					pNode->Type(ntAttribute);
					pNode->NodeName(currentname);
					pNode->Text(context.CurrentData());
				}
				context = pcUnknown;
				context.CurrentData().erase();
				continue;
			}
			break;

		case '=':
			switch(context.Type())
			{
			case pcUnknown:
			case pcPreAttributeValue:
				//Found two = signs on the trot
				throw errInvalidAttributeSyntax();
			case pcAttributeName:
				currentname = context.CurrentData();
				context.CurrentData().erase();
				//break intentionally missing
			case pcPostAttributeName:
				//= marks the start of the value
				context = pcPreAttributeValue;
				continue;
			}
			break;

		case '"':
			//Double quotes in tag attributes, or
			//processing instruction attributes are
			//significant
			switch (context.Type()) {
			case pcStringDouble:
				context.pop();
				if (context == pcAttributeValue) {
					if (pData->AcceptNodeType(ntAttribute)) {
						pNode = pNodes->AppendChild();
						pNode->Type(ntAttribute);
						pNode->NodeName(currentname);
						pNode->Text(context.CurrentData());
					}
					context = pcUnknown;
					context.CurrentData().erase();
					continue;
				}
				break;
			case pcPreAttributeValue:
				context = pcAttributeValue;
				context.push(pcStringDouble,false);
				continue;
			default:
				break;
			}
			break;

		case '\x27':
			//Single quotes in tag attributes, or
			//processing instruction attributes are
			//significant
			switch (context.Type()) {
			case pcStringSingle:
				context.pop();
				if (context == pcAttributeValue) {
					if (pData->AcceptNodeType(ntAttribute)) {
						pNode = pNodes->AppendChild();
						pNode->Type(ntAttribute);
						pNode->NodeName(currentname);
						pNode->Text(context.CurrentData());
					}
					context = pcUnknown;
					context.CurrentData().erase();
					continue;
				}
				break;
			case pcPreAttributeValue:
				context = pcAttributeValue;
				context.push(pcStringSingle,false);
				continue;
			default:
				break;
			}
			break;


		default:
			switch (context.Type()) {
			case pcUnknown:
				context = pcAttributeName;
				break;
			case pcPreAttributeValue:
				context = pcAttributeValue;
				break;
			case pcPostAttributeName:
				context = pcAttributeName;
				break;
			}
		}
		context.CurrentData().append(1,c);
	}

	if ((context == pcStringDouble) || (context == pcStringSingle))
	{
		//Flag parse error
		context.pop();
	}


	switch(context.Type()) {

	case pcAttributeName:
		currentname = context.CurrentData();
    // Intentionally no break
	case pcPostAttributeName:
		break;

	case pcPreAttributeValue:
	case pcAttributeValue:
		if (pData->AcceptNodeType(ntAttribute)) {
			pNode = pNodes->AppendChild();
			pNode->Type(ntAttribute);
			pNode->NodeName(currentname);
			pNode->Text(context.CurrentData());
		}
		break;
	}
	return pNodes;
}

