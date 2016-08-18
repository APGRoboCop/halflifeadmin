//Error macros
//
const int ERR_BASE = 0;

#define DECLARE_ERROR(name,code) \
struct name : public errUnknown { inline virtual unsigned long Code() { return ERR_BASE + (code); }};

struct errUnknown 
{ 
	inline virtual unsigned long Code() { return ERR_BASE; }
};


//Error list
//
DECLARE_ERROR(errNoParent,1)
DECLARE_ERROR(errInvalidNodeSpec,2)
DECLARE_ERROR(errWrongNodeType,3)
DECLARE_ERROR(errIndexRange,4)
DECLARE_ERROR(errNotASingleNode,5)
DECLARE_ERROR(errIllegalNodeType,6)
DECLARE_ERROR(errEndTagMismatch,7)
DECLARE_ERROR(errInvalidKey,8)
DECLARE_ERROR(errInvalidTagName,9)
DECLARE_ERROR(errUnclosedServerScript,10)
DECLARE_ERROR(errInvalidAttributeSyntax,11)
DECLARE_ERROR(errUnclosedQuote,12)
DECLARE_ERROR(errOpenFile,13)
DECLARE_ERROR(errStreamInput,14)

