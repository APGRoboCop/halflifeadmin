/*  Abstract Machine for the Small compiler
 *
 *  Copyright (c) ITB CompuPhase, 1997-2002
 *  This file may be freely used. No warranties of any kind.
 *
 *  Version: $Id: amx.h,v 1.6 2004/08/02 17:54:13 darope Exp $
 */

#if defined LINUX
  #include "sclinux.h"
#endif

#ifndef __AMX_H
#define __AMX_H

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
      typedef long int          int32_t;	// Conflicting with HLSDK 2015? [APG]RoboCop[CL]
      typedef unsigned long int uint32_t;
      typedef long long          int64_t;
      typedef unsigned long long uint64_t;
    #endif
  #endif
#endif

#if defined TARGET_64BIT
#   define FADDR_SIZE 8
    typedef uint64_t faddr_t;
#   define BIT64
#else
#   define FADDR_SIZE 4
    typedef uint32_t faddr_t;
#endif


#ifdef  __cplusplus
extern  "C" {
#endif

/* calling convention for native functions */
#if !defined AMX_NATIVE_CALL
  #define AMX_NATIVE_CALL
#endif
/* calling convention for all interface functions and callback functions */
#if !defined AMXAPI
  #define AMXAPI
#endif

/* File format version                          Required AMX version
 *   0 (original version)                       0
 *   1 (opcodes JUMP.pri, SWITCH and CASETBL)   1
 *   2 (compressed files)                       2
 *   3 (public variables)                       2
 *   4 (opcodes SWAP.pri/alt and PUSHADDR)      4
 *   5 (tagnames table)                         4
 */
#define MIN_FILE_VERSION  4     /* lowest file format version */
#define CUR_FILE_VERSION  5     /* current AMX version (parallel with file version) */

#if !defined CELL_TYPE
  #define CELL_TYPE
  #if defined(BIT16)
    #define CELL_SIZE 2
    typedef uint16_t  ucell;    /* only for type casting */
    typedef int16_t   cell;
  #elif defined(BIT32)
    #define CELL_SIZE 4
    typedef uint32_t  ucell;
    typedef int32_t   cell;
  #elif defined(BIT64)
    #define CELL_SIZE 8
    typedef uint64_t  ucell;
    typedef int64_t   cell;
  #else
    #define CELL_SIZE 4
    typedef uint32_t  ucell;
    typedef int32_t   cell;
  #endif
#endif

struct __amx;
typedef cell (AMX_NATIVE_CALL *AMX_NATIVE)(struct __amx *amx, cell *params);
typedef int (AMXAPI *AMX_CALLBACK)(struct __amx *amx, cell index,
                                   cell *result, cell *params);
typedef int (AMXAPI *AMX_DEBUG)(struct __amx *amx);
#if !defined FAR
  #define FAR
#endif

#if defined _MSC_VER
  #pragma warning(disable:4103)  /* disable warning message 4103 that complains
                                  * about pragma pack in a header file */
#endif

#if defined LINUX
#  if (defined __amd64__) || (defined TARGET_64BIT)          /* For AMD we use the packed struct which will be default in the future */
#    pragma pack(1)         /* structures must be packed (byte-aligned) */
#  endif
#else
  #pragma pack(push)
  #pragma pack(1)         /* structures must be packed (byte-aligned) */
  #if defined __TURBOC__
    #pragma option -a-    /* "pack" pragma for older Borland compilers */
  #endif
#endif

typedef struct {
  char FAR *name;
  AMX_NATIVE func;
} AMX_NATIVE_INFO;

#define AMX_USERNUM     4
#define sEXPMAX         19
#if (sEXPMAX+1 + FADDR_SIZE) % CELL_SIZE > 0
#  define DEFSIZE_MOD_CELL_PADDING char pad[(sEXPMAX+1 + FADDR_SIZE) % CELL_SIZE];
#else
#  define DEFSIZE_MOD_CELL_PADDING
#endif

typedef struct {
  faddr_t address;
  char name[sEXPMAX+1];
  DEFSIZE_MOD_CELL_PADDING
} AMX_FUNCSTUB;

/* The AMX structure is the internal structure for many functions. Not all
 * fields are valid at all times; many fields are cached in local variables.
 */
typedef struct __amx {
  unsigned char FAR *base;      /* points to the AMX header ("amxhdr") */
  AMX_CALLBACK callback;
  AMX_DEBUG debug;
  /* for external functions a few registers must be accessible from the outside */
  cell cip;                     /* relative to base + amxhdr->cod */
  cell frm;                     /* relative to base + amxhdr->dat */
  cell hea, hlw, stk, stp;      /* all four are relative to base + amxhdr->dat */
  int32_t flags;                    /* current status, see amx_Flags() */
  /* for assertions and debug hook */
  cell curline, curfile;
  int32_t dbgcode;
  cell dbgaddr, dbgparam;
  char FAR *dbgname;
  /* user data */
  int32_t usertags[AMX_USERNUM];
  void FAR *userdata[AMX_USERNUM];
  /* native functions can raise an error */
  int32_t error;
  /* the sleep opcode needs to store the full AMX status */
  cell pri, alt, reset_stk, reset_hea;
  #if defined JIT
    /* support variables for the JIT */
    int reloc_size;     /* required temporary buffer for relocations */
    long code_size;     /* estimated memory footprint of the native code */
  #endif
} AMX;

/* The AMX_HEADER structure is both the memory format as the file format. The
 * structure is used internaly.
 */
typedef struct __amx_header {
  int32_t size;         /* size of the "file" */
  uint16_t magic;       /* signature */
  char    file_version; /* file format version */
  char    amx_version;  /* required version of the AMX */
  int16_t flags;
  int16_t defsize;
  int32_t cod;          /* initial value of COD - code block */
  int32_t dat;          /* initial value of DAT - data block */
  int32_t hea;          /* initial value of HEA - start of the heap */
  int32_t stp;          /* initial value of STP - stack top */
  int32_t cip;          /* initial value of CIP - the instruction pointer */
  int16_t num_publics;  /* number of items in the "public functions" table */
  int32_t publics;      /* offset to the "public functions" table */
  int16_t num_natives;  /* number of items in the "native functions" table */
  int32_t natives;      /* offset to the "native functions" table */
  int16_t num_libraries;/* number of external libraries (dynamically loaded) */
  int32_t libraries;    /* offset to the table of libraries */
  int16_t num_pubvars;  /* number of items in the "public variables" table */
  int32_t pubvars;      /* the "public variables" table */
  int16_t num_tags;     /* number of items in the "public tagnames" table */
  int32_t tags;         /* the "public tagnames" table */
  int16_t reserved;     /* padding */
} AMX_HEADER;
#define AMX_MAGIC       0xf1e0

enum {
  AMX_ERR_NONE,
  /* reserve the first 15 error codes for exit codes of the abstract machine */
  AMX_ERR_EXIT,         /* forced exit */
  AMX_ERR_ASSERT,       /* assertion failed */
  AMX_ERR_STACKERR,     /* stack/heap collision */
  AMX_ERR_BOUNDS,       /* index out of bounds */
  AMX_ERR_MEMACCESS,    /* invalid memory access */
  AMX_ERR_INVINSTR,     /* invalid instruction */
  AMX_ERR_STACKLOW,     /* stack underflow */
  AMX_ERR_HEAPLOW,      /* heap underflow */
  AMX_ERR_CALLBACK,     /* no callback, or invalid callback */
  AMX_ERR_NATIVE,       /* native function failed */
  AMX_ERR_DIVIDE,       /* divide by zero */
  AMX_ERR_SLEEP,        /* go into sleepmode - code can be restarted */

  AMX_ERR_MEMORY = 16,  /* out of memory */
  AMX_ERR_FORMAT,       /* invalid file format */
  AMX_ERR_VERSION,      /* file is for a newer version of the AMX */
  AMX_ERR_NOTFOUND,     /* function not found */
  AMX_ERR_INDEX,        /* invalid index parameter (bad entry point) */
  AMX_ERR_DEBUG,        /* debugger cannot run */
  AMX_ERR_INIT,         /* AMX not initialized (or doubly initialized) */
  AMX_ERR_USERDATA,     /* unable to set user data field (table full) */
  AMX_ERR_INIT_JIT,     /* cannot initialize the JIT */
  AMX_ERR_PARAMS,       /* parameter error */
};

enum {
  DBG_INIT,             /* query/initialize */
  DBG_FILE,             /* file number in curfile, filename in name */
  DBG_LINE,             /* line number in curline, file number in curfile */
  DBG_SYMBOL,           /* address in dbgaddr, class/type in dbgparam */
  DBG_CLRSYM,           /* stack address below which locals should be removed. stack address in stk */
  DBG_CALL,             /* function call, address jumped to in dbgaddr */
  DBG_RETURN,           /* function returns */
  DBG_TERMINATE,        /* program ends, code address in dbgaddr, reason in dbgparam */
  DBG_SRANGE,           /* symbol size and dimensions (arrays); level in dbgaddr (!); length in dbgparam */
};

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


#define AMX_EXEC_MAIN   -1      /* start at program entry point */
#define AMX_EXEC_CONT   -2      /* continue from last address */

#define AMX_USERTAG(a,b,c,d)    ((a) | ((b)<<8) | ((int32_t)(c)<<16) | ((int32_t)(d)<<24))

uint16_t * AMXAPI amx_Align16(uint16_t *v);
uint32_t * AMXAPI amx_Align32(uint32_t *v);
uint64_t * AMXAPI amx_Align64(uint64_t *v);

#ifdef AMXDLL
int AMXAPI amx_Allot(AMX *amx, int cells, cell *amx_addr, cell **phys_addr);
int AMXAPI amx_Callback(AMX *amx, cell index, cell *result, cell *params);
int AMXAPI amx_Debug(AMX *amx); /* default debug procedure, does nothing */
int AMXAPI amx_Exec(AMX *amx, cell *retval, int index, int numparams, ...);
int AMXAPI amx_Execv(AMX *amx, cell *retval, int index, int numparams, cell params[]);
int AMXAPI amx_FindPublic(AMX *amx, char *funcname, int *index);
int AMXAPI amx_FindPubVar(AMX *amx, char *varname, cell *amx_addr);
int AMXAPI amx_FindTagId(AMX *amx, cell tag_id, char *tagname);
int AMXAPI amx_Flags(AMX *amx,uint16_t *flags);
int AMXAPI amx_GetAddr(AMX *amx,cell amx_addr,cell **phys_addr);
int AMXAPI amx_GetPublic(AMX *amx, int index, char *funcname);
int AMXAPI amx_GetPubVar(AMX *amx, int index, char *varname, cell *amx_addr);
int AMXAPI amx_GetString(char *dest,cell *source);
int AMXAPI amx_GetTag(AMX *amx, int index, char *tagname, cell *tag_id);
int AMXAPI amx_GetUserData(AMX *amx, int32_t tag, void **ptr);
int AMXAPI amx_Init(AMX *amx, void *program);
int AMXAPI amx_InitJIT(AMX *amx, void *reloc_table, void *native_code);
int AMXAPI amx_NameLength(AMX *amx, int *length);
AMX_NATIVE_INFO * AMXAPI amx_NativeInfo(char *name,AMX_NATIVE func);
int AMXAPI amx_NumPublics(AMX *amx, int *number);
int AMXAPI amx_NumPubVars(AMX *amx, int *number);
int AMXAPI amx_NumTags(AMX *amx, int *number);
int AMXAPI amx_RaiseError(AMX *amx, int error);
int AMXAPI amx_Register(AMX *amx, AMX_NATIVE_INFO *nativelist, int number);
int AMXAPI amx_Release(AMX *amx, cell amx_addr);
int AMXAPI amx_SetCallback(AMX *amx, AMX_CALLBACK callback);
int AMXAPI amx_SetDebugHook(AMX *amx, AMX_DEBUG debug);
int AMXAPI amx_SetString(cell *dest, const char *source, int pack);
int AMXAPI amx_SetUserData(AMX *amx, int32_t tag, void *ptr);
int AMXAPI amx_StrLen(cell *cstring, int *length);
#endif  /*  AMXDLL  */

#if defined LINUX
#  if (defined __amd64__) || (defined TARGET_64BIT)          /* For AMD we use the packed struct which will be default in the future */
#    pragma pack()    /* reset default packing */
#  endif
#else
  #pragma pack(pop) /* reset previous packing */
#endif

#ifdef  __cplusplus
}
#endif

#endif /* __AMX_H */
