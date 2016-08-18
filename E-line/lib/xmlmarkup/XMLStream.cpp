#include "XMLMarkup.h"

//Copied from various windows include files to prevent having to include windows.h and all that jazz
#ifdef WIN32
#include <windows.h>
#endif

/////////////////////////////////////////////////////////////////////////////
// CXMLStream


void CXMLStream::LoadXML(const char* src)
{
	m_Length = strlen(src);
	m_Data = src;
	m_Position = 0;	
}


void CXMLStream::Load(const char* FileName)
{
	m_Length=0;
	m_Position=0;
	m_Data="";
	m_File = fopen(FileName,"rb");
	if (m_File)
	{
#ifdef LINUX
		struct stat file;
		if ( stat( FileName, &file ) >= 0 ) {
			m_Length=file.st_size;
		} else {
			throw errOpenFile();
		}
#else
		HANDLE fFile;
		fFile=CreateFileA(FileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
		if (fFile)
		{
			m_Length=GetFileSize(fFile, NULL);
			CloseHandle(fFile);
			if (m_Length==0xFFFFFFFF)
			{
				throw errOpenFile();
			}
		}
#endif	
    }  // if
	else {
		throw errOpenFile();
	}
	
}



bool CXMLStream::PeekWord(string str)
{
	//Ensure there are enough characters loaded into the buffer
	//to peek at.
  long lRead;
	long pl = str.size();
	if (m_Position + pl > m_Length) return false;
	lRead=ReadData(pl);

  if (lRead<pl) return false;

	for (long i=0; i<pl;i++) {
		if (CharAt(m_Position+i) != str[i]) return false;
	}

	return true;
}


bool CXMLStream::PeekWordI(string str)
{
	//Ensure there are enough characters loaded into the buffer
	//to peek at.
  long lRead;
	long pl = str.size();

	if (m_Position + pl > m_Length) return false;
	lRead=ReadData(pl);

  if (lRead<pl) return false;

	unsigned short c1,c2;
	for (long i=0; i<pl;i++) {
		c1=CharAt(m_Position+i);
		c2=str[i];
		if (c1 != c2 ) {
			if ((c1>= L'A') && (c1<=L'Z') && (c2>=L'a') && (c2<=L'z') && c1+32==c2) continue;
			if ((c1>= L'a') && (c1<=L'z') && (c2>=L'A') && (c2<=L'Z') && c1-32==c2) continue;
			return false;
		}
	}

	return true;
}


// This method uses a truly crude method
//   to determine if an ampersand (&) should be escaped
// It looks at the next 7 chars for any of ';', ' ', TAB, CR, LF
// If it finds ';' first, the ampersand is likely to be
//   part of an entity and is not to be escaped
// Returns: true if possible entity (don't escape)
//          false otherwise (escape)
bool CXMLStream::PossibleEntity()
{
  long i;
  long lRead;
  long lMaxLen=7;
  unsigned short c;
	if (m_Position + lMaxLen > m_Length)
  {
    lMaxLen=m_Length-m_Position;
  };
	//Ensure there are enough characters loaded into the buffer
	//to peek at.
	lRead=ReadData(lMaxLen);
  if (lRead >lMaxLen) lRead = lMaxLen;

  i=0;
  while (i<lRead)
  {
    c=m_Data[m_Position-m_Offset+i];
    if (c == L';') return (i>2);
    if (((c < L'A') || (c > L'Z')) && ((c < L'a') || (c > L'z')) &&
        ((c < L'0') || (c > L'9')) && (c != L'#')) return false;
    i++;
  }
  return false;
}


string CXMLStream::GetTagName()
{
  const string alpha = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789:-_.";
  string tagname;

  while (!Eof()) {
    long sep=m_Data.find_first_not_of(alpha, m_Position-m_Offset);
    if (sep>=0)
	{
      tagname += m_Data.substr(m_Position-m_Offset, sep-(m_Position-m_Offset));
	  return tagname;
	}
	tagname+=m_Data.substr(m_Position-m_Offset);
	m_Position+=tagname.size();
	ReadData(MIN_BUFFER);
  }

  throw errInvalidTagName();
}

void CXMLStream::SkipToEndOfTag()
{
	while ((m_Position < m_Length) && (m_Data[m_Position-m_Offset] != L'>')) {
		m_Position++;
		if (m_Position-m_Offset == m_Data.size()) {
			ReadData(MIN_BUFFER);
		}
	}
	if (m_Position < m_Length) {
		m_Position++;
		if (m_Position-m_Offset == m_Data.size()) {
			ReadData(MIN_BUFFER);
		}
	}
}


//This function attempts to ensure that there are MinLen
//characters from m_Position available in the string buffer
//by reading from disk if necessary.
long CXMLStream::ReadData(long MinLen)
{
	long lRead;
	unsigned long lBytesRead;
	char *strTemp;

	//Work out it there are Minlen characters
	//available without reading from disk.  If so,
	//no need to do anything,
	long lRemaining = m_Data.size() - (m_Position - m_Offset);
	if (MinLen <= lRemaining)
	{
		return lRemaining;
	}

	//If stream isn't backed by disk storage,
	//ignore calls to read data - there is nothing
	//to read from


	if (!m_File) return lRemaining;

	//Remove any characters before m_Position - we
	//aren't interested in them anymore
	if (m_Data.size()>0) {
		if (lRemaining>0) {
			memcpy(&m_Data[0],&m_Data[m_Position-m_Offset],lRemaining*2);
		}
		m_Offset+= m_Data.size() - lRemaining;
		lRead = m_Data.size() - lRemaining;
	}
	else {
		lRead = MIN_BUFFER;
	}

	//Workout how many characters we can read
	//long nRead = MIN_BUFFER - lRemaining;

	if (lRead > m_Length - m_Position - lRemaining)
	{
		lRead = m_Length - m_Position - lRemaining;
	}

	if (lRead>0) {
		strTemp=new char[lRead+1];
		lBytesRead=fread(&strTemp[0],1,lRead,m_File);
		if (ferror(m_File)) {
			throw errStreamInput();
		}
		strTemp[lBytesRead]=0;

		//Resize the buffer
		m_Data.resize(lRemaining + lRead);
		strcpy(&m_Data[lRemaining],strTemp);
		delete strTemp;
	}
  return lRemaining + lRead;
}
