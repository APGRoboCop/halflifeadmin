#ifndef _AMXCONVERT_L_H_
#define _AMXCONVERT_L_H_

/*
 * File: amxconvert_l.h
 *
 */

#include <string.h>
#include <stdio.h>
#include "amx_l.h"

// return values of check_header_type() function
const int WIN32_AMX = 1;
const int LINUX_AMX = 2;
const int INVAL_AMX = 0;
const int INVAL_AMX_HDR = 4;
const int INVAL_AMX_VER = 5;
const int INVAL_ERR = 3;





// Function check_header_type()
//
// Arguments:
//            a reference to a Linux amx header which is to be tested
//            a reference to an integer into which the change of the
//              file size is returned, was this file converted into
//              the opposite format.
// Return value:
//              WIN32_AMX if the file is a Windows amx file
//              LINUX_AMX if the file is a Linux amx file
//              INVAL_AMX_HDR if the file has no valid amx header
//              INVAL_AMX_VER if the file has a wrong amx version

int check_header_type( const AMX_LINUX_HEADER&, int&  );



// Function check_header_type()
//
// Arguments:
//            a reference to a Linux amx header which is to be tested
// Return value:
//              WIN32_AMX if the file is a Windows amx file
//              LINUX_AMX if the file is a Linux amx file
//              INVAL_AMX_HDR if the file has no valid amx header
//              INVAL_AMX_VER if the file has a wrong amx version

inline int check_header_type( const AMX_LINUX_HEADER& _oLinHeader ) {

	int nil;
	return check_header_type( _oLinHeader, nil );
}  // check_header_type(amx_linux_header&, int&)




// Function convert_header()
// Convert a Linux header to a Windows header
//
// Arguments:
//            a char pointer to a memory copy of the file
//            a reference to a Windows amx header which will hold the result
// Return value:
//            a char pointer into the memory copy of the file
//                           pointing to the start of the code

char* convert_header( char*, AMX_WIN32_HEADER& );


// Function convert_header()
// Convert a Linux v4 header to a Windows v4 header
//
// Arguments:
//            a char pointer to a memory copy of the file
//            a reference to a Windows amx v4 header which will hold the result
// Return value:
//            a char pointer into the memory copy of the file
//                           pointing to the start of the code

char* convert_header( char*, AMX_WIN32_HEADER_V4& );



// Function convert_header()
// Convert a Windows header to a Linux header
//
// Arguments:
//            a char pointer to a memory copy of the file
//            a reference to a Linux amx header which will hold the result
// Return value:
//            a char pointer into the memory copy of the file
//                           pointing to the start of the code

char* convert_header( char*, AMX_LINUX_HEADER& );


// Function convert_header()
// Convert a Windows v4 header to a Linux v4 header
//
// Arguments:
//            a char pointer to a memory copy of the file
//            a reference to a Linux amx v4 header which will hold the result
// Return value:
//            a char pointer into the memory copy of the file
//                           pointing to the start of the code

char* convert_header( char*, AMX_LINUX_HEADER_V4& );



// Function copy_header()
// Copy a Windows header to a Linux header
// Necessary since I don't want to alter the standard __amx struct too much
// 
// Arguments:
//            a refrenece to a Linux header into which the header is copied
//            a reference to a Windows header which gets copied

void copy_header( AMX_LINUX_HEADER&, AMX_WIN32_HEADER& );


// Function copy_header()
// Copy a Windows v4 header to a Linux v4 header
// Necessary since I don't want to alter the standard __amx struct too much
// 
// Arguments:
//            a refrenece to a Linux v4 header into which the header is copied
//            a reference to a Windows v4 header which gets copied

void copy_header( AMX_LINUX_HEADER_V4&, AMX_WIN32_HEADER_V4& );


// Function copy_header()
// Copy a Linux header to a Windows header
// Necessary since I don't want to alter the standard __amx struct too much
// 
// Arguments:
//            a refrenece to a Winodws header into which the header is copied
//            a reference to a Linux header which gets copied

void copy_header( AMX_WIN32_HEADER&, AMX_LINUX_HEADER& );


// Function copy_header()
// Copy a Linux v4 header to a Windows v4 header
// Necessary since I don't want to alter the standard __amx struct too much
// 
// Arguments:
//            a refrenece to a Winodws v4 header into which the header is copied
//            a reference to a Linux v4 header which gets copied

void copy_header( AMX_WIN32_HEADER_V4&, AMX_LINUX_HEADER_V4& );



#endif
