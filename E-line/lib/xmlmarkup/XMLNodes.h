// Nodes.h : Declaration of the CXMLNodes

#ifndef __NODES_H_
#define __NODES_H_

typedef vector<CXMLNode*> veCXMLNodes;

/////////////////////////////////////////////////////////////////////////////
// CXMLNodes
//
// This class represents a collection of nodes. It is used as:
//
// - A collection for the top level tags, comments & processing instructions in a document
// - A collection for the child tags, text strings, comments and cdata elements of a specific tag
// - A collection for the attributes of a specific tag
//
// NOTE: Deleting a nodes collections deletes all child nodes too.

class CXMLNodes
{
	friend class CXMLNode;
	friend class CXMLParser;
	friend class CXMLStream;
public:
	CXMLNodes() {
		m_Parent = 0;
	}

	~CXMLNodes();

	//Returns the number of nodes in the collection
	inline unsigned long Count() {
		return m_Nodes.size();
	}

	//Returns an item from the collection
	inline CXMLNode* Item(long index=0)
	{
		return m_Nodes[index];
	}

	//Returns the parent node or NULL if this is the top level nodes collection
	inline CXMLNode* Parent()
	{
		return m_Parent;
	}

	//Returns the Value of all nodes in the collection contactenated.
	void   Value(string& str);

	//Removes all nodes from the collection and deletes them
	void Clear();

	//Adds more nodes to the end of this collection
	void Append(CXMLNodeSpec NodeSpec);

	//Inserts more nodes before the node in the specified position
	void Insert(unsigned long Position,CXMLNodeSpec NodeSpec);

	//Removes all nodes from the colleciton and deletes them,
	//then replaces them with a new set
	void ReplaceWith(CXMLNodeSpec NodeSpec);

	//Searches for a node. Can optionally recurse into child tags
	CXMLNode* FindNode(NodeType Type, const char* NodeName, bool Recursive);

	//Removes an item from the collection
	//
	//Note: Unlike Clear and ReplaceWith,
	//this doesn't release the memory
	//for the node, just takes it out of the
	//structure and returns it
	//
	//Be sure to delete the node returned
	//or add it back into another nodes collection
	inline CXMLNode* Remove(long index=0)
	{
		CXMLNode* pNode = m_Nodes[index];
		m_Nodes.erase(m_Nodes.begin() + index);
		pNode->Parent(0);
		return pNode;
	}


private:
	veCXMLNodes	m_Nodes;
	CXMLNode*		m_Parent;

	CXMLNode* AppendChild();
	void Insert(unsigned long Position,CXMLNodes* pNodes);
	void AppendNode(CXMLNode* pNode);
	long GetIndex(CXMLNode* pChild);


	inline void Parent(CXMLNode* p) {
		m_Parent = p;
	}


	inline InputType DefaultMode()
	{
		if (m_Parent) {
			if (m_Parent->IsAttributes(this))
			{
				return itXMLAttributes;
			}
		}
		return itXML;
	}

	inline bool AllAttributes()
	{
		veCXMLNodes::iterator i;
		NodeType nt;
		for (i=m_Nodes.begin();i!=m_Nodes.end();i++)
		{
			nt = (*i)->Type();
			if (nt != ntAttribute) {
				return false;
			}
		}
		return true;
	}

	inline bool AnyAttributes()
	{
		veCXMLNodes::iterator i;
		NodeType nt;
		for (i=m_Nodes.begin();i!=m_Nodes.end();i++)
		{
			nt=(*i)->Type();
			if (nt == ntAttribute) {
				return true;
			}
		}
		return false;
	}

	void   SetNewParent(CXMLNodes* pNodes);
	CXMLNode* FindNodeByName(NodeType Type, const char* NodeName, bool Recursive);

};

#endif //__NODES_H_
