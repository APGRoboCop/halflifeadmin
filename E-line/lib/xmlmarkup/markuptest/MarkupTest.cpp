// MarkupTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "../XMLMarkup.h"

int _tmain(int argc, _TCHAR* argv[])
{
	CXMLParser p;
	CXMLStream* s = p.CreateStream();

	s->Options(poTrimWhitespace); //Have the parser trim all text and remove whitespace between tags
	s->Filter(ntText+ntTag+ntAttribute+ntCData); //Have the parser filter out processing instructions and comments
	s->Load(argv[1]);

	CXMLNodes* pNodes = p.CreateNodes(CXMLNodeSpec(s));

	string out;
	pNodes->Value(out);
	printf(out.c_str());
	printf("%i",out.size());
	delete pNodes;
	delete s;

	return 0;
}

