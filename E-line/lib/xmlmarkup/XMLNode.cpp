#include "XMLMarkup.h"

/////////////////////////////////////////////////////////////////////////////
// CXMLNode

CXMLNode::~CXMLNode() {
	if (m_Nodes) {
		delete m_Nodes;
	}

	if (m_Attributes) {
		delete m_Attributes;
	}
}


void CXMLNode::InsertBefore(CXMLNodeSpec NodeSpec)
{
	//Can't insert before if there is no parent
	if (!m_Parent) throw errNoParent();
	long lIndex = m_Parent->GetIndex(this);
	return m_Parent->Insert(lIndex,NodeSpec);
}

void CXMLNode::InsertAfter(CXMLNodeSpec NodeSpec)
{
	//Can't insert after if there is no parent
	if (!m_Parent) throw errNoParent();
	long lIndex = m_Parent->GetIndex(this);
	if (lIndex == m_Parent->Count()-1) {
		return m_Parent->Append(NodeSpec);
	}
	else {
		return m_Parent->Insert(lIndex+1,NodeSpec);
	}
}

CXMLNodes* CXMLNode::Children()
{
	if (m_Nodes == NULL) {
		m_Nodes = new CXMLNodes();
		m_Nodes->Parent(this);
	}
	return m_Nodes;
}

CXMLNodes* CXMLNode::Attributes()
{
	if (m_Attributes == NULL) {
		m_Attributes = new CXMLNodes();
		m_Attributes->Parent(this);
	}
    return m_Attributes;
}

void CXMLNode::Remove()
{
	if (!m_Parent) throw errNoParent();
	long lIndex = m_Parent->GetIndex(this);
	m_Parent->Remove(lIndex);
}

void CXMLNode::ReplaceWith(CXMLNodeSpec NodeSpec)
{
	if (!m_Parent) throw errNoParent();
	CXMLNodes* pNodes = CXMLParser::ParseNodeSpec(NodeSpec,DefaultMode());

	long lIndex = m_Parent->GetIndex(this);
	m_Parent->Insert(lIndex+1,pNodes);
	m_Parent->Remove(lIndex);
}

void CXMLNode::Value(string& str)
{
	switch (m_Type)
	{
	case ntAttribute:
	{
		str.append(m_NodeName);
		str.append("=\"");
		str.append(m_Text);
		str.append("\"");
		return;
	}

	case ntTag:
	{
		str.append("<");
		str.append(m_NodeName);

		//Check for attributes
		if (m_Attributes) {
			if (m_Attributes->Count() >0) {
				str.append(" ");
				m_Attributes->Value(str);
			}
		}

		if (m_Nodes) {
			//Check for tagchildren
			if (m_Nodes->Count() >0) {
				str.append(">");
				m_Nodes->Value(str);
				str.append("</");
				str.append(m_NodeName);
				str.append(">");
			}
			else {
				str.append("/>");
			}
		}
		else {
			//Close tag without children
			str.append("/>");
		}
		return;
	}

	case ntProcessingInstruction:
		str.append("<?");
		str.append(m_NodeName);
		//Check for attributes
		if (m_Attributes) {
			if (m_Attributes->Count() >0) {
				str.append(" ");
				m_Attributes->Value(str);
			}
		}
		str.append("?>");
		return;

	case ntComment:
		str.append("<!--");
		str.append(m_Text);
		str.append("-->");
		return;

	case ntCData:
		str.append("<![CDATA[");
		str.append(m_Text);
		str.append("]]>");
		return;
	}

	str.append(m_Text);
	return;
}



//Makes all children of this node siblings. Assumes this is the
//last child of its parent, as this is only used during node
//creation, when we realise we have made a mistake due to mismatched
//end tags
void CXMLNode::Collapse()
{
	CXMLNode* pChild;
	if (m_Nodes)
	{
		while (m_Nodes->Count() > 0)
		{
			//Obtain our first child and remove from collection
			pChild = m_Nodes->Remove(0);
			//At it as our sibling
			m_Parent->AppendNode(pChild);
		}
	}
}




//It has been determined that this node is a tag, client script
//or processing instruction, and therefore may have attributes
//
//Note: This function ignores CombiningChar's and Extender's
//http://www.w3.org/TR/2000/REC-xml-20001006#sec-common-syn
void CXMLNode::ParseAttributes(string attr,CXMLStream* pData)
{
	const string alpha = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789:-_.";

	long sep = attr.find_first_not_of(alpha);
	if (sep >=0 ) {
		m_NodeName = attr.substr(0,sep);
		CXMLStream pStream;
		pStream.LoadXML(attr.substr(sep).c_str());
		pStream.CopyConfig(pData);
		m_Attributes = CXMLParser::ParseAttributeStream(&pStream);
		m_Attributes->Parent(this);
	}
	else {
		m_NodeName = attr;
	}
}

CXMLNode* CXMLNode::FindNode(NodeType Type, const char* NodeName, bool Recursive)
{
  return FindNodeByName(Type, NodeName, Recursive, true);
}


CXMLNode* CXMLNode::FindNodeByName(NodeType Type, const char* NodeName, bool Recursive, bool FirstCall)
{
	if ((Type & m_Type)!=0)
	{
		if (strlen(NodeName)==0)
		{
			return this;
		}
		if (strcmp(m_NodeName.c_str(),NodeName)==0)
		{
			return this;
		}
	}

	if ((m_Attributes)&&(Recursive||FirstCall))
	{
		CXMLNode* pRes = m_Attributes->FindNodeByName(Type, NodeName, Recursive);
		if (pRes) return pRes;
	}

	if ((m_Nodes)&&(Recursive||FirstCall))
	{
	    CXMLNode* pRes = m_Nodes->FindNodeByName(Type, NodeName, Recursive);
		if (pRes) return pRes;
	}

	return NULL;
}


bool CXMLNode::GetAttribute(const char* Name, string& Value)
{
	CXMLNode* pAttr=FindNodeByName(ntAttribute,Name,false,true);
	if (pAttr) {
		Value=pAttr->Text();
		return true;
	}
	else {
		//Value.clear();
		Value.erase();
		return false;
	}
}

bool CXMLNode::GetAttribute(const char* Name, int& Value)
{
	CXMLNode* pAttr=FindNodeByName(ntAttribute,Name,false,true);
	if (pAttr) {
		Value = atoi(pAttr->Text().c_str());
		return true;
	}
	else {
		Value = 0;
		return false;
	}
}

bool CXMLNode::GetAttribute(const char* Name, float& Value)
{
	CXMLNode* pAttr=FindNodeByName(ntAttribute,Name,false,true);
	if (pAttr) {
		Value = atof(pAttr->Text().c_str());
		return true;
	}
	else {
		Value = 0.0;
		return false;
	}
}



void CXMLNode::SetAttribute(const char* Name, const char* Value)
{
	CXMLNode* pAttr=FindNodeByName(ntAttribute,Name,false,true);
	if (pAttr==NULL) {
		pAttr->Type(ntAttribute);
		pAttr->NodeName(string(Name));

		if (m_Attributes == NULL) {
			m_Attributes = new CXMLNodes;
		}
		m_Attributes->AppendNode(pAttr);
	}

	pAttr->Children()->ReplaceWith(Value);
}
