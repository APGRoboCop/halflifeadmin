/*  Small compiler - code generation (unoptimized "assembler" code)
 *
 *  Copyright (c) ITB CompuPhase, 1997-2001
 *  This file may be freely used. No warranties of any kind.
 */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>     /* for _MAX_PATH */
#include <string.h>
#include "sc.h"

/*  writetrailer
 *  Not much left of this once important function.
 *
 *  Global references: stksize          (referred to only)
 */
SC_FUNC void writetrailer(void)
{
  stgwrite("\nSTKSIZE ");     /* write stack size */
  outval(stksize, TRUE);
}

/*
 *  Start (or restart) the CODE segment.
 *
 *  In fact, the code and data segment specifiers are purely informational;
 *  the "DUMP" instruction itself already specifies that the following values
 *  should go to the data segment. All otherinstructions go to the code
 *  segment.
 *
 *  Global references: curseg
 */
SC_FUNC void begcseg(void)
{
  if (curseg!=sIN_CSEG) {
    stgwrite("\n");
    stgwrite("CODE\t; ");
    outval(code_idx,TRUE);
    curseg=sIN_CSEG;
  } /* endif */
}

/*
 *  Start (or restart) the DATA segment.
 *
 *  Global references: curseg
 */
SC_FUNC void begdseg(void)
{
  if (curseg!=sIN_DSEG) {
    stgwrite("\n");
    stgwrite("DATA\t; ");
    outval(glb_declared,TRUE);
    curseg=sIN_DSEG;
  } /* if */
}

SC_FUNC cell nameincells(char *name)
{
  cell clen=(strlen(name)+sizeof(cell)) & ~(sizeof(cell)-1);
  return clen;
}

SC_FUNC void setfile(char *name,int fileno)
{
  if ((debug & sSYMBOLIC)!=0) {
    begcseg();
    stgwrite("file ");
    outval(fileno,FALSE);
    stgwrite(" ");
    stgwrite(name);
    stgwrite("\n");
    /* calculate code length */
    code_idx+=opcodes(1)+opargs(2)+nameincells(name);
  } /* if */
}

SC_FUNC void setline(int line,int fileno)
{
  if ((debug & sSYMBOLIC)!=0 || (debug & sCHKBOUNDS)!=0) {
    stgwrite("line ");
    outval(line,FALSE);
    stgwrite(" ");
    outval(fileno,FALSE);
    stgwrite("\t; ");
    outval(code_idx,TRUE);
    code_idx+=opcodes(1)+opargs(2);
  } /* if */
}

/*  postlab2
 *
 *  Post a code label (specified as a number) into the code stream, (but not
 *  on a new line).
 */
static void postlab2(int number)
{
  stgwrite((char *)itoh(number));
}

/*  setlabel
 *
 *  Post a code label (specified as a number), on a new line.
 */
SC_FUNC void setlabel(int number)
{
  assert(number>=0);
  stgwrite("l.");
  postlab2(number);
  stgwrite("\t; ");
  outval(code_idx,TRUE);
}

/* Write a token that signifies the end of an expression. This
 * allows several simple optimizations by the peeohole optimizer.
 */
SC_FUNC void endexpr(void)
{
  stgwrite("\t;^;\n");
}

/*  startfunc   - declare a CODE entry point (function start)
 *
 *  Global references: funcstatus  (referred to only)
 */
SC_FUNC void startfunc(void)
{
  stgwrite("\tproc\t; ");
  outval(code_idx,TRUE);
  code_idx+=opcodes(1);
}

/*  endfunc
 *
 *  Declare a CODE ending point (function end)
 */
SC_FUNC void endfunc(void)
{
  stgwrite("\n");       /* skip a line */
}

/*  Define a variable or function
 */
SC_FUNC void defsymbol(char *name,int ident,int vclass,cell offset)
{
  if ((debug & sSYMBOLIC)!=0) {
    begcseg();          /* symbol definition in code segment */
    stgwrite("symbol ");

    stgwrite(name);
    stgwrite(" ");

    outval(offset,FALSE);
    stgwrite(" ");

    outval(vclass,FALSE);
    stgwrite(" ");

    outval(ident,TRUE);

    code_idx+=opcodes(1)+opargs(3)+nameincells(name);  /* class and ident encoded in "flags" */
  } /* if */
}

SC_FUNC void symbolrange(int level,cell size)
{
  if ((debug & sSYMBOLIC)!=0) {
    begcseg();          /* symbol definition in code segment */
    stgwrite("srange ");
    outval(level,FALSE);
    stgwrite(" ");
    outval(size,TRUE);
    code_idx+=opcodes(1)+opargs(2);
  } /* if */
}

/*  rvalue
 *
 *  Generate code to get the value of a symbol into "primary".
 */
SC_FUNC void rvalue(value *lval)
{
  symbol *sym;

  sym=lval->sym;
  if (lval->ident==iARRAYCELL) {
    /* indirect fetch, address already in PRI */
    stgwrite("\tload.i\n");
    code_idx+=opcodes(1);
  } else if (lval->ident==iARRAYCHAR) {
    /* indirect fetch of a character from a pack, address already in PRI */
    stgwrite("\tlodb.i ");
    outval(charbits/8,TRUE);    /* read one or two bytes */
    code_idx+=opcodes(1)+opargs(1);
  } else if (lval->ident==iREFERENCE) {
    /* indirect fetch, but address not yet in PRI */
    assert(sym!=NULL);
    assert(sym->vclass==sLOCAL);    /* global references don't exist in Small */
    if (sym->vclass==sLOCAL)
      stgwrite("\tlref.s.pri ");
    else
      stgwrite("\tlref.pri ");
    outval(sym->addr,TRUE);
    sym->usage|=uREAD;
    code_idx+=opcodes(1)+opargs(1);
  } else {
    /* direct or stack relative fetch */
    assert(sym!=NULL);
    if (sym->vclass==sLOCAL)
      stgwrite("\tload.s.pri ");
    else
      stgwrite("\tload.pri ");
    outval(sym->addr,TRUE);
    sym->usage|=uREAD;
    code_idx+=opcodes(1)+opargs(1);
  } /* if */
}

/*
 *  Get the address of a symbol into the primary register (used for arrays,
 *  and for passing arguments by reference).
 */
SC_FUNC void address(symbol *sym)
{
  assert(sym!=NULL);
  /* the symbol can be a local array, a global array, or an array
   * that is passed by reference.
   */
  if (sym->ident==iREFARRAY || sym->ident==iREFERENCE) {
    /* reference to a variable or to an array; currently this is
     * always a local variable */
    stgwrite("\tload.s.pri ");
  } else {
    /* a local array or local variable */
    if (sym->vclass==sLOCAL)
      stgwrite("\taddr.pri ");
    else
      stgwrite("\tconst.pri ");
  } /* if */
  outval(sym->addr,TRUE);
  sym->usage|=uREAD;
  code_idx+=opcodes(1)+opargs(1);
}

/*  store
 *
 *  Saves the contents of "primary" into a memory cell, either directly
 *  or indirectly (at the address given in the alternate register).
 */
SC_FUNC void store(value *lval)
{
  symbol *sym;

  sym=lval->sym;
  if (lval->ident==iARRAYCELL) {
    /* store at address in ALT */
    stgwrite("\tstor.i\n");
    code_idx+=opcodes(1);
  } else if (lval->ident==iARRAYCHAR) {
    /* store at address in ALT */
    stgwrite("\tstrb.i ");
    outval(charbits/8,TRUE);    /* write one or two bytes */
    code_idx+=opcodes(1)+opargs(1);
  } else if (lval->ident==iREFERENCE) {
    assert(sym!=NULL);
    if (sym->vclass==sLOCAL)
      stgwrite("\tsref.s.pri ");
    else
      stgwrite("\tsref.pri ");
    outval(sym->addr,TRUE);
    code_idx+=opcodes(1)+opargs(1);
  } else {
    assert(sym!=NULL);
    if (sym->vclass==sLOCAL)
      stgwrite("\tstor.s.pri ");
    else
      stgwrite("\tstor.pri ");
    outval(sym->addr,TRUE);
    code_idx+=opcodes(1)+opargs(1);
  } /* if */
}

/* source must in PRI, destination address in ALT. The "size"
 * parameter is in bytes, not cells.
 */
SC_FUNC void memcopy(cell size)
{
  stgwrite("\tmovs ");
  outval(size,TRUE);

  code_idx+=opcodes(1)+opargs(1);
}

/* Address of the source must already have been loaded in PRI
 * "size" is the size in bytes (not cells).
 */
SC_FUNC void copyarray(symbol *sym,cell size)
{
  assert(sym!=NULL);
  /* the symbol can be a local array, a global array, or an array
   * that is passed by reference.
   */
  if (sym->ident==iREFARRAY) {
    /* reference to an array; currently this is always a local variable */
    assert(sym->vclass==sLOCAL);        /* symbol must be stack relative */
    stgwrite("\tload.s.alt ");
  } else {
    /* a local or global array */
    if (sym->vclass==sLOCAL)
      stgwrite("\taddr.alt ");
    else
      stgwrite("\tconst.alt ");
  } /* if */
  outval(sym->addr,TRUE);
  sym->usage|=uWRITTEN;

  code_idx+=opcodes(1)+opargs(1);
  memcopy(size);
}

SC_FUNC void fillarray(symbol *sym,cell size,cell value)
{
  const1(value);    /* lad value in PRI */

  assert(sym!=NULL);
  /* the symbol can be a local array, a global array, or an array
   * that is passed by reference.
   */
  if (sym->ident==iREFARRAY) {
    /* reference to an array; currently this is always a local variable */
    assert(sym->vclass==sLOCAL);        /* symbol must be stack relative */
    stgwrite("\tload.s.alt ");
  } else {
    /* a local or global array */
    if (sym->vclass==sLOCAL)
      stgwrite("\taddr.alt ");
    else
      stgwrite("\tconst.alt ");
  } /* if */
  outval(sym->addr,TRUE);
  sym->usage|=uWRITTEN;

  stgwrite("\tfill ");
  outval(size,TRUE);

  code_idx+=opcodes(2)+opargs(2);
}

/*
 *  Instruction to get an immediate value into the primary register
 */
SC_FUNC void const1(cell val)
{
  if (val==0) {
    stgwrite("\tzero.pri\n");
    code_idx+=opcodes(1);
  } else {
    stgwrite("\tconst.pri ");
    outval(val, TRUE);
    code_idx+=opcodes(1)+opargs(1);
  } /* if */
}

/*
 *  Instruction to get an immediate value into the secondary register
 */
SC_FUNC void const2(cell val)
{
  if (val==0) {
    stgwrite("\tzero.alt\n");
    code_idx+=opcodes(1);
  } else {
    stgwrite("\tconst.alt ");
    outval(val, TRUE);
    code_idx+=opcodes(1)+opargs(1);
  } /* if */
}

/* Copy value in secondary register to the primary register */
SC_FUNC void moveto1(void)
{
  stgwrite("\tmove.pri\n");
  code_idx+=opcodes(1)+opargs(0);
}

/*
 *  Push primary register onto the stack
 */
SC_FUNC void push1(void)
{
  stgwrite("\tpush.pri\n");
  code_idx+=opcodes(1);
}

/*
 *  Push alternate register onto the stack
 */
SC_FUNC void push2(void)
{
  stgwrite("\tpush.alt\n");
  code_idx+=opcodes(1);
}

/*
 *  Push a constant value onto the stack
 */
SC_FUNC void pushval(cell val)
{
  stgwrite("\tpush.c ");
  outval(val, TRUE);
  code_idx+=opcodes(1)+opargs(1);
}

/*
 *  pop stack to the primary register
 */
SC_FUNC void pop1(void)
{
  stgwrite("\tpop.pri\n");
  code_idx+=opcodes(1);
}

/*
 *  pop stack to the secondary register
 */
SC_FUNC void pop2(void)
{
  stgwrite("\tpop.alt\n");
  code_idx+=opcodes(1);
}

/*
 *  swap the top-of-stack with the value in primary register
 */
SC_FUNC void swap1(void)
{
  stgwrite("\tswap.pri\n");
  code_idx+=opcodes(1);
}

/* Switch statements
 * The "switch" statement generates a "case" table using the "CASE" opcode.
 * The case table contains a list of records, each record holds a comparison
 * value and a label to branch to on a match. The very first record is an
 * exception: it holds the size of the table (excluding the first record) and
 * the label to branch to when none of the values in the case table match.
 * The case table is sorted on the comparison value. This allows more advanced
 * abstract machines to sift the case table with a binary search.
 */
SC_FUNC void ffswitch(int label)
{
  stgwrite("\tswitch ");
  outval(label,TRUE);           /* the label is the address of the case table */
  code_idx+=opcodes(1)+opargs(1);
}

SC_FUNC void ffcase(cell value,char *labelname,int newtable)
{
  if (newtable) {
    stgwrite("\tcasetbl\n");
    code_idx+=opcodes(1);
  } /* if */
  stgwrite("\tcase ");
  outval(value,FALSE);
  stgwrite(" ");
  stgwrite(labelname);
  stgwrite("\n");
  code_idx+=opcodes(0)+opargs(2);
}

/*
 *  Call specified function
 */
SC_FUNC void ffcall(symbol *sym,int numargs)
{
  char symname[2*sNAMEMAX+16];

  assert(sym!=NULL);
  assert(sym->ident==iFUNCTN);
  funcdisplayname(symname,sym->name);
  if ((sym->usage & uNATIVE)!=0) {
    /* reserve a SYSREQ id if called for the first time */
    if (sc_status==statWRITE && (sym->usage & uREFER)==0)
      sym->addr=ntv_funcid++;
    stgwrite("\tsysreq.c ");
    outval(sym->addr,FALSE);
    stgwrite("\t; ");
    stgwrite(symname);
    stgwrite("\n\tstack ");
    outval((numargs+1)*sizeof(cell), TRUE);
    code_idx+=opcodes(2)+opargs(2);
  } else {
    /* normal function */
    stgwrite("\tcall ");
    stgwrite(sym->name);
    stgwrite("\t; ");
    stgwrite(symname);
    stgwrite("\n");
    code_idx+=opcodes(1)+opargs(1);
  } /* if */
}

/*  Return from function
 *
 *  Global references: funcstatus  (referred to only)
 */
SC_FUNC void ffret(void)
{
  stgwrite("\tretn\n");
  code_idx+=opcodes(1);
}

SC_FUNC void ffabort(int reason)
{
  stgwrite("\thalt ");
  outval(reason,TRUE);
  code_idx+=opcodes(1)+opargs(1);
}

SC_FUNC void ffbounds(cell size)
{
  if ((debug & sCHKBOUNDS)!=0) {
    stgwrite("\tbounds ");
    outval(size,TRUE);
    code_idx+=opcodes(1)+opargs(1);
  } /* if */
}

/*
 *  Jump to local label number (the number is converted to a name)
 */
SC_FUNC void jumplabel(int number)
{
  stgwrite("\tjump ");
  outval(number,TRUE);
  code_idx+=opcodes(1)+opargs(1);
}

/*
 *   Define storage according to size
 */
SC_FUNC void defstorage(void)
{
  stgwrite("dump ");
}

/*
 *  Inclrement/decrement stack pointer. Note that this routine does
 *  nothing if the delta is zero.
 */
SC_FUNC void modstk(int delta)
{
  if (delta) {
    stgwrite("\tstack ");
    outval(delta, TRUE);
    code_idx+=opcodes(1)+opargs(1);
  } /* if */
}

/* set the stack to a hard offset from the frame */
SC_FUNC void setstk(cell value)
{
  stgwrite("\tlctrl 5\n");      /* get FRM */
  stgwrite("\tadd.c ");
  outval(value, TRUE);          /* add (negative) offset */
  stgwrite("\tsctrl 4\n");      /* store in STK */
  code_idx+=opcodes(3)+opargs(3);
}

SC_FUNC void modheap(int delta)
{
  if (delta) {
    stgwrite("\theap ");
    outval(delta, TRUE);
    code_idx+=opcodes(1)+opargs(1);
  } /* if */
}

SC_FUNC void setheap_pri(void)
{
  stgwrite("\theap ");          /* ALT = HEA++ */
  outval(sizeof(cell), TRUE);
  stgwrite("\tstor.i\n");       /* store PRI (default value) at address ALT */
  stgwrite("\tmove.pri\n");     /* move ALT to PRI: PRI contains the address */
  code_idx+=opcodes(3)+opargs(1);
}

SC_FUNC void setheap(cell value)
{
  stgwrite("\tconst.pri ");     /* load default value in PRI */
  outval(value, TRUE);
  code_idx+=opcodes(1)+opargs(1);
  setheap_pri();
}

/*
 *  Convert a cell number to a "byte" address; i.e. double or quadruple
 *  the primary register.
 */
SC_FUNC void cell2addr(void)
{
  #if defined(BIT16)
    stgwrite("\tshl.c.pri 1\n");
  #else
    stgwrite("\tshl.c.pri 2\n");
  #endif
  code_idx+=opcodes(1)+opargs(1);
}

/*
 *  Double or quadruple the alternate register.
 */
SC_FUNC void cell2addr_alt(void)
{
  #if defined(BIT16)
    stgwrite("\tshl.c.alt 1\n");
  #else
    stgwrite("\tshl.c.alt 2\n");
  #endif
  code_idx+=opcodes(1)+opargs(1);
}

/*
 *  Convert "distance of addresses" to "number of cells" in between.
 *  Or convert a number of packed characters to the number of cells (with
 *  truncation).
 */
SC_FUNC void addr2cell(void)
{
  #if defined(BIT16)
    stgwrite("\tshr.c.pri 1\n");
  #else
    stgwrite("\tshr.c.pri 2\n");
  #endif
  code_idx+=opcodes(1)+opargs(1);
}

/* Convert from character index to byte address. This routine does
 * nothing if a character has the size of a byte.
 */
SC_FUNC void char2addr(void)
{
  if (charbits==16) {
    stgwrite("\tshl.c.pri 1\n");
    code_idx+=opcodes(1)+opargs(1);
  } /* if */
}

/* Align PRI (which should hold a character index) to an address.
 * The first character in a "pack" occupies the highest bits of
 * the cell. This is at the lower memory address on Big Endian
 * computers and on the higher address on Little Endian computers.
 * The ALIGN.pri/alt instructions must solve this machine dependence;
 * that is, on Big Endian computers, ALIGN.pri/alt shuold do nothing
 * and on Little Endian computers they should toggle the address.
 */
SC_FUNC void charalign(void)
{
  stgwrite("\talign.pri ");
  outval(charbits/8,TRUE);
  code_idx+=opcodes(1)+opargs(1);
}

/*
 *  Add a constant to the primary register.
 */
SC_FUNC void addconst(cell value)
{
  stgwrite("\tadd.c ");
  outval(value,TRUE);
  code_idx+=opcodes(1)+opargs(1);
}

/*
 *  signed multiply of primary and secundairy registers (result in primary)
 */
SC_FUNC void os_mult(void)
{
  stgwrite("\tsmul\n");
  code_idx+=opcodes(1);
}

/*
 *  signed divide of alternate register by primary register (quotient in
 *  primary; remainder in alternate)
 */
SC_FUNC void os_div(void)
{
  stgwrite("\tsdiv.alt\n");
  code_idx+=opcodes(1);
}

/*
 *  modulus of (alternate % primary), result in primary (signed)
 */
SC_FUNC void os_mod(void)
{
  stgwrite("\tsdiv.alt\n");
  stgwrite("\tmove.pri\n");     /* move ALT to PRI */
  code_idx+=opcodes(2);
}

/*
 *  Add primary and alternate registers (result in primary).
 */
SC_FUNC void ob_add(void)
{
  stgwrite("\tadd\n");
  code_idx+=opcodes(1);
}

/*
 *  subtract primary register from alternate register (result in primary)
 */
SC_FUNC void ob_sub(void)
{
  stgwrite("\tsub.alt\n");
  code_idx+=opcodes(1);
}

/*
 *  arithmic shift left alternate register the number of bits
 *  given in the primary register (result in primary).
 *  There is no need for a "logical shift left" routine, since
 *  logical shift left is identical to arithmic shift left.
 */
SC_FUNC void ob_sal(void)
{
  stgwrite("\txchg\n");
  stgwrite("\tshl\n");
  code_idx+=opcodes(2);
}

/*
 *  arithmic shift right alternate register the number of bits
 *  given in the primary register (result in primary).
 */
SC_FUNC void os_sar(void)
{
  stgwrite("\txchg\n");
  stgwrite("\tsshr\n");
  code_idx+=opcodes(2);
}

/*
 *  logical (unsigned) shift right of the alternate register by the
 *  number of bits given in the primary register (result in primary).
 */
SC_FUNC void ou_sar(void)
{
  stgwrite("\txchg\n");
  stgwrite("\tshr\n");
  code_idx+=opcodes(2);
}

/*
 *  inclusive "or" of primary and secondary registers (result in primary)
 */
SC_FUNC void ob_or(void)
{
  stgwrite("\tor\n");
  code_idx+=opcodes(1);
}

/*
 *  "exclusive or" of primary and alternate registers (result in primary)
 */
SC_FUNC void ob_xor(void)
{
  stgwrite("\txor\n");
  code_idx+=opcodes(1);
}

/*
 *  "and" of primary and secundairy registers (result in primary)
 */
SC_FUNC void ob_and(void)
{
  stgwrite("\tand\n");
  code_idx+=opcodes(1);
}

/*
 *  test ALT==PRI; result in primary register (1 or 0).
 */
SC_FUNC void ob_eq(void)
{
  stgwrite("\teq\n");
  code_idx+=opcodes(1);
}

/*
 *  test ALT!=PRI
 */
SC_FUNC void ob_ne(void)
{
  stgwrite("\tneq\n");
  code_idx+=opcodes(1);
}

/* The abstract machine defines the relational instructions so that PRI is
 * on the left side and ALT on the right side of the operator. For example,
 * SLESS sets PRI to either 1 or 0 depending on whether the expression
 * "PRI < ALT" is true.
 *
 * The compiler generates comparisons with ALT on the left side of the
 * relational operator and PRI on the right side. The XCHG instruction
 * prefixing the relational operators resets this. We leave it to the
 * peephole optimizer to choose more compact instructions where possible.
 */

/* Relational operator prefix for chained relational expressions. The
 * "suffix" code restores the stack.
 * For chained relational operators, the goal is to keep the comparison
 * result "so far" in PRI and the value of the most recent operand in
 * ALT, ready for a next comparison.
 * The "prefix" instruction pushed the comparison result (PRI) onto the
 * stack and moves the value of ALT into PRI. If there is a next comparison,
 * PRI can now serve as the "left" operand of the relational operator.
 */
SC_FUNC void relop_prefix(void)
{
  stgwrite("\tpush.pri\n");
  stgwrite("\tmove.pri\n");
  code_idx+=opcodes(2);
}

SC_FUNC void relop_suffix(void)
{
  stgwrite("\tswap.alt\n");
  stgwrite("\tand\n");
  stgwrite("\tpop.alt\n");
  code_idx+=opcodes(3);
}

/*
 *  test ALT<PRI (signed)
 */
SC_FUNC void os_lt(void)
{
  stgwrite("\txchg\n");
  stgwrite("\tsless\n");
  code_idx+=opcodes(2);
}

/*
 *  test ALT<=PRI (signed)
 */
SC_FUNC void os_le(void)
{
  stgwrite("\txchg\n");
  stgwrite("\tsleq\n");
  code_idx+=opcodes(2);
}

/*
 *  test ALT>PRI (signed)
 */
SC_FUNC void os_gt(void)
{
  stgwrite("\txchg\n");
  stgwrite("\tsgrtr\n");
  code_idx+=opcodes(2);
}

/*
 *  test ALT>=PRI (signed)
 */
SC_FUNC void os_ge(void)
{
  stgwrite("\txchg\n");
  stgwrite("\tsgeq\n");
  code_idx+=opcodes(2);
}

/*
 *  logical negation of primary register
 */
SC_FUNC void lneg(void)
{
  stgwrite("\tnot\n");
  code_idx+=opcodes(1);
}

/*
 *  two's complement primary register
 */
SC_FUNC void neg(void)
{
  stgwrite("\tneg\n");
  code_idx+=opcodes(1);
}

/*
 *  one's complement of primary register
 */
SC_FUNC void invert(void)
{
  stgwrite("\tinvert\n");
  code_idx+=opcodes(1);
}

/*  increment symbol
 */
SC_FUNC void inc(value *lval)
{
  symbol *sym;

  sym=lval->sym;
  if (lval->ident==iARRAYCELL) {
    /* indirect increment, address already in PRI */
    stgwrite("\tinc.i\n");
    code_idx+=opcodes(1);
  } else if (lval->ident==iARRAYCHAR) {
    /* indirect increment of single character, address already in PRI */
    stgwrite("\tpush.pri\n");
    stgwrite("\tpush.alt\n");
    stgwrite("\tmove.alt\n");   /* copy address */
    stgwrite("\tlodb.i ");      /* read from PRI into PRI */
    outval(charbits/8,TRUE);    /* read one or two bytes */
    stgwrite("\tinc.pri\n");
    stgwrite("\tstrb.i ");      /* write PRI to ALT */
    outval(charbits/8,TRUE);    /* write one or two bytes */
    stgwrite("\tpop.alt\n");
    stgwrite("\tpop.pri\n");
    code_idx+=opcodes(8)+opargs(2);
  } else if (lval->ident==iREFERENCE) {
    assert(sym!=NULL);
    stgwrite("\tpush.pri\n");
    /* load dereferenced value */
    assert(sym->vclass==sLOCAL);    /* global references don't exist in Small */
    if (sym->vclass==sLOCAL)
      stgwrite("\tlref.s.pri ");
    else
      stgwrite("\tlref.pri ");
    outval(sym->addr,TRUE);
    /* increment */
    stgwrite("\tinc.pri\n");
    /* store dereferenced value */
    if (sym->vclass==sLOCAL)
      stgwrite("\tsref.s.pri ");
    else
      stgwrite("\tsref.pri ");
    outval(sym->addr,TRUE);
    stgwrite("\tpop.pri\n");
    code_idx+=opcodes(5)+opargs(2);
  } else {
    /* local or global variable */
    assert(sym!=NULL);
    if (sym->vclass==sLOCAL)
      stgwrite("\tinc.s ");
    else
      stgwrite("\tinc ");
    outval(sym->addr,TRUE);
    code_idx+=opcodes(1)+opargs(1);
  } /* if */
}

/*  decrement symbol
 *
 *  in case of an integer pointer, the symbol must be incremented by 2.
 */
SC_FUNC void dec(value *lval)
{
  symbol *sym;

  sym=lval->sym;
  if (lval->ident==iARRAYCELL) {
    /* indirect decrement, address already in PRI */
    stgwrite("\tdec.i\n");
    code_idx+=opcodes(1);
  } else if (lval->ident==iARRAYCHAR) {
    /* indirect decrement of single character, address already in PRI */
    stgwrite("\tpush.pri\n");
    stgwrite("\tpush.alt\n");
    stgwrite("\tmove.alt\n");   /* copy address */
    stgwrite("\tlodb.i ");      /* read from PRI into PRI */
    outval(charbits/8,TRUE);    /* read one or two bytes */
    stgwrite("\tdec.pri\n");
    stgwrite("\tstrb.i ");      /* write PRI to ALT */
    outval(charbits/8,TRUE);    /* write one or two bytes */
    stgwrite("\tpop.alt\n");
    stgwrite("\tpop.pri\n");
    code_idx+=opcodes(8)+opargs(2);
  } else if (lval->ident==iREFERENCE) {
    assert(sym!=NULL);
    stgwrite("\tpush.pri\n");
    /* load dereferenced value */
    assert(sym->vclass==sLOCAL);    /* global references don't exist in Small */
    if (sym->vclass==sLOCAL)
      stgwrite("\tlref.s.pri ");
    else
      stgwrite("\tlref.pri ");
    outval(sym->addr,TRUE);
    /* decrement */
    stgwrite("\tdec.pri\n");
    /* store dereferenced value */
    if (sym->vclass==sLOCAL)
      stgwrite("\tsref.s.pri ");
    else
      stgwrite("\tsref.pri ");
    outval(sym->addr,TRUE);
    stgwrite("\tpop.pri\n");
    code_idx+=opcodes(5)+opargs(2);
  } else {
    /* local or global variable */
    assert(sym!=NULL);
    if (sym->vclass==sLOCAL)
      stgwrite("\tdec.s ");
    else
      stgwrite("\tdec ");
    outval(sym->addr,TRUE);
    code_idx+=opcodes(1)+opargs(1);
  } /* if */
}

/*
 *  Jumps to "label" if PRI != 0
 */
SC_FUNC void jmp_ne0(int number)
{
  stgwrite("\tjnz ");
  outval(number,TRUE);
  code_idx+=opcodes(1)+opargs(1);
}

/*
 *  Jumps to "label" if PRI == 0
 */
SC_FUNC void jmp_eq0(int number)
{
  stgwrite("\tjzer ");
  outval(number,TRUE);
  code_idx+=opcodes(1)+opargs(1);
}

/* write a value in hexadecimal; optionally adds a newline */
SC_FUNC void outval(cell val,int newline)
{
  stgwrite(itoh(val));
  if (newline)
    stgwrite("\n");
}
