/*  Abstract Machine for the Small compiler
 *
 *  Copyright (c) ITB CompuPhase, 1997-2000
 *  This file may be freely used. No warranties of any kind.
 */

#ifndef _AMX_L_H_
#define _AMX_L_H_

#if !defined __STDC_VERSION__ || __STDC_VERSION__ < 199901L
  /* The ISO C99 defines the int16_t and int_32t types. If the compiler got
   * here, these types are probably undefined.
   */
  #if defined __LCC__ || defined LINUX
    #include <stdint.h>
  #else
    typedef short int           int16_t;
    typedef unsigned short int  uint16_t;
    #if defined SN_TARGET_PS2
      typedef int               int32_t;
      typedef unsigned int      uint32_t;
    #elif defined(_MSC_VER)
      typedef __int32 int32_t;
      typedef unsigned __int32 uint32_t;
      typedef __int64 int64_t;
      typedef unsigned __int64 uint64_t;
      #define INT64_MIN _I64_MIN
      #define INT64_MAX _I64_MAX
    #else
      typedef long int          int32_t;
      typedef unsigned long int uint32_t;
      typedef long long          int64_t;
      typedef unsigned long long uint64_t;
    #endif
  #endif
#endif

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


#define AMX_FLAG_CHAR16   0x01  /* characters are 16-bit */
#define AMX_FLAG_DEBUG    0x02  /* symbolic info. available */
#define AMX_FLAG_COMPACT  0x04  /* compact encoding */
#define AMX_FLAG_BIGENDIAN 0x08 /* big endian encoding */
#define AMX_FLAG_BROWSE 0x4000
#define AMX_FLAG_RELOC  0x8000  /* jump/call addresses relocated */
 
#define AMX_FLAG_CELL_16 0x0100   /* A cell is 16 bit wide */
#define AMX_FLAG_CELL_32 0x0200   /* A cell is 32 bit wide */
#define AMX_FLAG_CELL_64 0x0300   /* A cell is 64 bit wide */
#define AMX_FLAG_CELL_MASK 0x03   /* Bits occupying this flag */
#define AMX_FLAG_CELL_OFF  8      /* Offset of flag in flags member */
/* Get cell flag value from flags member */
#define AMX_FLAG_CELL_VAL(f) (((f)>>AMX_FLAG_CELL_OFF) & AMX_FLAG_CELL_MASK)
/* Calculate cell size from cell flag */
#define AMX_FLAG_CELLSIZE(f) (8 << AMX_FLAG_CELL_VAL((f)))



#if defined _MSC_VER_aus
  #pragma warning(disable:4103)  /* disable warning message 4103 that complains
                                  * about pragma pack in a header file */
#endif

  /* definitely a Linux amx header */

  typedef struct __amx_linheader_v4 {
	int32_t size;            /* size of the "file" */
	uint16_t magic; /* signature */
	char file_version;    /* file format version */
	char amx_version;     /* required version of the AMX */
	int16_t flags;
	int16_t defsize;
	int32_t cod;             /* initial value of COD - code block */
	int32_t dat;             /* initial value of DAT - data block */
	int32_t hea;             /* initial value of HEA - start of the heap */
	int32_t stp;             /* initial value of STP - stack top */
	int32_t cip;             /* initial value of CIP - the instruction pointer */
	int32_t num_publics;    /* number of items in the "public functions" table */
	int32_t publics;         /* offset to the "public functions" table */
	int32_t num_natives;    /* number of items in the "native functions" table */
	int32_t natives;         /* offset to the "native functions" table */
	int32_t num_libraries;  /* number of external libraries (dynamically loaded) */
	int32_t libraries;       /* offset to the table of libraries */
	int32_t num_pubvars;    /* number of items in the "public variables" table */
	int32_t pubvars;         /* the "public variables" table */
  } AMX_LINUX_HEADER_V4;


  typedef struct __amx_linheader {
	int32_t size;            /* size of the "file" */
	uint16_t magic; /* signature */
	char file_version;    /* file format version */
	char amx_version;     /* required version of the AMX */
	int16_t flags;
	int16_t defsize;
	int32_t cod;             /* initial value of COD - code block */
	int32_t dat;             /* initial value of DAT - data block */
	int32_t hea;             /* initial value of HEA - start of the heap */
	int32_t stp;             /* initial value of STP - stack top */
	int32_t cip;             /* initial value of CIP - the instruction pointer */
	int32_t num_publics;     /* number of items in the "public functions" table */
	int32_t publics;         /* offset to the "public functions" table */
	int32_t num_natives;     /* number of items in the "native functions" table */
	int32_t natives;         /* offset to the "native functions" table */
	int32_t num_libraries;   /* number of external libraries (dynamically loaded) */
	int32_t libraries;       /* offset to the table of libraries */
	int32_t num_pubvars;     /* number of items in the "public variables" table */
	int32_t pubvars;         /* the "public variables" table */
	int32_t num_tags;        /* number of items in the "public tagnames" table */
	int32_t tags;            /* the "public tagnames" table */
    int32_t reserved;        /* padding */ 
  } AMX_LINUX_HEADER;

  /* definitely a Windows amx header */

#pragma pack(1)
  typedef struct __amx_winheader_v4 {
	int32_t size;            /* size of the "file" */
	uint16_t magic; /* signature */
	char file_version;    /* file format version */
	char amx_version;     /* required version of the AMX */
	int16_t flags;
	int16_t defsize;
	int32_t cod;             /* initial value of COD - code block */
	int32_t dat;             /* initial value of DAT - data block */
	int32_t hea;             /* initial value of HEA - start of the heap */
	int32_t stp;             /* initial value of STP - stack top */
	int32_t cip;             /* initial value of CIP - the instruction pointer */
	int16_t num_publics;    /* number of items in the "public functions" table */
	int32_t publics;         /* offset to the "public functions" table */
	int16_t num_natives;    /* number of items in the "native functions" table */
	int32_t natives;         /* offset to the "native functions" table */
	int16_t num_libraries;  /* number of external libraries (dynamically loaded) */
	int32_t libraries;       /* offset to the table of libraries */
	int16_t num_pubvars;    /* number of items in the "public variables" table */
	int32_t pubvars;         /* the "public variables" table */
  } AMX_WIN32_HEADER_V4;


  typedef struct __amx_winheader {
	int32_t size;            /* size of the "file" */
	uint16_t magic; /* signature */
	char file_version;    /* file format version */
	char amx_version;     /* required version of the AMX */
	int16_t flags;
	int16_t defsize;
	int32_t cod;             /* initial value of COD - code block */
	int32_t dat;             /* initial value of DAT - data block */
	int32_t hea;             /* initial value of HEA - start of the heap */
	int32_t stp;             /* initial value of STP - stack top */
	int32_t cip;             /* initial value of CIP - the instruction pointer */
	int16_t num_publics;    /* number of items in the "public functions" table */
	int32_t publics;         /* offset to the "public functions" table */
	int16_t num_natives;    /* number of items in the "native functions" table */
	int32_t natives;         /* offset to the "native functions" table */
	int16_t num_libraries;  /* number of external libraries (dynamically loaded) */
	int32_t libraries;       /* offset to the table of libraries */
	int16_t num_pubvars;    /* number of items in the "public variables" table */
	int32_t pubvars;         /* the "public variables" table */
	int16_t num_tags;       /* number of items in the "public tagnames" table */
	int32_t tags;            /* the "public tagnames" table */
    int16_t reserved;       /* padding */ 
  } AMX_WIN32_HEADER;
#pragma pack()


#define AMX_MAGIC       0xf1e0

#define AMX_EXEC_MAIN   (-1)      /* start at program entry point */
#define AMX_EXEC_CONT   (-2)      /* continue from last address */


#ifdef  __cplusplus
}
#endif

#endif /* _AMX_L_H_ */
