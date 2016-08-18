// Node.h : Declaration of the CXMLNode

#ifndef __NODE_H_
#define __NODE_H_

class CXMLNodes;
class CXMLStream;
class CXMLNodeSpec;

/////////////////////////////////////////////////////////////////////////////
// CXMLNode
//
// A node represents one element of a document. That can be:
//
// - Plain text
// - A tag
// - A processing instruction (e.g. <?xml?>)
// - A attribute of a tag or processing instruction
// - A comment
// - A <[CDATA{ section
//
// Depending on its type a node can have:
//
// - A name
// - A text value
// - A collection of attributes
// - A collection of child nodes
//
// NOTE: Deleting a node object deletes all child nodes & attributes too.

class CXMLNode
{
	friend class CXMLNodes;
	friend class CXMLParser;
	friend class CXMLStream;
public:
	CXMLNode()
	{
	    m_Nodes=NULL;
	    m_Attributes=NULL;
	    m_Parent=NULL;
	}

	~CXMLNode();

	//Returns the type of this node- one of: ntText, ntTag, ntAttribute,
	//ntProcessingInstruction, ntComment, ntCData
	inline NodeType Type() const {
		return m_Type;
	}

	//Returns the name of this node
	inline const string& NodeName() const {
		return m_NodeName;
	}

	//Returns the XML text of this node and all its attributes / children.
	//This is like element.OuterHTML in IE. For the equivilent of element.InnerHTML,
	//use Children()->Value(&str);
	void Value(string& str);

	//Returns the text of the node - unlike Value() this returns the
	//simple text of this node and doesn't include the XML of any
	//child nodes. Only valid on comment, text and attribute nodes.
	inline const string& Text() const {
		return m_Text;
	}

	//Returns the parent nodes collection. To get the parent node you
	//need to do parent()->parent(). Returns NULL if this node has been
	//removed from a collection
	inline CXMLNodes* Parent()
	{
		return m_Parent;
	}

	//Retrives the value of a specified attribute into the
	//Value argument. Returns true if an attribute was found
	//to retrive. The string value will be converted to int/float if required
	bool GetAttribute(const char* Name, string& Value);
	bool GetAttribute(const char* Name, int& Value);
	bool GetAttribute(const char* Name, float& Value);

	//Sets an attribute of this node, replacing any existing
	//attribute with the same name, or creating a new one
	//if necessary
	void SetAttribute(const char* Name, const char* Value);

	//Returns the set of attributes - if there are no attributes
	//this returns an empty CXMLNodes object, never NULL
    CXMLNodes* Attributes();

	//Returns the set of child nodes - if there are no children
	//this returns an empty CXMLNodes object, never NULL
	CXMLNodes* Children();

	//Inserts more nodes as siblings to this node after it
	void InsertAfter(CXMLNodeSpec NodeSpec);

	//Inserts more nodes as siblings to this node before it
	void InsertBefore(CXMLNodeSpec NodeSpec);

	//Removes this node from its parent node then replaces
	//it with the new nodes specied.
	//
	//Note: this does not delete this node object - you must
	//do that or add it to a different CXMLNodes collection.
	void ReplaceWith(CXMLNodeSpec NodeSpec);

	//Removes this node from its parent node. Again, this
	//does not delete this object
	void Remove();

	//Searches the attributes and childnodes for a node
	//(and attributes of childnodes when in Recursive mode)
	//for a node with the specified name.
	//
	//This can be used to find tags, attributes or processing instructions
	CXMLNode* FindNode(NodeType Type, const char* NodeName, bool Recursive);

private:
	//Member Variables
	CXMLNodes* m_Nodes;
	CXMLNodes* m_Attributes;
	CXMLNodes* m_Parent;

	NodeType		m_Type;
	string			m_NodeName;
	string			m_Text;

	inline void Parent(CXMLNodes* p) {
		m_Parent = p;
	}

	inline void Text(string t) {
		m_Text = t;
	}

	inline void Type(NodeType t) {
		m_Type = t;
	}

	inline void NodeName(string newVal) {
		m_NodeName = newVal;
	}

	inline bool IsAttributes(CXMLNodes* pTest) {
		return m_Attributes == pTest;
	}

	inline InputType DefaultMode()
	{
		if (m_Type == ntAttribute) {
			return itXMLAttributes;
		}
		return itXML;
	}

    CXMLNode* FindNodeByName(NodeType Type, const char* NodeName, bool Recursive, bool FirstCall);
	void ParseAttributes(string attr,CXMLStream* pData);
	void Collapse();
};

#endif //__NODE_H_
