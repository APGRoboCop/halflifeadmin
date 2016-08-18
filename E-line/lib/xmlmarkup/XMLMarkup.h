#if !defined(MARKUP_H)
#define MARKUP_H

//For file I/O
#include <sys/types.h>
#include <sys/stat.h>
#ifndef WIN32
#  include <unistd.h>
#endif
#ifdef _WIN32
#  include <io.h>
#  include <direct.h>

#endif

//For STL collections
#include <string>
#include <vector>
#include <map>
#include <fstream>
using namespace std;

//For this library
#include "XMLParser.h"
#include "XMLNode.h"
#include "XMLNodes.h"
#include "XMLStream.h"
#include "errors.h"


#endif
