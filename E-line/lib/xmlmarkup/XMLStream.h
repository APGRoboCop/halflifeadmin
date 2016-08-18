// NodeStream.h : Declaration of the CXMLStream

#ifndef NODESTREAM_H_
#define NODESTREAM_H_

#define MIN_BUFFER 4000

#include <stdio.h>

/////////////////////////////////////////////////////////////////////////////
// CXMLStream
class CXMLStream
{
	friend class CXMLNode;
	friend class CXMLNodes;
	friend class CXMLParser;

public:
	CXMLStream()
	{
		m_Type = itXML;
		m_Filter = 0xFFFFFFFF;
		m_ParseOptions = poDefault;
		m_File = 0;
		m_Offset =0;
		m_Position=0;
	}

	virtual ~CXMLStream()
	{
		if (m_File) fclose(m_File);
	}

	//Configures the stream to use the string src as the source of XML Data
	//
	void LoadXML(const char* src);

	//Configures the stream to read XML Data from Filename
	//
    void Load(const char* FileName);

	//Returns the current filter settings
	long Filter()
	{
		return m_Filter;
	}

	//Sets the filter. Use values from the NodeType enumeration in parser.h. Only
	//those types of node specified here will be copied into the CXMLNodes collection
	//build. You can filter out Text,Comments,CData,ProcessingInstructions & Attributes,
	//but not Tags.
	void Filter(long RHS)
	{
		m_Filter = RHS;
	}

	//Returns the current parse options
	long Options()
	{
		return m_ParseOptions;
	}

	//Sets the parse options. Use values from the ParseOptions enumeration
	//in parser.h. Currently the only option is to remove whitespace.
	void Options(long newVal)
	{
		m_ParseOptions = newVal;
	}


// Internal Use
private:
	long Position()
	{
		return m_Position;
	}

	inline InputType Type()
	{
		return m_Type;
	}

	void Type(InputType it)
	{
		m_Type = it;
	}

	inline unsigned char CharAt(long x)
	{
		return m_Data[x - m_Offset];
	}

	inline unsigned char GetChar()
	{
		if (m_Position - m_Offset == m_Data.size()) ReadData(MIN_BUFFER);
		return CharAt(m_Position++);
	}

	inline bool Eof()
	{
		return m_Position == m_Length;
	}

	inline bool AcceptNodeType(NodeType nt)
	{
		return (m_Filter & nt) != 0 ;
	}

	inline void CopyConfig(CXMLStream* o)
	{
		m_Filter = o->m_Filter;
		m_ParseOptions = o->m_ParseOptions;
	}


	inline bool TrimWhitespace()
	{
		return ((m_ParseOptions & poTrimWhitespace)!=0);
	}

	bool PeekWord(string str);
	bool PeekWordI(string str);
	bool PossibleEntity();
	string GetTagName();
	void SkipToEndOfTag();
	long ReadData(long MinLen);

	// Member variables
	string m_Data;
	long m_Position;
	long m_Length;
	long m_Offset;
	InputType m_Type;
	long m_Filter;
	long m_ParseOptions;

	//File Handling
	FILE* m_File;
};

#endif /* NODESTREAM_H_ */

