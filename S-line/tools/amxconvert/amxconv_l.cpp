#include "amxconv_l.h"
#include "amx_l.h"

constexpr char OFFSET_DIFF_V5 = 12;
constexpr char OFFSET_DIFF_V4 = 8;

int check_header_type(const AMX_LINUX_HEADER& _oLinHeader, int& _iSizeChange)
{
	constexpr long c_lShortMax = 0xffff;

    if(_oLinHeader.magic != AMX_MAGIC) {
        return INVAL_AMX_HDR;
    } // if

    if(_oLinHeader.file_version < MIN_FILE_VERSION || _oLinHeader.file_version > MAX_FILE_VERSION ||
        _oLinHeader.amx_version != MIN_FILE_VERSION) {
        return INVAL_AMX_VER;
    } // if

    if((_oLinHeader.flags & AMX_FLAG_CELL_64) == AMX_FLAG_CELL_64)
        return AMD64_AMX;

    if(_oLinHeader.num_publics > c_lShortMax) {
        // A Windows header
        // Converting this to Linux would increase the file size
        if(_oLinHeader.file_version == 4)
            _iSizeChange = +OFFSET_DIFF_V4;
        else
            _iSizeChange = +OFFSET_DIFF_V5;
        return WIN32_AMX;

    } else {
        // A Linux header
        // Converting this to Windows would decrease the file size
        if(_oLinHeader.file_version == 4)
            _iSizeChange = -OFFSET_DIFF_V4;
        else
            _iSizeChange = -OFFSET_DIFF_V5;
        return LINUX_AMX;

    } // if-else

    //return INVAL_AMX;

} // check_header_type(amx_linux_header&)

// convert header from Windows to Linux format
char* convert_header(char* _pcFile, AMX_LINUX_HEADER& _oHeader)
{

    AMX_WIN32_HEADER* poOHeader = reinterpret_cast<AMX_WIN32_HEADER*>(_pcFile);
    copy_header(_oHeader, *poOHeader);
    return (_pcFile + sizeof(AMX_WIN32_HEADER));
} // convert_header(char*, amx_linux_header*)

// convert header from Windows to Linux format, version 4
char* convert_header(char* _pcFile, AMX_LINUX_HEADER_V4& _oHeader)
{

    AMX_WIN32_HEADER_V4* poOHeader = reinterpret_cast<AMX_WIN32_HEADER_V4*>(_pcFile);
    copy_header(_oHeader, *poOHeader);
    return (_pcFile + sizeof(AMX_WIN32_HEADER_V4));
} // convert_header(char*, amx_linux_header_v4*)

// convert header from Linux to Windows format
char* convert_header(char* _pcFile, AMX_WIN32_HEADER& _oHeader)
{

    AMX_LINUX_HEADER* poOHeader = reinterpret_cast<AMX_LINUX_HEADER*>(_pcFile);
    copy_header(_oHeader, *poOHeader);
    return (_pcFile + sizeof(AMX_LINUX_HEADER));

} // convert_header(char*, amx_win32_header*)

// convert header from Linux to Windows format, version 4
char* convert_header(char* _pcFile, AMX_WIN32_HEADER_V4& _oHeader)
{

    AMX_LINUX_HEADER_V4* poOHeader = reinterpret_cast<AMX_LINUX_HEADER_V4*>(_pcFile);
    copy_header(_oHeader, *poOHeader);
    return (_pcFile + sizeof(AMX_LINUX_HEADER_V4));

} // convert_header(char*, amx_win32_header_v4*)

void copy_header(AMX_WIN32_HEADER& _oWinHeader, AMX_LINUX_HEADER& _oLinHeader)
{

    _oWinHeader.size = _oLinHeader.size - OFFSET_DIFF_V5;
    _oWinHeader.magic = _oLinHeader.magic;
    _oWinHeader.file_version = _oLinHeader.file_version;
    _oWinHeader.amx_version = _oLinHeader.amx_version;
    _oWinHeader.flags = _oLinHeader.flags;
    _oWinHeader.defsize = _oLinHeader.defsize;
    _oWinHeader.cod = _oLinHeader.cod - OFFSET_DIFF_V5;
    _oWinHeader.dat = _oLinHeader.dat - OFFSET_DIFF_V5;
    _oWinHeader.hea = _oLinHeader.hea - OFFSET_DIFF_V5;
    _oWinHeader.stp = _oLinHeader.stp - OFFSET_DIFF_V5;
    _oWinHeader.cip = _oLinHeader.cip;
    _oWinHeader.num_publics = static_cast<short>(_oLinHeader.num_publics);
    _oWinHeader.publics = _oLinHeader.publics - OFFSET_DIFF_V5;
    _oWinHeader.num_natives = static_cast<short>(_oLinHeader.num_natives);
    _oWinHeader.natives = _oLinHeader.natives - OFFSET_DIFF_V5;
    _oWinHeader.num_libraries = static_cast<short>(_oLinHeader.num_libraries);
    _oWinHeader.libraries = _oLinHeader.libraries - OFFSET_DIFF_V5;
    _oWinHeader.num_pubvars = static_cast<short>(_oLinHeader.num_pubvars);
    _oWinHeader.pubvars = _oLinHeader.pubvars - OFFSET_DIFF_V5;
    _oWinHeader.num_tags = static_cast<short>(_oLinHeader.num_tags);
    _oWinHeader.tags = _oLinHeader.tags - OFFSET_DIFF_V5;
    _oWinHeader.reserved = static_cast<short>(_oLinHeader.reserved);

} // copy_header(amx_win32_header&, amx_linux_header&)

void copy_header(AMX_WIN32_HEADER_V4& _oWinHeader, AMX_LINUX_HEADER_V4& _oLinHeader)
{

    _oWinHeader.size = _oLinHeader.size - OFFSET_DIFF_V4;
    _oWinHeader.magic = _oLinHeader.magic;
    _oWinHeader.file_version = _oLinHeader.file_version;
    _oWinHeader.amx_version = _oLinHeader.amx_version;
    _oWinHeader.flags = _oLinHeader.flags;
    _oWinHeader.defsize = _oLinHeader.defsize;
    _oWinHeader.cod = _oLinHeader.cod - OFFSET_DIFF_V4;
    _oWinHeader.dat = _oLinHeader.dat - OFFSET_DIFF_V4;
    _oWinHeader.hea = _oLinHeader.hea - OFFSET_DIFF_V4;
    _oWinHeader.stp = _oLinHeader.stp - OFFSET_DIFF_V4;
    _oWinHeader.cip = _oLinHeader.cip;
    _oWinHeader.num_publics = static_cast<short>(_oLinHeader.num_publics);
    _oWinHeader.publics = _oLinHeader.publics - OFFSET_DIFF_V4;
    _oWinHeader.num_natives = static_cast<short>(_oLinHeader.num_natives);
    _oWinHeader.natives = _oLinHeader.natives - OFFSET_DIFF_V4;
    _oWinHeader.num_libraries = static_cast<short>(_oLinHeader.num_libraries);
    _oWinHeader.libraries = _oLinHeader.libraries - OFFSET_DIFF_V4;
    _oWinHeader.num_pubvars = static_cast<short>(_oLinHeader.num_pubvars);
    _oWinHeader.pubvars = _oLinHeader.pubvars - OFFSET_DIFF_V4;

} // copy_header(amx_win32_header_v4&, amx_linux_header_v4&)

void copy_header(AMX_LINUX_HEADER& _oLinHeader, AMX_WIN32_HEADER& _oWinHeader)
{

    _oLinHeader.size = _oWinHeader.size + OFFSET_DIFF_V5;
    _oLinHeader.magic = _oWinHeader.magic;
    _oLinHeader.file_version = _oWinHeader.file_version;
    _oLinHeader.amx_version = _oWinHeader.amx_version;
    _oLinHeader.flags = _oWinHeader.flags;
    _oLinHeader.defsize = _oWinHeader.defsize;
    _oLinHeader.cod = _oWinHeader.cod + OFFSET_DIFF_V5;
    _oLinHeader.dat = _oWinHeader.dat + OFFSET_DIFF_V5;
    _oLinHeader.hea = _oWinHeader.hea + OFFSET_DIFF_V5;
    _oLinHeader.stp = _oWinHeader.stp + OFFSET_DIFF_V5;
    _oLinHeader.cip = _oWinHeader.cip;
    _oLinHeader.num_publics = _oWinHeader.num_publics;
    _oLinHeader.publics = _oWinHeader.publics + OFFSET_DIFF_V5;
    _oLinHeader.num_natives = _oWinHeader.num_natives;
    _oLinHeader.natives = _oWinHeader.natives + OFFSET_DIFF_V5;
    _oLinHeader.num_libraries = _oWinHeader.num_libraries;
    _oLinHeader.libraries = _oWinHeader.libraries + OFFSET_DIFF_V5;
    _oLinHeader.num_pubvars = _oWinHeader.num_pubvars;
    _oLinHeader.pubvars = _oWinHeader.pubvars + OFFSET_DIFF_V5;
    _oLinHeader.num_tags = _oWinHeader.num_tags;
    _oLinHeader.tags = _oWinHeader.tags + OFFSET_DIFF_V5;
    _oLinHeader.reserved = _oWinHeader.reserved;

} // copy_header(amx_win32_header&, amx_linux_header&)

void copy_header(AMX_LINUX_HEADER_V4& _oLinHeader, AMX_WIN32_HEADER_V4& _oWinHeader)
{

    _oLinHeader.size = _oWinHeader.size + OFFSET_DIFF_V4;
    _oLinHeader.magic = _oWinHeader.magic;
    _oLinHeader.file_version = _oWinHeader.file_version;
    _oLinHeader.amx_version = _oWinHeader.amx_version;
    _oLinHeader.flags = _oWinHeader.flags;
    _oLinHeader.defsize = _oWinHeader.defsize;
    _oLinHeader.cod = _oWinHeader.cod + OFFSET_DIFF_V4;
    _oLinHeader.dat = _oWinHeader.dat + OFFSET_DIFF_V4;
    _oLinHeader.hea = _oWinHeader.hea + OFFSET_DIFF_V4;
    _oLinHeader.stp = _oWinHeader.stp + OFFSET_DIFF_V4;
    _oLinHeader.cip = _oWinHeader.cip;
    _oLinHeader.num_publics = _oWinHeader.num_publics;
    _oLinHeader.publics = _oWinHeader.publics + OFFSET_DIFF_V4;
    _oLinHeader.num_natives = _oWinHeader.num_natives;
    _oLinHeader.natives = _oWinHeader.natives + OFFSET_DIFF_V4;
    _oLinHeader.num_libraries = _oWinHeader.num_libraries;
    _oLinHeader.libraries = _oWinHeader.libraries + OFFSET_DIFF_V4;
    _oLinHeader.num_pubvars = _oWinHeader.num_pubvars;
    _oLinHeader.pubvars = _oWinHeader.pubvars + OFFSET_DIFF_V4;

} // copy_header(amx_win32_header_v4&, amx_linux_header_v4&)
