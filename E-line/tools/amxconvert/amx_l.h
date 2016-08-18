/*  Abstract Machine for the Small compiler
 *
 *  Copyright (c) ITB CompuPhase, 1997-2000
 *  This file may be freely used. No warranties of any kind.
 */

#ifndef _AMX_L_H_
#define _AMX_L_H_

#ifdef  __cplusplus
extern  "C" {
#endif

#define MIN_FILE_VERSION  4     /* lowest file format version */
#define MAX_FILE_VERSION  5     /* highest file format version */
#define AMX_VERSION     5       /* current file format version */

#if !defined CELL_TYPE
  #define CELL_TYPE
  #if defined(BIT16)
    typedef unsigned short int ucell;     /* only for type casting */
    typedef short int cell;
  #else
    typedef unsigned long int ucell;
    typedef long int cell;
  #endif
#endif

#if !defined FAR
  #define FAR
#endif

#if defined _MSC_VER_aus
  #pragma warning(disable:4103)  /* disable warning message 4103 that complains
                                  * about pragma pack in a header file */
#endif

  /* definitely a Linux amx header */

  typedef struct __amx_linheader_v4 {
	long size;            /* size of the "file" */
	unsigned short magic; /* signature */
	char file_version;    /* file format version */
	char amx_version;     /* required version of the AMX */
	short flags;
	short defsize;
	long cod;             /* initial value of COD - code block */
	long dat;             /* initial value of DAT - data block */
	long hea;             /* initial value of HEA - start of the heap */
	long stp;             /* initial value of STP - stack top */
	long cip;             /* initial value of CIP - the instruction pointer */
	long num_publics;    /* number of items in the "public functions" table */
	long publics;         /* offset to the "public functions" table */
	long num_natives;    /* number of items in the "native functions" table */
	long natives;         /* offset to the "native functions" table */
	long num_libraries;  /* number of external libraries (dynamically loaded) */
	long libraries;       /* offset to the table of libraries */
	long num_pubvars;    /* number of items in the "public variables" table */
	long pubvars;         /* the "public variables" table */
  } AMX_LINUX_HEADER_V4;


  typedef struct __amx_linheader {
	long size;            /* size of the "file" */
	unsigned short magic; /* signature */
	char file_version;    /* file format version */
	char amx_version;     /* required version of the AMX */
	short flags;
	short defsize;
	long cod;             /* initial value of COD - code block */
	long dat;             /* initial value of DAT - data block */
	long hea;             /* initial value of HEA - start of the heap */
	long stp;             /* initial value of STP - stack top */
	long cip;             /* initial value of CIP - the instruction pointer */
	long num_publics;     /* number of items in the "public functions" table */
	long publics;         /* offset to the "public functions" table */
	long num_natives;     /* number of items in the "native functions" table */
	long natives;         /* offset to the "native functions" table */
	long num_libraries;   /* number of external libraries (dynamically loaded) */
	long libraries;       /* offset to the table of libraries */
	long num_pubvars;     /* number of items in the "public variables" table */
	long pubvars;         /* the "public variables" table */
	long num_tags;        /* number of items in the "public tagnames" table */
	long tags;            /* the "public tagnames" table */
    long reserved;        /* padding */ 
  } AMX_LINUX_HEADER;

  /* definitely a Windows amx header */

#pragma pack(1)
  typedef struct __amx_winheader_v4 {
	long size;            /* size of the "file" */
	unsigned short magic; /* signature */
	char file_version;    /* file format version */
	char amx_version;     /* required version of the AMX */
	short flags;
	short defsize;
	long cod;             /* initial value of COD - code block */
	long dat;             /* initial value of DAT - data block */
	long hea;             /* initial value of HEA - start of the heap */
	long stp;             /* initial value of STP - stack top */
	long cip;             /* initial value of CIP - the instruction pointer */
	short num_publics;    /* number of items in the "public functions" table */
	long publics;         /* offset to the "public functions" table */
	short num_natives;    /* number of items in the "native functions" table */
	long natives;         /* offset to the "native functions" table */
	short num_libraries;  /* number of external libraries (dynamically loaded) */
	long libraries;       /* offset to the table of libraries */
	short num_pubvars;    /* number of items in the "public variables" table */
	long pubvars;         /* the "public variables" table */
  } AMX_WIN32_HEADER_V4;


  typedef struct __amx_winheader {
	long size;            /* size of the "file" */
	unsigned short magic; /* signature */
	char file_version;    /* file format version */
	char amx_version;     /* required version of the AMX */
	short flags;
	short defsize;
	long cod;             /* initial value of COD - code block */
	long dat;             /* initial value of DAT - data block */
	long hea;             /* initial value of HEA - start of the heap */
	long stp;             /* initial value of STP - stack top */
	long cip;             /* initial value of CIP - the instruction pointer */
	short num_publics;    /* number of items in the "public functions" table */
	long publics;         /* offset to the "public functions" table */
	short num_natives;    /* number of items in the "native functions" table */
	long natives;         /* offset to the "native functions" table */
	short num_libraries;  /* number of external libraries (dynamically loaded) */
	long libraries;       /* offset to the table of libraries */
	short num_pubvars;    /* number of items in the "public variables" table */
	long pubvars;         /* the "public variables" table */
	short num_tags;       /* number of items in the "public tagnames" table */
	long tags;            /* the "public tagnames" table */
    short reserved;       /* padding */ 
  } AMX_WIN32_HEADER;
#pragma pack()


#define AMX_MAGIC       0xf1e0

#define AMX_EXEC_MAIN   -1      /* start at program entry point */
#define AMX_EXEC_CONT   -2      /* continue from last address */


#ifdef  __cplusplus
}
#endif

#endif /* _AMX_L_H_ */
