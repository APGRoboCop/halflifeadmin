//////////////////////////////////////////////////////////////////////////////
// XML Markup Parser - (C) Ravenous Bugblatter Beast
//
// Usage:
//
// To scan an XML string:
//
//	CXMLParser p;
//  try {
//	    CXMLNodes* pNodes = p.CreateNodes(CXMLNodeSpec(strXML));
//  }
//  catch (errUnknown& e) {
//		/* Handle error. You can also catch any specific error in errors.h */
//  }
//
//
//
// To scan an XML file:
//
//	CXMLParser p;
//	CXMLStream* s = p.CreateStream();
//  /* set any options on the NodeStream here - e.g. trim whitepace */
//  try {
//	    s->Load(strFilename);
//	    CXMLNodes* pNodes = p.CreateNodes(CXMLNodeSpec(s));
//  }
//  catch (errUnknown& e) {
//		/* Handle error. You can also catch any specific error in errors.h */
//  }
//
//
//
// To scan an XML string with parsing options:
//
//	CXMLParser p;
//	CXMLStream* s = p.CreateStream();
//  s->Options(poTrimWhitespace);
//  try {
//	    s->LoadXML(strXML);
//	    CXMLNodes* pNodes = p.CreateNodes(CXMLNodeSpec(s));
//  }
//  catch (errUnknown& e) {
//		/* Handle error. You can also catch any specific error in errors.h */
//  }
//
//
//  The CXMLNodes collection created is not tied to the parser that created it
//  you can let the parser object go out of scope before you delete the CXMLNodes
//
//  See Nodes.h and Node.h for information on navigating and manipulating
//  the CXMLNodes collection returned.
//

#ifndef __PARSER_H_
#define __PARSER_H_

class CXMLStream;
class CXMLNodes;
class CXMLNode;


typedef enum tagNodeType
{
	ntText=1,
	ntTag=2,
	ntAttribute=4,
	ntCData=8,
	ntProcessingInstruction=16,
	ntComment=32,
	ntAll=65535
} NodeType;

typedef enum tagInputType
{
	itXML,
	itXMLAttributes
} InputType;

typedef enum tagParseOptions
{
	poDefault=0,
	poTrimWhitespace=1,
} ParseOptions;


/////////////////////////////////////////////////////////////////////////////
// NodeSpecification

typedef enum tagNodeSpecType
{
	nstNodes,
	nstNode,
	nstNodeStream,
	nstString
} NodeSpecType;

class CXMLNodeSpec {
private:
	NodeSpecType m_Type;
	string m_String;
	CXMLNodes* m_Nodes;
	CXMLNode* m_Node;
	CXMLStream* m_NodeStream;

public:
	CXMLNodeSpec(const char* s) {
		m_Type=nstString;
		m_String = s;
	}

	CXMLNodeSpec(CXMLNodes* n) {
		m_Type=nstNodes;
		m_Nodes = n;
	}

	CXMLNodeSpec(CXMLNode* n) {
		m_Type=nstNode;
		m_Node= n;
	}

	CXMLNodeSpec(CXMLStream* s) {
		m_Type=nstNodeStream;
		m_NodeStream = s;
	}

	string& String() {
		return m_String;
	}

	CXMLNodes* Nodes() {
		return m_Nodes;
	}

	CXMLNode* Node() {
		return m_Node;
	}

	CXMLStream* NodeStream() {
		return m_NodeStream;
	}

	NodeSpecType Type() { return m_Type; }
};

/////////////////////////////////////////////////////////////////////////////
// CXMLParser

class CXMLParser
{
	friend class CXMLNode;
	friend class CXMLNodes;
	friend class CXMLStream;

public:
	CXMLNodes* CreateNodes(CXMLNodeSpec);
	CXMLStream* CreateStream();

private:
	static CXMLNodes* ParseNodeSpec(CXMLNodeSpec NodeSpec,InputType DefaultType=itXML);
	static CXMLNodes* ParseStream(CXMLStream* Stream);
	static CXMLNodes* ParseTagStream(CXMLStream* Stream);
	static CXMLNodes* ParseAttributeStream(CXMLStream* Stream);
};

#endif //__PARSER_H_
