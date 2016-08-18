#include "XMLMarkup.h"

/////////////////////////////////////////////////////////////////////////////
// CXMLNodes

void CXMLNodes::Insert(unsigned long Index, CXMLNodeSpec NodeSpec)
{
	Index--;
	if (Index >= m_Nodes.size()) throw errIndexRange();

	CXMLNodes* pNodes = CXMLParser::ParseNodeSpec(NodeSpec,DefaultMode());
	pNodes->SetNewParent(this);

	m_Nodes.insert(m_Nodes.begin()+Index,pNodes->m_Nodes.begin(),pNodes->m_Nodes.end());
}

void CXMLNodes::Append(CXMLNodeSpec NodeSpec)
{
	CXMLNodes* pNodes = CXMLParser::ParseNodeSpec(NodeSpec,DefaultMode());
	pNodes->SetNewParent(this);
	m_Nodes.insert(m_Nodes.end(),pNodes->m_Nodes.begin(),pNodes->m_Nodes.end());
}

void CXMLNodes::Clear()
{
	veCXMLNodes::iterator i;
	for(i = m_Nodes.begin(); i<m_Nodes.end(); i++)
	{
		delete (*i);
	}

	m_Nodes.clear();
}



void CXMLNodes::ReplaceWith(CXMLNodeSpec NodeSpec)
{
	CXMLNodes* pNodes = CXMLParser::ParseNodeSpec(NodeSpec,DefaultMode());
	pNodes->SetNewParent(this);
	Clear();
	m_Nodes.insert(m_Nodes.begin(),pNodes->m_Nodes.begin(),pNodes->m_Nodes.end());
	delete pNodes;
}

CXMLNodes::~CXMLNodes()
{
	veCXMLNodes::iterator i;
	for(i=m_Nodes.begin();i!=m_Nodes.end();i++)
	{
		delete (*i);
	}
}

void CXMLNodes::Insert(unsigned long Position,CXMLNodes* pNodes)
{
	pNodes->SetNewParent(this);
	if (Position < m_Nodes.size()) {
		m_Nodes.insert(m_Nodes.begin()+Position,pNodes->m_Nodes.begin(),pNodes->m_Nodes.end());
	}
	else {
		m_Nodes.insert(m_Nodes.end(),pNodes->m_Nodes.begin(),pNodes->m_Nodes.end());
	}
}


CXMLNode* CXMLNodes::AppendChild()
{
	CXMLNode* pNode = new CXMLNode;
	pNode->Parent(this);
	m_Nodes.push_back(pNode);
	return pNode;
}


void CXMLNodes::AppendNode(CXMLNode* pNode)
{
	pNode->Parent(this);
	m_Nodes.push_back(pNode);
}

void CXMLNodes::Value(string& str)
{
	veCXMLNodes::iterator i;
	for(i = m_Nodes.begin(); i!=m_Nodes.end(); i++)
	{
		CXMLNode* pNode = (*i);
		pNode->Value(str);
		switch (pNode->Type())
		{
		case ntAttribute:
			if (i + 1 != m_Nodes.end()) {
				str.append(" ");
			}
		}
	}
}

long CXMLNodes::GetIndex(CXMLNode* pChild)
{
	veCXMLNodes::iterator i;
	long pos=0;
	for(i = m_Nodes.begin(); i<m_Nodes.end(); i++)
	{
		if (*i == pChild) return pos;
		pos++;
	}
	return -1;
}

CXMLNode* CXMLNodes::FindNode(NodeType Type, const char* NodeName, bool Recursive)
{
    return FindNodeByName(Type, NodeName, Recursive);
}

CXMLNode* CXMLNodes::FindNodeByName(NodeType Type, const char* NodeName, bool Recursive)
{
	veCXMLNodes::iterator i;
	for(i = m_Nodes.begin(); i<m_Nodes.end(); i++)
	{
		CXMLNode* pNode=(*i);
		CXMLNode* pRes=pNode->FindNodeByName(Type, NodeName, Recursive, false);
		if (pRes) return pRes;
	}
	return NULL;
}


//Called just before this Nodes object is about to be
//merged into another Nodes object then deleted. This
//sets the parent of all child nodes in this collection
void CXMLNodes::SetNewParent(CXMLNodes* pNodes)
{
	veCXMLNodes::iterator i;
	for(i = m_Nodes.begin(); i<m_Nodes.end(); i++)
	{
		(*i)->Parent(pNodes);
	}
}