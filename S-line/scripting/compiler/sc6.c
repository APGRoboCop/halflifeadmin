/*  Small compiler - Binary code generation (the "assembler")
 *
 *  Copyright (c) ITB CompuPhase, 1997-2002
 *  This file may be freely used. No warranties of any kind.
 *
 *  Version: $Id: sc6.c,v 1.3 2004/07/23 21:28:06 darope Exp $
 */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>     /* for macro max() */
#include <string.h>
#include <ctype.h>
#if defined LINUX
  #include <sclinux.h>
#endif
#include "sc.h"

typedef cell (*OPCODE_PROC)(FILE *fbin,char *params,cell opcode);

typedef struct {
  cell opcode;
  char *name;
  int segment;          /* sIN_CSEG=parse in cseg, sIN_DSEG=parse in dseg */
  OPCODE_PROC func;
} OPCODE;

static cell codeindex;  /* similar to "code_idx" */
static cell *lbltab;    /* label table */
static int writeerror;

/* apparently, strtol() does not work correctly on very large (unsigned)
 * hexadecimal values */
static ucell hex2long(char *s,char **n)
{
  ucell result=0L;
  int digit;

  /* ignore leading whitespace */
  while (*s==' ' || *s=='\t')
    s++;

  /* NB. only works for lower case letters */
  for ( ;; ) {
    assert(!(*s>='A' && *s<='Z'));      /* lower case only */
    if (*s>='0' && *s<='9')
      digit=*s-'0';
    else if (*s>='a' && *s<='f')
      digit=*s-'a' + 10;
    else
      break;
    result=(result<<4) | digit;
    s++;
  } /* for */
  if (n!=NULL)
    *n=s;
  return (ucell)result;
}

#if BYTE_ORDER==BIG_ENDIAN
static int16_t *align16(int16_t *v)
{
  unsigned char *s = (unsigned char *)v;
  unsigned char t;

  /* swap two bytes */
  t=s[0];
  s[0]=s[1];
  s[1]=t;
  return v;
}

static int32_t *align32(int32_t *v)
{
  unsigned char *s = (unsigned char *)v;
  unsigned char t;

  /* swap outer two bytes */
  t=s[0];
  s[0]=s[3];
  s[3]=t;
  /* swap inner two bytes */
  t=s[1];
  s[1]=s[2];
  s[2]=t;
  return v;
}

static int64_t *align64(int64_t *v)
{
  unsigned char *s = (unsigned char *)v;
  unsigned char t;

  /* swap outer two bytes */
  t=s[0];
  s[0]=s[7];
  s[7]=t;
  /* swap second outer two bytes */
  t=s[1];
  s[1]=s[6];
  s[6]=t;
  /* swap second innner two bytes */
  t=s[2];
  s[2]=s[5];
  s[5]=t;
  /* swap inner two bytes */
  t=s[3];
  s[3]=s[4];
  s[4]=t;
  return v;
}
  #if defined BIT16
    #define aligncell(v)  align16(v)
  #elif defined BIT64
    #define aligncell(v)  align64(v)
  #else
    #define aligncell(v)  align32(v)
  #endif
#else
  #define align16(v)    (v)
  #define align32(v)    (v)
  #define align64(v)    (v)
  #define aligncell(v)  (v)
#endif

static char *skipwhitespace(char *str)
{
  while (isspace(*str))
    str++;
  return str;
}

static char *stripcomment(char *str)
{
  char *ptr=strchr(str,';');
  if (ptr!=NULL) {
    *ptr++='\n';        /* terminate the line, but leave the '\n' */
    *ptr='\0';
  } /* if */
  return str;
}

static void write_encoded(FILE *fbin,ucell *c,int num)
{
  while (num-->0) {
    if (sc_compress) {
      ucell p=(ucell)*c;
      unsigned char t[5];     /* a 32-bit cell is encoded in max. 5 bytes (3 bytes for a 16-bit cell) */
      unsigned char code;
      int index;
  	  assert(sizeof(cell)<=4);    /* code must be adjusted for larger cells */
      for (index=0; index<5; index++) {
        t[index]=(unsigned char)(p & 0x7f);     /* store 7 bits */
        p>>=7;
      } /* for */
      /* skip leading zeros */
      while (index>1 && t[index-1]==0 && (t[index-2] & 0x40)==0)
        index--;
      /* skip leading -1s */  // ??? for BIT16, check for index==3 && t[index-1]==0x03
      if (index==5 && t[index-1]==0x0f && (t[index-2] & 0x40)!=0)
        index--;
      while (index>1 && t[index-1]==0x7f && (t[index-2] & 0x40)!=0)
        index--;
      /* write high byte first, write continuation bits */
      assert(index>0);
      while (index-->0) {
        code=(index==0) ? t[index] : (unsigned char)(t[index]|0x80);
        writeerror |= !sc_writebin(fbin,&code,1);
      } /* while */
    } else {
      if ( (sc_lengthbin(fbin) % sizeof(cell)) != 0 ) {
      	printf( "write_encoded(): fbin write position is unaligned by %d: %ld\n", sc_lengthbin(fbin)%sizeof(cell), sc_lengthbin(fbin) );
      }
      assert((sc_lengthbin(fbin) % sizeof(cell)) == 0);
      writeerror |= !sc_writebin(fbin,aligncell(c),sizeof *c);
    } /* if */
    c++;
  } /* while */
}

#if defined __BORLANDC__ || defined __WATCOMC__
  #pragma argsused
#endif
static cell noop(FILE *fbin,char *params,cell opcode)
{
  return 0;
}

#if defined __BORLANDC__ || defined __WATCOMC__
  #pragma argsused
#endif
static cell parm0(FILE *fbin,char *params,cell opcode)
{
  if (fbin!=NULL)
    write_encoded(fbin,(ucell*)&opcode,1);
  return opcodes(1);
}

static cell parm1(FILE *fbin,char *params,cell opcode)
{
  ucell p=hex2long(params,NULL);
  if (fbin!=NULL) {
    write_encoded(fbin,(ucell*)&opcode,1);
    write_encoded(fbin,&p,1);
  } /* if */
  return opcodes(1)+opargs(1);
}

static cell parm2(FILE *fbin,char *params,cell opcode)
{
  ucell p[2];

  p[0]=hex2long(params,&params);
  p[1]=hex2long(params,NULL);
  if (fbin!=NULL) {
    write_encoded(fbin,(ucell*)&opcode,1);
    write_encoded(fbin,p,2);
  } /* if */
  return opcodes(1)+opargs(2);
}

#if defined __BORLANDC__ || defined __WATCOMC__
  #pragma argsused
#endif
static cell do_dump(FILE *fbin,char *params,cell opcode)
{
  ucell p;
  int num = 0;

  while (*params!='\0') {
    p=hex2long(params,&params);
    if (fbin!=NULL)
      write_encoded(fbin,&p,1);
    num++;
    while (isspace(*params))
      params++;
  } /* while */
  return num*sizeof(cell);
}

static cell do_call(FILE *fbin,char *params,cell opcode)
{
  char name[sNAMEMAX+1];
  int i;
  symbol *sym;
  ucell p;

  for (i=0; !isspace(*params); i++,params++) {
    assert(*params!='\0');
    assert(i<sNAMEMAX);
    name[i]=*params;
  } /* for */
  name[i]='\0';

  /* look up the function address; note that the correct file number must
   * already have been set (in order for static globals to be found).
   */
  sym=findglb(name);
  assert(sym!=NULL);
  assert(sym->ident==iFUNCTN || sym->ident==iREFFUNC);
  assert(sym->vclass==sGLOBAL);

  p=sym->addr;
  if (fbin!=NULL) {
    write_encoded(fbin,(ucell*)&opcode,1);
    write_encoded(fbin,&p,1);
  } /* if */
  return opcodes(1)+opargs(1);
}

static cell do_jump(FILE *fbin,char *params,cell opcode)
{
  int i;
  ucell p;

  i=(int)hex2long(params,NULL);
  assert(i>=0 && i<labnum);

  if (fbin!=NULL) {
    assert(lbltab!=NULL);
    p=lbltab[i];
    write_encoded(fbin,(ucell*)&opcode,1);
    write_encoded(fbin,&p,1);
  } /* if */
  return opcodes(1)+opargs(1);
}

static cell do_file(FILE *fbin,char *params,cell opcode)
{
  ucell p, clen;
  int len;

  p=hex2long(params,&params);

  /* remove leading and trailing white space from the filename */
  while (isspace(*params))
    params++;
  len=strlen(params);
  while (len>0 && isspace(params[len-1]))
    len--;
  params[len++]='\0';           /* zero-terminate */
  while (len % sizeof(cell) != 0)
    params[len++]='\0';         /* pad with zeros up to full cell */
  assert(len>0 && len<256);
  clen=len+sizeof(cell);        /* add size of file ordinal */

  if (fbin!=NULL) {
    write_encoded(fbin,(ucell*)&opcode,1);
    write_encoded(fbin,&clen,1);
    write_encoded(fbin,&p,1);
    write_encoded(fbin,(ucell*)params,len/sizeof(cell));
  } /* if */
  return opcodes(1)+opargs(1)+clen;     /* other argument is in clen */
}

static cell do_symbol(FILE *fbin,char *params,cell opcode)
{
  char *endptr;
  ucell offset, clen, flags;

  for (endptr=params; !isspace(*endptr) && endptr!= (void*)0; endptr++)
    /* nothing */;
  assert(*endptr==' ');

  int len = (int)(endptr - params);
  assert(len>0 && len<sNAMEMAX);
  /* first get the other parameters from the line */
  offset=hex2long(endptr,&endptr);
  unsigned char mclass = (unsigned char)hex2long(endptr, &endptr);
  unsigned char type = (unsigned char)hex2long(endptr,NULL);
  flags=type + 256*mclass;
  /* now finish up the name (overwriting the input line) */
  params[len++]='\0';           /* zero-terminate */
  while (len % sizeof(cell) != 0)
    params[len++]='\0';         /* pad with zeros up to full cell */
  clen=len+2*sizeof(cell);      /* add size of symbol address and flags */

  if (fbin!=NULL) {
    write_encoded(fbin,(ucell*)&opcode,1);
    write_encoded(fbin,&clen,1);
    write_encoded(fbin,&offset,1);
    write_encoded(fbin,&flags,1);
    write_encoded(fbin,(ucell*)params,len/sizeof(cell));
  } /* if */

  #if !defined NDEBUG
    /* function should start right after the symbolic information */
    if (fbin==NULL && mclass==0 && type==iFUNCTN)
      assert(offset==codeindex + opcodes(1)+opargs(1)+clen);
  #endif

  return opcodes(1)+opargs(1)+clen;     /* other 2 arguments are in clen */
}

static cell do_switch(FILE *fbin,char *params,cell opcode)
{
	ucell p;

	const int i = (int)hex2long(params,NULL);
  assert(i>=0 && i<labnum);

  if (fbin!=NULL) {
    assert(lbltab!=NULL);
    p=lbltab[i];
    write_encoded(fbin,(ucell*)&opcode,1);
    write_encoded(fbin,&p,1);
  } /* if */
  return opcodes(1)+opargs(1);
}

#if defined __BORLANDC__ || defined __WATCOMC__
  #pragma argsused
#endif
static cell do_case(FILE *fbin,char *params,cell opcode)
{
	ucell p,v;

  v=hex2long(params,&params);
	const int i = (int)hex2long(params,NULL);
  assert(i>=0 && i<labnum);

  if (fbin!=NULL) {
    assert(lbltab!=NULL);
    p=lbltab[i];
    write_encoded(fbin,&v,1);
    write_encoded(fbin,&p,1);
  } /* if */
  return opcodes(0)+opargs(2);
}

#if defined __BORLANDC__ || defined __WATCOMC__
  #pragma argsused
#endif
static cell curfile(FILE *fbin,char *params,cell opcode)
{
  fcurrent=(int)hex2long(params,NULL);
  return 0;
}

static OPCODE opcodelist[] = {
  /* node for "invalid instruction" */
  {  0, NULL,         0,       noop },
  /* opcodes in sorted order */
  { 78, "add",        sIN_CSEG, parm0 },
  { 87, "add.c",      sIN_CSEG, parm1 },
  { 14, "addr.alt",   sIN_CSEG, parm1 },
  { 13, "addr.pri",   sIN_CSEG, parm1 },
  { 30, "align.alt",  sIN_CSEG, parm1 },
  { 29, "align.pri",  sIN_CSEG, parm1 },
  { 81, "and",        sIN_CSEG, parm0 },
  {121, "bounds",     sIN_CSEG, parm1 },
  { 49, "call",       sIN_CSEG, do_call },
  { 50, "call.pri",   sIN_CSEG, parm0 },
  {  0, "case",       sIN_CSEG, do_case },
  {130, "casetbl",    sIN_CSEG, parm0 },
  {118, "cmps",       sIN_CSEG, parm1 },
  {  0, "code",       0,       noop },
  { 12, "const.alt",  sIN_CSEG, parm1 },
  { 11, "const.pri",  sIN_CSEG, parm1 },
  {  0, "curfile",    sIN_CSEG, curfile },
  {  0, "data",       0,       noop },
  {114, "dec",        sIN_CSEG, parm1 },
  {113, "dec.alt",    sIN_CSEG, parm0 },
  {116, "dec.i",      sIN_CSEG, parm0 },
  {112, "dec.pri",    sIN_CSEG, parm0 },
  {115, "dec.s",      sIN_CSEG, parm1 },
  {  0, "dump",       sIN_DSEG, do_dump },
  { 95, "eq",         sIN_CSEG, parm0 },
  {106, "eq.c.alt",   sIN_CSEG, parm1 },
  {105, "eq.c.pri",   sIN_CSEG, parm1 },
  {124, "file",       sIN_CSEG, do_file },
  {119, "fill",       sIN_CSEG, parm1 },
  {100, "geq",        sIN_CSEG, parm0 },
  { 99, "grtr",       sIN_CSEG, parm0 },
  {120, "halt",       sIN_CSEG, parm1 },
  { 45, "heap",       sIN_CSEG, parm1 },
  { 27, "idxaddr",    sIN_CSEG, parm0 },
  { 28, "idxaddr.b",  sIN_CSEG, parm1 },
  {109, "inc",        sIN_CSEG, parm1 },
  {108, "inc.alt",    sIN_CSEG, parm0 },
  {111, "inc.i",      sIN_CSEG, parm0 },
  {107, "inc.pri",    sIN_CSEG, parm0 },
  {110, "inc.s",      sIN_CSEG, parm1 },
  { 86, "invert",     sIN_CSEG, parm0 },
  { 55, "jeq",        sIN_CSEG, do_jump },
  { 60, "jgeq",       sIN_CSEG, do_jump },
  { 59, "jgrtr",      sIN_CSEG, do_jump },
  { 58, "jleq",       sIN_CSEG, do_jump },
  { 57, "jless",      sIN_CSEG, do_jump },
  { 56, "jneq",       sIN_CSEG, do_jump },
  { 54, "jnz",        sIN_CSEG, do_jump },
  { 52, "jrel",       sIN_CSEG, parm1 },  /* always a number */
  { 64, "jsgeq",      sIN_CSEG, do_jump },
  { 63, "jsgrtr",     sIN_CSEG, do_jump },
  { 62, "jsleq",      sIN_CSEG, do_jump },
  { 61, "jsless",     sIN_CSEG, do_jump },
  { 51, "jump",       sIN_CSEG, do_jump },
  {128, "jump.pri",   sIN_CSEG, parm0 },
  { 53, "jzer",       sIN_CSEG, do_jump },
  { 31, "lctrl",      sIN_CSEG, parm1 },
  { 98, "leq",        sIN_CSEG, parm0 },
  { 97, "less",       sIN_CSEG, parm0 },
  { 25, "lidx",       sIN_CSEG, parm0 },
  { 26, "lidx.b",     sIN_CSEG, parm1 },
  {125, "line",       sIN_CSEG, parm2 },
  {  2, "load.alt",   sIN_CSEG, parm1 },
  {  9, "load.i",     sIN_CSEG, parm0 },
  {  1, "load.pri",   sIN_CSEG, parm1 },
  {  4, "load.s.alt", sIN_CSEG, parm1 },
  {  3, "load.s.pri", sIN_CSEG, parm1 },
  { 10, "lodb.i",     sIN_CSEG, parm1 },
  {  6, "lref.alt",   sIN_CSEG, parm1 },
  {  5, "lref.pri",   sIN_CSEG, parm1 },
  {  8, "lref.s.alt", sIN_CSEG, parm1 },
  {  7, "lref.s.pri", sIN_CSEG, parm1 },
  { 34, "move.alt",   sIN_CSEG, parm0 },
  { 33, "move.pri",   sIN_CSEG, parm0 },
  {117, "movs",       sIN_CSEG, parm1 },
  { 85, "neg",        sIN_CSEG, parm0 },
  { 96, "neq",        sIN_CSEG, parm0 },
  { 84, "not",        sIN_CSEG, parm0 },
  { 82, "or",         sIN_CSEG, parm0 },
  { 43, "pop.alt",    sIN_CSEG, parm0 },
  { 42, "pop.pri",    sIN_CSEG, parm0 },
  { 46, "proc",       sIN_CSEG, parm0 },
  { 40, "push",       sIN_CSEG, parm1 },
  { 37, "push.alt",   sIN_CSEG, parm0 },
  { 39, "push.c",     sIN_CSEG, parm1 },
  { 36, "push.pri",   sIN_CSEG, parm0 },
  { 38, "push.r",     sIN_CSEG, parm1 },
  { 41, "push.s",     sIN_CSEG, parm1 },
  {133, "pushaddr",   sIN_CSEG, parm1 },
  { 47, "ret",        sIN_CSEG, parm0 },
  { 48, "retn",       sIN_CSEG, parm0 },
  { 32, "sctrl",      sIN_CSEG, parm1 },
  { 73, "sdiv",       sIN_CSEG, parm0 },
  { 74, "sdiv.alt",   sIN_CSEG, parm0 },
  {104, "sgeq",       sIN_CSEG, parm0 },
  {103, "sgrtr",      sIN_CSEG, parm0 },
  { 65, "shl",        sIN_CSEG, parm0 },
  { 69, "shl.c.alt",  sIN_CSEG, parm1 },
  { 68, "shl.c.pri",  sIN_CSEG, parm1 },
  { 66, "shr",        sIN_CSEG, parm0 },
  { 71, "shr.c.alt",  sIN_CSEG, parm1 },
  { 70, "shr.c.pri",  sIN_CSEG, parm1 },
  { 94, "sign.alt",   sIN_CSEG, parm0 },
  { 93, "sign.pri",   sIN_CSEG, parm0 },
  {102, "sleq",       sIN_CSEG, parm0 },
  {101, "sless",      sIN_CSEG, parm0 },
  { 72, "smul",       sIN_CSEG, parm0 },
  { 88, "smul.c",     sIN_CSEG, parm1 },
  {127, "srange",     sIN_CSEG, parm2 }, /* version 1 */
  { 20, "sref.alt",   sIN_CSEG, parm1 },
  { 19, "sref.pri",   sIN_CSEG, parm1 },
  { 22, "sref.s.alt", sIN_CSEG, parm1 },
  { 21, "sref.s.pri", sIN_CSEG, parm1 },
  { 67, "sshr",       sIN_CSEG, parm0 },
  { 44, "stack",      sIN_CSEG, parm1 },
  {  0, "stksize",    0,       noop },
  { 16, "stor.alt",   sIN_CSEG, parm1 },
  { 23, "stor.i",     sIN_CSEG, parm0 },
  { 15, "stor.pri",   sIN_CSEG, parm1 },
  { 18, "stor.s.alt", sIN_CSEG, parm1 },
  { 17, "stor.s.pri", sIN_CSEG, parm1 },
  { 24, "strb.i",     sIN_CSEG, parm1 },
  { 79, "sub",        sIN_CSEG, parm0 },
  { 80, "sub.alt",    sIN_CSEG, parm0 },
  {132, "swap.alt",   sIN_CSEG, parm0 },
  {131, "swap.pri",   sIN_CSEG, parm0 },
  {129, "switch",     sIN_CSEG, do_switch },
  {126, "symbol",     sIN_CSEG, do_symbol },
  {123, "sysreq.c",   sIN_CSEG, parm1 },
  {122, "sysreq.pri", sIN_CSEG, parm0 },
  { 76, "udiv",       sIN_CSEG, parm0 },
  { 77, "udiv.alt",   sIN_CSEG, parm0 },
  { 75, "umul",       sIN_CSEG, parm0 },
  { 35, "xchg",       sIN_CSEG, parm0 },
  { 83, "xor",        sIN_CSEG, parm0 },
  { 91, "zero",       sIN_CSEG, parm1 },
  { 90, "zero.alt",   sIN_CSEG, parm0 },
  { 89, "zero.pri",   sIN_CSEG, parm0 },
  { 92, "zero.s",     sIN_CSEG, parm1 },
};

#define MAX_INSTR_LEN   30
static int findopcode(char *instr,int maxlen)
{
	char str[MAX_INSTR_LEN];

  if (maxlen>=MAX_INSTR_LEN)
    return 0;
  strncpy(str,instr,maxlen);
  str[maxlen]='\0';     /* make sure the string is zero terminated */
  /* look up the instruction with a binary search
   * the assembler is case insensitive to instructions (but case sensitive
   * to symbols)
   */
  int low = 1;                /* entry 0 is reserved (for "not found") */
  int high = (sizeof opcodelist / sizeof opcodelist[0]) - 1;
  while (low<high) {
	  const int mid = (low + high) / 2;
    assert(opcodelist[mid].name!=NULL);
	  const int cmp = stricmp(str, opcodelist[mid].name);
    if (cmp>0)
      low=mid+1;
    else
      high=mid;
  } /* while */

  assert(low==high);
  if (stricmp(str,opcodelist[low].name)==0)
    return low;         /* found */
  return 0;             /* not found, return special index */
}

SC_FUNC void assemble(FILE *fout,FILE *fin)
{
  AMX_HEADER hdr;
  AMX_FUNCSTUB func;
  int numpublics,numnatives,numlibraries,numpubvars,numtags;
  char line[300],*instr,*params;
  int i,pass,len;
  symbol *sym;
  constval *constptr;
  cell mainaddr;

  writeerror=FALSE;
  numpublics=0;
  numnatives=0;
  numpubvars=0;
  mainaddr=-1;
  /* count number of public and native functions and public variables */
  for (sym=glbtab.next; sym!=NULL; sym=sym->next) {
    if (sym->ident==iFUNCTN) {
      if ((sym->usage & uNATIVE)!=0 && (sym->usage & uREAD)!=0 && sym->addr>=0)
        numnatives++;
      if ((sym->usage & uPUBLIC)!=0 && (sym->usage & uDEFINE)!=0)
        numpublics++;
      if (strcmp(sym->name,uMAINFUNC)==0) {
        assert(sym->vclass==sGLOBAL);
        mainaddr=sym->addr;
      } /* if */
    } else if (sym->ident==iVARIABLE) {
      if ((sym->usage & uPUBLIC)!=0)
        numpubvars++;
    } /* if */
  } /* for */
  assert(numnatives==ntv_funcid);

  /* count number of libraries */
  numlibraries=0;
  for (constptr=libname_tab.next; constptr!=NULL; constptr=constptr->next) {
    if (constptr->value>0) {
      assert(strlen(constptr->name)>0);
      numlibraries++;
    } /* if */
  } /* for */

  /* count number of public tags */
  numtags=0;
  for (constptr=tagname_tab.next; constptr!=NULL; constptr=constptr->next) {
    if ((constptr->value & PUBLICTAG)!=0) {
      assert(strlen(constptr->name)>0);
      numtags++;
    } /* if */
  } /* for */

  /* write the abstract machine header */
  assert(sEXPMAX<=sNAMEMAX);
  memset(&hdr, 0, sizeof hdr);
  hdr.magic=(uint16_t)0xF1E0;
  hdr.file_version=CUR_FILE_VERSION;
  hdr.amx_version=MIN_FILE_VERSION;
  hdr.flags=(int16_t)(debug & sSYMBOLIC);
  if (charbits==16)
    hdr.flags|=AMX_FLAG_CHAR16;
  if (sc_compress)
    hdr.flags|=AMX_FLAG_COMPACT;
  #if BYTE_ORDER==BIG_ENDIAN
    hdr.flags|=AMX_FLAG_BIGENDIAN;
  #endif

  #if defined BIT16
  hdr.flags|=AMX_FLAG_CELL_16;
  #elif defined BIT64
  hdr.flags|=AMX_FLAG_CELL_64;
  #else
  hdr.flags|=AMX_FLAG_CELL_32;
  #endif

  hdr.defsize=sizeof(AMX_FUNCSTUB);
//printf("hdr.defsize=%d(0x%x), sizeof(cell) = %d, modulo=%d\n", hdr.defsize, hdr.defsize, sizeof(cell), (hdr.defsize % sizeof(cell)));
  assert((hdr.defsize % sizeof(cell))==0);
  hdr.num_publics=(int16_t)numpublics;
  hdr.num_natives=(int16_t)numnatives;
  hdr.num_libraries=(int16_t)numlibraries;
  hdr.num_pubvars=(int16_t)numpubvars;
  hdr.num_tags=(int16_t)numtags;
  hdr.publics=sizeof hdr; /* public table starts right after the header */
  hdr.natives=hdr.publics + numpublics*sizeof(AMX_FUNCSTUB);
  hdr.libraries=hdr.natives + numnatives*sizeof(AMX_FUNCSTUB);
  hdr.pubvars=hdr.libraries + numlibraries*sizeof(AMX_FUNCSTUB);
  hdr.tags=hdr.pubvars + numpubvars*sizeof(AMX_FUNCSTUB);
  hdr.cod=hdr.tags + numtags*sizeof(AMX_FUNCSTUB);
  hdr.dat=hdr.cod + code_idx;
  hdr.hea=hdr.dat + glb_declared*sizeof(cell);
  hdr.stp=hdr.hea + stksize*sizeof(cell);
  hdr.cip=mainaddr;
  hdr.size=hdr.hea; /* preset, this is incorrect in case of compressed output */
  #if BYTE_ORDER==BIG_ENDIAN
    align32(&hdr.size);
    align16(&hdr.magic);
    align16(&hdr.flags);
    align16(&hdr.defsize);
    align16(&hdr.num_publics);
    align16(&hdr.num_natives);
    align16(&hdr.num_libraries);
    align16(&hdr.num_pubvars);
    align16(&hdr.num_tags);
    align32(&hdr.publics);
    align32(&hdr.natives);
    align32(&hdr.libraries);
    align32(&hdr.pubvars);
    align32(&hdr.tags);
    align32(&hdr.cod);
    align32(&hdr.dat);
    align32(&hdr.hea);
    align32(&hdr.stp);
    align32(&hdr.cip);
  #endif
  sc_writebin(fout,&hdr,sizeof hdr);

  /* write the public functions table */
  for (sym=glbtab.next; sym!=NULL; sym=sym->next) {
    if (sym->ident==iFUNCTN
        && (sym->usage & uPUBLIC)!=0 && (sym->usage & uDEFINE)!=0)
    {
      memset(&func, 0, sizeof func);
      len=strlen(sym->name);
      if (len>sEXPMAX) {
        len=sEXPMAX;
        error(220,sym->name,sEXPMAX);
      } /* if */
      strncpy(func.name,sym->name,len);
      assert(sym->vclass==sGLOBAL);
      func.address=sym->addr;
      #if BYTE_ORDER==BIG_ENDIAN
        align32(&func.address);
      #endif
      sc_writebin(fout,&func,sizeof func);
    } /* if */
  } /* for */

  /* write the natives table */
  /* The native functions must be written in sorted order. (They are
   * sorted on their "id", not on their name). A nested loop to find
   * each successive function would be an O(n^2) operation. But we
   * do not really need to sort, because the native function id's
   * are sequential and there are no duplicates. So we first walk
   * through the complete symbol list and store a pointer to every
   * native function of interest in a temporary table, where its id
   * serves as the index in the table. Now we can walk the table and
   * have all native functions in sorted order.
   */
  if (numnatives>0) {
	  symbol **nativelist;
	  nativelist=(symbol **)malloc(numnatives*sizeof(symbol *));
    if (nativelist==NULL)
      error(103);               /* insufficient memory */
    #if !defined NDEBUG
      memset(nativelist,0,numnatives*sizeof(symbol *)); /* for NULL checking */
    #endif
    for (sym=glbtab.next; sym!=NULL; sym=sym->next) {
      if (sym->ident==iFUNCTN && (sym->usage & uNATIVE)!=0 && (sym->usage & uREAD)!=0 && sym->addr>=0) {
        assert(sym->addr < numnatives);
        nativelist[(int)sym->addr]=sym;
      } /* if */
    } /* for */
    for (i=0; i<numnatives; i++) {
      sym=nativelist[i];
      assert(sym!=NULL);
      memset(&func, 0, sizeof func);
      if (!lookup_alias(func.name,sym->name)) {
        len=strlen(sym->name);
        if (len>sEXPMAX) {
          len=sEXPMAX;
          error(220,sym->name,sEXPMAX);
        } /* if */
        strncpy(func.name,sym->name,len);
      } /* if */
      assert(sym->vclass==sGLOBAL);
      func.address=0;
      #if BYTE_ORDER==BIG_ENDIAN
        align32(&func.address);
      #endif
      sc_writebin(fout,&func,sizeof func);
    } /* for */
    free(nativelist);
  } /* if */

  /* write the libraries table */
  for (constptr=libname_tab.next; constptr!=NULL; constptr=constptr->next) {
    if (constptr->value>0) {
      assert(strlen(constptr->name)>0);
      memset(&func, 0, sizeof func);
      len=strlen(constptr->name);
      if (len>sEXPMAX) {
        len=sEXPMAX;
        error(220,constptr->name,sEXPMAX);
      } /* if */
      strncpy(func.name,constptr->name,len);
      func.address=constptr->value;
      #if BYTE_ORDER==BIG_ENDIAN
        align32(&func.address);
      #endif
      sc_writebin(fout,&func,sizeof func);
    } /* if */
  } /* for */

  /* write the public variables table */
  for (sym=glbtab.next; sym!=NULL; sym=sym->next) {
    if (sym->ident==iVARIABLE && (sym->usage & uPUBLIC)!=0) {
      assert((sym->usage & uDEFINE)!=0);
      memset(&func, 0, sizeof func);
      len=strlen(sym->name);
      if (len>sEXPMAX) {
        len=sEXPMAX;
        error(220,sym->name,sEXPMAX);
      } /* if */
      strncpy(func.name,sym->name,len);
      assert(sym->vclass==sGLOBAL);
      func.address=sym->addr;
      #if BYTE_ORDER==BIG_ENDIAN
        align32(&func.address);
      #endif
      sc_writebin(fout,&func,sizeof func);
    } /* if */
  } /* for */

  /* write the public tagnames table */
  for (constptr=tagname_tab.next; constptr!=NULL; constptr=constptr->next) {
    if ((constptr->value & PUBLICTAG)!=0) {
      assert(strlen(constptr->name)>0);
      memset(&func, 0, sizeof func);
      len=strlen(constptr->name);
      if (len>sEXPMAX) {
        len=sEXPMAX;
        error(220,constptr->name,sEXPMAX);
      } /* if */
      strncpy(func.name,constptr->name,len);
      func.address=constptr->value & TAGMASK;
      #if BYTE_ORDER==BIG_ENDIAN
        align32(&func.address);
      #endif
      sc_writebin(fout,&func,sizeof func);
    } /* if */
  } /* for */

  /* First pass: relocate all labels */
  /* This pass is necessary because the code addresses of labels is only known
   * after the peephole optimization flag. Labels can occur inside expressions
   * (e.g. the conditional operator), which are optimized.
   */
  lbltab=NULL;
  if (labnum>0) {
    /* only very short programs have zero labels; no first pass is needed
     * if there are no labels */
    lbltab=(cell *)malloc(labnum*sizeof(cell));
    if (lbltab==NULL)
      error(103);               /* insufficient memory */
    codeindex=0;
    sc_resetasm(fin);
    while (sc_readasm(fin,line,sizeof line)!=NULL) {
      stripcomment(line);
      instr=skipwhitespace(line);
      /* ignore empty lines */
      if (*instr=='\0')
        continue;
      if (tolower(*instr)=='l' && *(instr+1)=='.') {
        int lindex=(int)hex2long(instr+2,NULL);
        assert(lindex<labnum);
        lbltab[lindex]=codeindex;
      } else {
        /* get to the end of the instruction (make use of the '\n' that fgets()
         * added at the end of the line; this way we will *always* drop on a
         * whitespace character) */
        for (params=instr; *params!='\0' && !isspace(*params); params++)
          /* nothing */;
        assert(params>instr);
        i=findopcode(instr,(int)(params-instr));
        if (opcodelist[i].name==NULL) {
          *params='\0';
          error(104,instr);     /* invalid assembler instruction */
        } /* if */
        if (opcodelist[i].segment==sIN_CSEG)
          codeindex+=opcodelist[i].func(NULL,skipwhitespace(params),opcodelist[i].opcode);
      } /* if */
    } /* while */
  } /* if */

  /* Second pass (actually 2 more passes, one for all code and one for all data) */
  for (pass=sIN_CSEG; pass<=sIN_DSEG; pass++) {
    sc_resetasm(fin);
    while (sc_readasm(fin,line,sizeof line)!=NULL) {
      stripcomment(line);
      instr=skipwhitespace(line);
      /* ignore empty lines and labels (labels have a special syntax, so these
       * must be parsed separately) */
      if (*instr=='\0' || tolower(*instr)=='l' && *(instr+1)=='.')
        continue;
      /* get to the end of the instruction (make use of the '\n' that fgets()
       * added at the end of the line; this way we will *always* drop on a
       * whitespace character) */
      for (params=instr; *params!='\0' && !isspace(*params); params++)
        /* nothing */;
      assert(params>instr);
      i=findopcode(instr,(int)(params-instr));
      assert(opcodelist[i].name!=NULL);
      if (opcodelist[i].segment==pass)
        opcodelist[i].func(fout,skipwhitespace(params),opcodelist[i].opcode);
    } /* while */
  } /* for */

  if (lbltab!=NULL) {
    free(lbltab);
    #if !defined NDEBUG
      lbltab=NULL;
    #endif
  } /* if */

  if (writeerror)
    error(101,"disk full");

  /* adjust the header */
  if (sc_compress) {
    hdr.size=sc_lengthbin(fout);
    #if BYTE_ORDER==BIG_ENDIAN
      align32(&hdr.size);
    #endif
    sc_resetbin(fout);  /* "size" is the very first field */
    sc_writebin(fout,&hdr.size,sizeof hdr.size);
  } /* if */
}
