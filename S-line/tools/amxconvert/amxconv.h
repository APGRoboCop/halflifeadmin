#ifndef _AMXCONVERT_H_
#define _AMXCONVERT_H_

/*
 * File: amxconvert.h
 *
 */

#include <string.h>
#include <stdio.h>
#include "amx_l.h"
#include "amxconv_l.h"

// return values of convert_file() function
const int CONV_SUCC = 0;
const int CONV_ERR_OUT = 1;
const int CONV_ERR_MEM = 2;
const int CONV_ERR_FAIL = 3;
const int CONV_ERR_INF = 4;


// Function check_file_type()
//
// Arguments:
//            a char* holding the name of the file
//            a reference to a long into which the size of the file is returned
// Returnvalue:
//              1 if the file is a Windows amx file
//              2 if the file is a Linux amx file
//              0 if the file is no amx file 

int check_file_type( const char*, long& );



// Function convert_file()
//
// Arguments:
//      a char* to the name of the input file
//      a char* to the name of the output file
//      an int which holds the filetype of the amx file
//      a long which holds the size of the input file
// Return value:
//      0 if successful
//      an int error code otherwise

int convert_file( const char*, const char*, const int, const long );



#endif
