/*  Small compiler - Recursive descend expresion parser
 *
 *  Copyright (c) ITB CompuPhase, 1997-2001
 *  This file may be freely used. No warranties of any kind.
 */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>     /* for _MAX_PATH */
#include <string.h>
#include "sc.h"

static int skim(int *opstr,void (*testfunc)(int),int dropval,int endval,
                int (*hier)(value*),value *lval);
static void dropout(int lvalue,void (*testfunc)(int val),int exit1,value *lval);
static int plnge(int *opstr,int opoff,int (*hier)(value *lval),value *lval,char *forcetag);
static int plnge1(int (*hier)(value *lval),value *lval);
static void plnge2(void (*oper)(void),
                   int (*hier)(value *lval),
                   value *lval1,value *lval2);
static cell calc(cell left,void (*oper)(),cell right,char *boolresult);
static int hier13(value *lval);
static int hier12(value *lval);
static int hier11(value *lval);
static int hier10(value *lval);
static int hier9(value *lval);
static int hier8(value *lval);
static int hier7(value *lval);
static int hier6(value *lval);
static int hier5(value *lval);
static int hier4(value *lval);
static int hier3(value *lval);
static int hier2(value *lval);
static int hier1(value *lval1);
static int primary(value *lval);
static void callfunction(symbol *sym);
static int dbltest(void (*oper)(),value *lval1,value *lval2);
static int commutative(void (*oper)());
static int constant(value *lval);

/* Function addresses of binary operators for signed operations */
static void (*op1[17])(void) = {
  os_mult,os_div,os_mod,        /* hier3, index 0 */
  ob_add,ob_sub,                /* hier4, index 3 */
  ob_sal,os_sar,ou_sar,         /* hier5, index 5 */
  ob_and,                       /* hier6, index 8 */
  ob_xor,                       /* hier7, index 9 */
  ob_or,                        /* hier8, index 10 */
  os_le,os_ge,os_lt,os_gt,      /* hier9, index 11 */
  ob_eq,ob_ne,                  /* hier10, index 15 */
};
/* These two functions are defined because the functions inc() and dec() in
 * SC4.C have a different prototype than the other code generation functions.
 * The arrays for user-defined use the function pointers for identifying what
 * kind of operation is requested; these functions must all have the same
 * prototype. As inc() and dec() are special cases already, it is simplest
 * to add two "do-nothing" functions.
 */
static void user_inc(void) {}
static void user_dec(void) {}

/*
 *  Searches for a binary operator a list of operators. The list is stored in
 *  the array "list". The last entry in the list should be set to 0.
 *
 *  The index of an operator in "list" (if found) is returned in "opidx". If
 *  no operator is found, nextop() returns 0.
 */
static int nextop(int *opidx,int *list)
{
  *opidx=0;
  while (*list){
    if (matchtoken(*list)){
      return TRUE;      /* found! */
    } else {
      list+=1;
      *opidx+=1;
    } /* if */
  } /* while */
  return FALSE;         /* entire list scanned, nothing found */
}

SC_FUNC int check_userop(void (*oper)(void),int tag1,int tag2,int numparam,
                         value *lval,int *resulttag)
{
static char *binoperstr[] = { "*", "/", "%", "+", "-", "", "", "",
                              "", "", "", "<=", ">=", "<", ">", "==", "!=" };
static char *unoperstr[] = { "!", "-", "++", "--" };
static void (*unopers[])(void) = { lneg, neg, user_inc, user_dec };
  char opername[4] = "", symbolname[sNAMEMAX];
  int i,swapparams;
  symbol *sym;

  /* find the name with the operator */
  assert( (sizeof binoperstr / sizeof binoperstr[0]) == (sizeof op1 / sizeof op1[0]) );
  for (i=0; i<sizeof op1 / sizeof op1[0]; i++) {
    if (oper==op1[i]) {
      assert(numparam==2);
      strcpy(opername,binoperstr[i]);
      break;
    } /* if */
  } /* for */
  /* if not found, try a select group of unary operators */
  assert( (sizeof unoperstr / sizeof unoperstr[0]) == (sizeof unopers / sizeof unopers[0]) );
  if (opername[0]=='\0') {
    for (i=0; i<sizeof unopers / sizeof unopers[0]; i++) {
      if (oper==unopers[i]) {
        assert(numparam==1);
        strcpy(opername,unoperstr[i]);
        break;
      } /* if */
    } /* for */
  } /* if */
  /* if still not found, quit */
  if (opername[0]=='\0')
    return FALSE;

  /* create a symbol name from the tags and the operator name */
  assert(numparam==1 || numparam==2);
  switch (numparam) {
  case 1:
    sprintf(symbolname,"%s%d",opername,tag1);
    break;
  case 2:
    sprintf(symbolname,"%d%s%d",tag1,opername,tag2);
    break;
  } /* switch */
  swapparams=FALSE;
  sym=findglb(symbolname);
  if (sym==NULL /*|| (sym->usage & uDEFINE)==0*/) {  // ??? should not check uDEFINE; first pass clears these bits
    /* check for commutative operators */
    if (!commutative(oper))
      return FALSE;             /* not commutative, cannot swap operands */
    /* if arrived here, the operator is commutative, swap tags and try again */
    assert(numparam==2);        /* commutative operator must be a binary operator */
    sprintf(symbolname,"%d%s%d",tag2,opername,tag1);
    swapparams=TRUE;
    sym=findglb(symbolname);
    if (sym==NULL /*|| (sym->usage & uDEFINE)==0*/)
      return FALSE;
  } /* if */

  /* we don't want to use the redefined operator in the function that
   * redefines the operator itself, otherwise the snippet below gives
   * an unexpected recursion:
   *    fixed:operator+(fixed:a, fixed:b)
   *        return a + b
   */
  if (sym==curfunc)
    return FALSE;

  /* for a few functions, the symbol must first be loaded (and stored back
   * afterwards)
   */
  if (lval!=NULL) {
    if (lval->ident==iARRAYCELL || lval->ident==iARRAYCHAR)
      push1();                  /* save current address in PRI */
    rvalue(lval);               /* get the symbol's value in PRI */
  } /* if */

  /* push parameters, call the function */
  switch (numparam) {
  case 1:
    push1();
    break;
  case 2:
    /* note that 1) a function expects that the parameters are pushed
     * in reversed order, and 2) the left operand is in the secondary register
     * and the right operand is in the primary register */
    if (swapparams) {
      push2();
      push1();
    } else {
      push1();
      push2();
    } /* if */
    break;
  } /* switch */
  pushval((cell)numparam*sizeof(cell));
  assert(sym->ident==iFUNCTN);
  ffcall(sym,numparam);
  if (sc_status!=statSKIP)
    sym->usage|=uREFER;         /* do not mark as "used" when this call itself is skipped */
  if (sym->x.lib!=NULL)
    sym->x.lib->value += 1;     /* increment "usage count" of the library */
  sideeffect=TRUE;              /* assume functions carry out a side-effect */
  *resulttag=sym->tag;          /* save tag of the called function */

  if (lval!=NULL) {
    if (lval->ident==iARRAYCELL || lval->ident==iARRAYCHAR)
      pop2();                   /* restore address (in ALT) */
    store(lval);                /* store PRI in the symbol */
    moveto1();                  /* make sure PRI is restored on exit */
  } /* if */
  return TRUE;
}

SC_FUNC int matchtag(int formaltag,int actualtag,int allowcoerce)
{
  if (formaltag!=actualtag) {
    /* if the formal tag is zero and the actual tag is not "fixed", the actual
     * tag is "coerced" to zero
     */
    if (!allowcoerce || formaltag!=0 || (actualtag & FIXEDTAG)!=0)
      return FALSE;
  } /* if */
  return TRUE;
}

/*
 *  The AMX pseudo-processor has no direct support for logical (boolean)
 *  operations. These have to be done via comparing and jumping. Since we are
 *  already jumping through the code, we might as well implement an "early
 *  drop-out" evaluation (also called "short-circuit"). This conforms to
 *  standard C:
 *
 *  expr1 || expr2           expr2 will only be evaluated if expr1 is false.
 *  expr1 && expr2           expr2 will only be evaluated if expr1 is true.
 *
 *  expr1 || expr2 && expr3  expr2 will only be evaluated if expr1 is false
 *                           and expr3 will only be evaluated if expr1 is
 *                           false and expr2 is true.
 *
 *  Code generation for the last example proceeds thus:
 *
 *      evaluate expr1
 *      operator || found
 *      jump to "l1" if result of expr1 not equal to 0
 *      evaluate expr2
 *      ->  operator && found; skip to higher level in hierarchy diagram
 *          jump to "l2" if result of expr2 equal to 0
 *          evaluate expr3
 *          jump to "l2" if result of expr3 equal to 0
 *          set expression result to 1 (true)
 *          jump to "l3"
 *      l2: set expression result to 0 (false)
 *      l3:
 *      <-  drop back to previous hierarchy level
 *      jump to "l1" if result of expr2 && expr3 not equal to 0
 *      set expression result to 0 (false)
 *      jump to "l4"
 *  l1: set expression result to 1 (true)
 *  l4:
 *
 */

/*  Skim over terms adjoining || and && operators
 *  dropval   The value of the expression after "dropping out". An "or" drops
 *            out when the left hand is TRUE, so dropval must be 1 on "or"
 *            expressions.
 *  endval    The value of the expression when no expression drops out. In an
 *            "or" expression, this happens when both the left hand and the
 *            right hand are FALSE, so endval must be 0 for "or" expressions.
 */
static int skim(int *opstr,void (*testfunc)(int),int dropval,int endval,
                int (*hier)(value*),value *lval)
{
  int lvalue,hits,droplab,endlab,opidx;
  int allconst;
  cell constval;
  int index;
  cell cidx;

  stgget(&index,&cidx);         /* mark position in code generator */
  hits=FALSE;                   /* no logical operators "hit" yet */
  allconst=TRUE;                /* assume all values "const" */
  constval=0;
  droplab=0;                    /* to avoid a compiler warning */
  for ( ;; ) {
    lvalue=plnge1(hier,lval);   /* evaluate left expression */

    allconst= allconst && (lval->ident==iCONSTEXPR);
    if (allconst) {
      if (hits) {
        /* one operator was already found */
        if (testfunc==jmp_ne0)
          lval->constval= lval->constval || constval;
        else
          lval->constval= lval->constval && constval;
      } /* if */
      constval=lval->constval;  /* save result accumulated so far */
    } /* if */

    if (nextop(&opidx,opstr)) {
      if (!hits) {
        /* this is the first operator in the list */
        hits=TRUE;
        droplab=getlabel();
      } /* if */
      dropout(lvalue,testfunc,droplab,lval);
    } else if (hits) {                       /* no (more) identical operators */
      dropout(lvalue,testfunc,droplab,lval); /* found at least one operator! */
      const1(endval);
      jumplabel(endlab=getlabel());
      setlabel(droplab);
      const1(dropval);
      setlabel(endlab);
      lval->sym=NULL;
      lval->tag=0;
      if (allconst) {
        lval->ident=iCONSTEXPR;
        lval->constval=constval;
        stgdel(index,cidx);     /* scratch generated code and calculate */
      } else {
        lval->ident=iEXPRESSION;
        lval->constval=0;
      } /* if */
      return FALSE;
    } else {
      return lvalue;            /* none of the operators in "opstr" were found */
    } /* if */

  } /* while */
}

/*
 *  Reads into the primary register the variable pointed to by lval if
 *  plunging through the hierarchy levels detected an lvalue. Otherwise
 *  if a constant was detected, it is loaded. If there is no constant and
 *  no lvalue, the primary register must already contain the expression
 *  result.
 *
 *  After that, the compare routines "jmp_ne0" or "jmp_eq0" are called, which
 *  compare the primary register against 0, and jump to the "early drop-out"
 *  label "exit1" if the condition is true.
 */
static void dropout(int lvalue,void (*testfunc)(int val),int exit1,value *lval)
{
  if (lvalue)
    rvalue(lval);
  else if (lval->ident==iCONSTEXPR)
    const1(lval->constval);
  (*testfunc)(exit1);
}

static void checkfunction(value *lval)
{
  symbol *sym=lval->sym;

  if (sym==NULL || (sym->ident!=iFUNCTN && sym->ident!=iREFFUNC))
    return;             /* no known symbol, or not a function result */

  assert((sym->usage & (uDEFINE | uREFER))!=0);/* at least one should be set */
  if ((sym->usage & uDEFINE)!=0) {
    /* function is defined, can now check the return value (but make an
     * exception for directly recursive functions)
     */
    if (sym!=curfunc && (sym->usage & uRETVALUE)==0)
      error(209);       /* function should return a value */
  } else {
    /* function not yet defined, set */
    sym->usage|=uRETVALUE;      /* make sure that a future implementation of
                                 * the function uses "return <value>" */
  } /* if */
}

/*
 *  Plunge to a lower level
 */
static int plnge(int *opstr,int opoff,int (*hier)(value *lval),value *lval,char *forcetag)
{
  int lvalue,opidx;
  value lval2;

  lvalue=plnge1(hier,lval);
  if (nextop(&opidx,opstr)==0)
    return lvalue;              /* no operator in "opstr" found */
  if (lvalue)
    rvalue(lval);
  do {
    opidx+=opoff;
    plnge2(op1[opidx],hier,lval,&lval2);
  } while (nextop(&opidx,opstr)); /* enddo */
  if (forcetag!=NULL)
    lval->tag=sc_addtag(forcetag);
  return FALSE;         /* result of expression is not an lvalue */
}

/*  plnge_rel
 *
 *  Binary plunge to lower level; this is very simular to plnge, but
 *  it has special code generation sequences for chained operations.
 */
static int plnge_rel(int *opstr,int opoff,int (*hier)(value *lval),value *lval)
{
  int lvalue,opidx;
  value lval2;
  int count;

  /* this function should only be called for relational operators */
  assert(op1[opoff]==os_le);
  lvalue=plnge1(hier,lval);
  if (nextop(&opidx,opstr)==0)
    return lvalue;              /* no operator in "opstr" found */
  if (lvalue)
    rvalue(lval);
  count=0;
  lval->boolresult=TRUE;
  do {
    if (count>0)
      relop_prefix();
    opidx+=opoff;
    plnge2(op1[opidx],hier,lval,&lval2);
    if (count++>0)
      relop_suffix();
  } while (nextop(&opidx,opstr)); /* enddo */
  lval->constval=lval->boolresult;
  lval->tag=sc_addtag("bool");    /* force tag to be "bool" */
  return FALSE;         /* result of expression is not an lvalue */
}

/*  plnge1
 *
 *  Unary plunge to lower level
 *  Called by: skim(), plnge(), plnge2(), plnge_rel(), hier14() and hier13()
 */
static int plnge1(int (*hier)(value *lval),value *lval)
{
  int lvalue,index;
  cell cidx;

  stgget(&index,&cidx); /* mark position in code generator */
  lvalue=(*hier)(lval);
  if (lval->ident==iCONSTEXPR)
    stgdel(index,cidx); /* load constant later */
  return lvalue;
}

/*  plnge2
 *
 *  Binary plunge to lower level
 *  Called by: plnge(), plnge_rel(), hier14() and hier1()
 */
static void plnge2(void (*oper)(void),
                   int (*hier)(value *lval),
                   value *lval1,value *lval2)
{
  int index;
  cell cidx;

  stgget(&index,&cidx);             /* mark position in code generator */
  if (lval1->ident==iCONSTEXPR) {   /* constant on left side; it is not yet loaded */
    if (plnge1(hier,lval2))
      rvalue(lval2);
    const2(lval1->constval<<dbltest(oper,lval2,lval1));
                   /* ^ doubling of constants operating on integer addresses */
                   /*   is restricted to "add" and "subtract" operators */
  } else {                          /* non-constant on left side */
    push1();
    if (plnge1(hier,lval2))
      rvalue(lval2);
    if (lval2->ident==iCONSTEXPR) { /* constant on rigth side */
      if (commutative(oper)) {      /* test for commutative operators */
        value lvaltmp;
        stgdel(index,cidx);         /* scratch push1() and constant fetch (then
                                     * fetch the constant again */
        const2(lval2->constval<<dbltest(oper,lval1,lval2));
        /* now, the primary register has the left operand and the secondary
         * register the right operand; swap the "lval" variables so that lval1
         * is associated with the secondary register and lval2 with the
         * primary register, as is the "normal" case.
         */
        lvaltmp=*lval1;
        *lval1=*lval2;
        *lval2=lvaltmp;
      } else {
        const1(lval2->constval<<dbltest(oper,lval1,lval2));
        pop2();         /* pop result of left operand into secondary register */
      } /* if */
    } else {            /* non-constants on both sides */
      pop2();
      if (dbltest(oper,lval1,lval2))
        cell2addr();                    /* double primary register */
      if (dbltest(oper,lval2,lval1))
        cell2addr_alt();                /* double secondary register */
    } /* if */
  } /* if */
  if (oper) {
    /* If used in an expression, a function should return a value.
     * If the function has been defined, we can check this. If the
     * function was not defined, we can set this requirement (so that
     * a future function definition can check this bit.
     */
    checkfunction(lval1);
    checkfunction(lval2);
    if (lval1->ident==iARRAY || lval1->ident==iREFARRAY) {
      char *ptr=(lval1->sym->name!=NULL) ? lval1->sym->name : "-unknown-";
      error(33,ptr);                    /* array must be indexed */
    } else if (lval2->ident==iARRAY || lval2->ident==iREFARRAY) {
      char *ptr=(lval2->sym->name!=NULL) ? lval2->sym->name : "-unknown-";
      error(33,ptr);                    /* array must be indexed */
    } /* if */
    /* ??? ^^^ should do same kind of error checking with functions */

    /* check whether an "operator" function is defined for the tagnames
     * (a constant expression cannot be optimized in that case)
     */
    if (check_userop(oper,lval1->tag,lval2->tag,2,NULL,&lval1->tag)) {
      lval1->ident=iEXPRESSION;
      lval1->constval=0;
    } else if (lval1->ident==iCONSTEXPR && lval2->ident==iCONSTEXPR) {
      /* only constant expression if both constant */
      stgdel(index,cidx);       /* scratch generated code and calculate */
      if (!matchtag(lval1->tag,lval2->tag,FALSE))
        error(213);             /* tagname mismatch */
      lval1->constval=calc(lval1->constval,oper,lval2->constval,&lval1->boolresult);
    } else {
      if (intest && (oper==ob_and || oper==ob_or))
        error(212);             /* possibly unintended bitwise operation */
      if (!matchtag(lval1->tag,lval2->tag,FALSE))
        error(213);             /* tagname mismatch */
      (*oper)();                /* do the (signed) operation */
      lval1->ident=iEXPRESSION;
    } /* if */
  } /* if */
}

static cell truemodulus(cell a,cell b)
{
  return (a % b + b) % b;
}

static cell calc(cell left,void (*oper)(),cell right,char *boolresult)
{
  if (oper==ob_or)
    return (left | right);
  else if (oper==ob_xor)
    return (left ^ right);
  else if (oper==ob_and)
    return (left & right);
  else if (oper==ob_eq)
    return (left == right);
  else if (oper==ob_ne)
    return (left != right);
  else if (oper==os_le)
    return *boolresult &= (left <= right), right;
  else if (oper==os_ge)
    return *boolresult &= (left >= right), right;
  else if (oper==os_lt)
    return *boolresult &= (left < right), right;
  else if (oper==os_gt)
    return *boolresult &= (left > right), right;
  else if (oper==os_sar)
    return (left >> (int)right);
  else if (oper==ou_sar)
    return ((ucell)left >> (ucell)right);
  else if (oper==ob_sal)
    return ((ucell)left << (int)right);
  else if (oper==ob_add)
    return (left + right);
  else if (oper==ob_sub)
    return (left - right);
  else if (oper==os_mult)
    return (left * right);
  else if (oper==os_div)
    return (left - truemodulus(left,right)) / right;
  else if (oper==os_mod)
    return truemodulus(left,right);
  else
    error(29);  /* invalid expression, assumed 0 (this should never occur) */
  return 0;
}

SC_FUNC int expression(int *constant,cell *val,int *tag)
{
  value lval;

  if (hier14(&lval))
    rvalue(&lval);
  if (lval.ident==iCONSTEXPR) {        /* constant expression */
    *constant=TRUE;
    *val=lval.constval;
  } else {
    *constant=0;
  } /* if */
  if (tag!=NULL)
    *tag=lval.tag;
  return lval.ident;
}

static cell arraysize(symbol *sym)
{
  cell length;

  assert(sym!=NULL);
  assert(sym->ident==iARRAY || sym->ident==iREFARRAY);
  length=sym->dim.array.length;
  if (sym->dim.array.level > 0)
    length+=length*arraysize(finddepend(sym));
  return length;
}

/*  hier14
 *
 *  Lowest hierarchy level (except for the , operator).
 *
 *  Global references: intest   (reffered to only)
 */
SC_FUNC int hier14(value *lval1)
{
  int lvalue;
  value lval2,lval3;
  void (*oper)(void);
  int tok,level;
  cell val;
  char *st;

  lvalue=plnge1(hier13,lval1);
  if (lval1->ident==iCONSTEXPR)  /* load constant here */
    const1(lval1->constval);
  tok=lex(&val,&st);
  switch (tok) {
    case taOR:
      oper=ob_or;
      break;
    case taXOR:
      oper=ob_xor;
      break;
    case taAND:
      oper=ob_and;
      break;
    case taADD:
      oper=ob_add;
      break;
    case taSUB:
      oper=ob_sub;
      break;
    case taMULT:
      oper=os_mult;
      break;
    case taDIV:
      oper=os_div;
      break;
    case taMOD:
      oper=os_mod;
      break;
    case taSHRU:
      oper=ou_sar;
      break;
    case taSHR:
      oper=os_sar;
      break;
    case taSHL:
      oper=ob_sal;
      break;
    case '=':           /* simple assignment */
      oper=NULL;
      if (intest)
        error(211);     /* possibly unintended assignment */
      break;
    default:
      lexpush();
      return lvalue;
  } /* switch */

  /* if we get here, it was an assignment; first check a few special cases
   * and then the general */
  if (lval1->ident==iARRAYCHAR) {
    /* special case, assignment to packed character in a cell is permitted */
    lvalue=TRUE;
  } else if (lval1->ident==iARRAY || lval1->ident==iREFARRAY) {
    /* array assignment is permitted too (with restrictions) */
    if (oper)
      return error(23); /* array assignment must be simple assigment */
    assert(lval1->sym!=NULL);
    if (lval1->sym->dim.array.length==0)
      return error(46,lval1->sym->name);        /* unknown array size */
    lvalue=TRUE;
  } /* if */

  /* operand on left side of assignment must be lvalue */
  if (!lvalue)
    return error(22);                   /* must be lvalue */
  /* may not change "constant" parameters */
  assert(lval1->sym!=NULL);
  if ((lval1->sym->usage & uCONST)!=0)
    return error(22);   /* assignment to const argument */
  lval3=*lval1;         /* save symbol to enable storage of expresion result */
  if (lval1->ident==iARRAYCELL || lval1->ident==iARRAYCHAR
      || lval1->ident==iARRAY || lval1->ident==iREFARRAY)
  {
    /* if indirect fetch: save PRI (cell address) */
    if (oper) {
      push1();
      rvalue(lval1);
    } /* if */
    plnge2(oper,hier14,lval1,&lval2);
    if (oper)
      pop2();
  } else {
    if (oper){
      rvalue(lval1);
      plnge2(oper,hier14,lval1,&lval2);
    } else {
      /* if direct fetch and simple assignment: no "push"
       * and "pop" needed -> call hier14() directly, */
      if (hier14(&lval2))
        rvalue(&lval2);   /* instead of plnge2(). */
      checkfunction(&lval2);
    } /* if */
  } /* if */
  if (!oper && !matchtag(lval3.tag,lval2.tag,TRUE))
    error(213);           /* tagname mismatch (if "oper", warning already given in plunge2()) */
  if (lval3.ident==iARRAY || lval3.ident==iREFARRAY) {
    /* left operand is an array, right operand should be an array variable
     * of the same size and the same dimension, an array literal (of the
     * same size) or a literal string.
     */
    int exactmatch=TRUE;
    if (lval2.ident!=iARRAY && lval2.ident!=iREFARRAY)
      error(33,lval3.sym->name);        /* array must be indexed */
    if (lval2.sym!=NULL) {
      val=lval2.sym->dim.array.length;  /* array variable */
      level=lval2.sym->dim.array.level;
    } else {
      val=lval2.constval;               /* literal array */
      level=0;
      /* If val is negative, it means that lval2 is a
       * literal string. The string array size may be
       * smaller than the destination array.
       */
      if (val<0) {
        val=-val;
        exactmatch=FALSE;
      } /* if */
    } /* if */
    if (lval3.sym->dim.array.level!=level)
      return error(48); /* array dimensions must match */
    else if (lval3.sym->dim.array.length<val
             || exactmatch && lval3.sym->dim.array.length>val)
      return error(47); /* array sizes must match */
    if (level>0) {
      /* check the sizes of all sublevels too */
      symbol *sym1 = lval3.sym;
      symbol *sym2 = lval2.sym;
      int i;
      assert(sym1!=NULL && sym2!=NULL);
      /* ^^^ sym2 must be valid, because only variables can be
       *     multi-dimensional (there are no multi-dimensional arrays),
       *     sym1 must be valid because it must be an lvalue
       */
      assert(exactmatch);
      for (i=0; i<level; i++) {
        sym1=finddepend(sym1);
        sym2=finddepend(sym2);
        assert(sym1!=NULL && sym2!=NULL);
        /* ^^^ both arrays have the same dimensions (this was checked
         *     earlier) so the dependend should always be found
         */
        if (sym1->dim.array.length!=sym2->dim.array.length)
          error(47);    /* array sizes must match */
      } /* for */
      // get the total size in cells of the multi-dimensional array
      val=arraysize(lval3.sym);
    } /* if */
  } else {
    /* left operand is not an array, right operand should then not be either */
    if (lval2.ident==iARRAY || lval2.ident==iREFARRAY)
      error(6);         /* must be assigned to an array */
  } /* if */
  if (lval3.ident==iARRAY || lval3.ident==iREFARRAY)
    memcopy(val*sizeof(cell));
  else
    store(&lval3);      /* now, store the expression result */
  if (lval3.sym)
    lval3.sym->usage |= uWRITTEN;
  sideeffect=TRUE;
  return FALSE;         /* expression result is never an lvalue */
}

static int hier13(value *lval)
{
  int lvalue,flab1,flab2;
  value lval2;
  int array1,array2;

  lvalue=plnge1(hier12,lval);
  if (matchtoken('?')) {
    flab1=getlabel();
    flab2=getlabel();
    if (lvalue) {
      rvalue(lval);
    } else if (lval->ident==iCONSTEXPR) {
      const1(lval->constval);
      error(lval->constval ? 206 : 205);        /* redundant test */
    } /* if */
    jmp_eq0(flab1);             /* go to second expression if primary register==0 */
    if (hier14(lval))
      rvalue(lval);
    jumplabel(flab2);
    setlabel(flab1);
    needtoken(':');
    if (hier14(&lval2))
      rvalue(&lval2);
    array1= (lval->ident==iARRAY || lval->ident==iREFARRAY);
    array2= (lval2.ident==iARRAY || lval2.ident==iREFARRAY);
    if (array1 && !array2) {
      char *ptr=(lval->sym->name!=NULL) ? lval->sym->name : "-unknown-";
      error(33,ptr);            /* array must be indexed */
    } else if (!array1 && array2) {
      char *ptr=(lval2.sym->name!=NULL) ? lval2.sym->name : "-unknown-";
      error(33,ptr);            /* array must be indexed */
    } /* if */
    /* ??? if both are arrays, should check dimensions */
    if (!matchtag(lval->tag,lval2.tag,FALSE))
      error(213);               /* tagname mismatch ('true' and 'false' expressions) */
    setlabel(flab2);
    lval->ident=iEXPRESSION;    /* not an lvalue, not a constant */
    return FALSE;               /* conditional expression is no lvalue */
  } else {
    return lvalue;
  } /* endif */
}

/* the order of the operators in these lists is important and must cohere */
/* with the order of the operators in the array "op1" */
static int list3[]  = {'*','/','%',0};
static int list4[]  = {'+','-',0};
static int list5[]  = {tSHL,tSHR,tSHRU,0};
static int list6[]  = {'&',0};
static int list7[]  = {'^',0};
static int list8[]  = {'|',0};
static int list9[]  = {tlLE,tlGE,'<','>',0};
static int list10[] = {tlEQ,tlNE,0};
static int list11[] = {tlAND,0};
static int list12[] = {tlOR,0};

static int hier12(value *lval)
{
  return skim(list12,jmp_ne0,1,0,hier11,lval);
}

static int hier11(value *lval)
{
  return skim(list11,jmp_eq0,0,1,hier10,lval);
}

static int hier10(value *lval)
{ /* ==, != */
  return plnge(list10,15,hier9,lval,"bool");
}                  /* ^ this variable is the starting index in the op1[]
                    *   array of the operators of this hierarchy level */

static int hier9(value *lval)
{ /* <=, >=, <, > */
  return plnge_rel(list9,11,hier8,lval);
}

static int hier8(value *lval)
{ /* | */
  return plnge(list8,10,hier7,lval,NULL);
}

static int hier7(value *lval)
{ /* ^ */
  return plnge(list7,9,hier6,lval,NULL);
}

static int hier6(value *lval)
{ /* & */
  return plnge(list6,8,hier5,lval,NULL);
}

static int hier5(value *lval)
{ /* <<, >>, >>> */
  return plnge(list5,5,hier4,lval,NULL);
}

static int hier4(value *lval)
{ /* +, - */
  return plnge(list4,3,hier3,lval,NULL);
}

static int hier3(value *lval)
{ /* *, /, % */
  return plnge(list3,0,hier2,lval,NULL);
}

static int hier2(value *lval)
{
  int lvalue,tok;
  int tag,paranthese;
  cell val;
  char *st;
  symbol *sym;
  int saveresult;

  tok=lex(&val,&st);
  switch (tok) {
  case tINC:                    /* ++lval */
    if (!hier2(lval))
      return error(22);         /* must be lvalue */
    assert(lval->sym!=NULL);
    if ((lval->sym->usage & uCONST)!=0)
      return error(22);         /* assignment to const argument */
    if (!check_userop(user_inc,lval->tag,0,1,lval,&lval->tag))
      inc(lval);                /* increase variable first */
    rvalue(lval);               /* and read the result into PRI */
    sideeffect=TRUE;
    return FALSE;               /* result is no longer lvalue */
  case tDEC:                    /* --lval */
    if (!hier2(lval))
      return error(22);         /* must be lvalue */
    assert(lval->sym!=NULL);
    if ((lval->sym->usage & uCONST)!=0)
      return error(22);         /* assignment to const argument */
    if (!check_userop(user_dec,lval->tag,0,1,lval,&lval->tag))
      dec(lval);                /* decrease variable first */
    rvalue(lval);               /* and read the result into PRI */
    sideeffect=TRUE;
    return FALSE;               /* result is no longer lvalue */
  case '~':                     /* ~ (one's complement) */
    if (hier2(lval))
      rvalue(lval);
    invert();                   /* bitwise NOT */
    lval->constval=~lval->constval;
    return FALSE;
  case '!':                     /* ! (logical negate) */
    if (hier2(lval))
      rvalue(lval);
    if (check_userop(lneg,lval->tag,0,1,NULL,&lval->tag)) {
      lval->ident=iEXPRESSION;
      lval->constval=0;
    } else {
      lneg();                   /* 0 -> 1,  !0 -> 0 */
      lval->constval=!lval->constval;
      lval->tag=sc_addtag("bool");
    } /* if */
    return FALSE;
  case '-':                     /* unary - (two's complement) */
    if (hier2(lval))
      rvalue(lval);
    if (check_userop(neg,lval->tag,0,1,NULL,&lval->tag)) {
      lval->ident=iEXPRESSION;
      lval->constval=0;
    } else {
      neg();                    /* arithmic negation */
      lval->constval=-lval->constval;
    } /* if */
    return FALSE;
  case tLABEL:                  /* tagname override */
    tag=sc_addtag(st);
    if (hier2(lval))
      rvalue(lval);
    lval->tag=tag;
    return FALSE;
  case tDEFINED:
    paranthese= matchtoken('(');
    tok=lex(&val,&st);
    if (tok!=tSYMBOL)
      return error(20,st);      /* illegal symbol name */
    lval->ident=iCONSTEXPR;
    lval->constval= findconst(st)!=NULL || findloc(st)!=NULL || findglb(st)!=NULL;
    const1(lval->constval);
    if (paranthese)
      needtoken(')');
    return FALSE;
  case tSIZEOF:
    paranthese= matchtoken('(');
    tok=lex(&val,&st);
    if (tok!=tSYMBOL)
      return error(20,st);      /* illegal symbol name */
    sym=findloc(st);
    if (sym==NULL)
      sym=findglb(st);
    if (sym==NULL)
      return error(17,st);      /* undefined symbol */
    if (sym->ident==iCONSTEXPR)
      error(39);                /* constant symbol has no size */
    /* ??? check for functions too */
    lval->ident=iCONSTEXPR;
    if (sym->ident==iARRAY || sym->ident==iREFARRAY)
      lval->constval=arraysize(sym);
    else
      lval->constval=1;
    const1(lval->constval);
    if (paranthese)
      needtoken(')');
    return FALSE;
  default:
    lexpush();
    lvalue=hier1(lval);
    /* check for postfix operators */
    if (matchtoken(';')) {
      /* Found a ';', do not look further for postfix operators */
      lexpush();                /* push ';' back after successful match */
      return lvalue;
    } else if (matchtoken(tTERM)) {
      /* Found a newline that ends a statement (this is the case when
       * semicolons are optional). Note that an explicit semicolon was
       * handled above. This case is similar, except that the token must
       * not be pushed back.
       */
      return lvalue;
    } else {
      tok=lex(&val,&st);
      switch (tok) {
      case tINC:                /* lval++ */
        if (!lvalue)
          return error(22);     /* must be lvalue */
        assert(lval->sym!=NULL);
        if ((lval->sym->usage & uCONST)!=0)
          return error(22);     /* assignment to const argument */
        /* on incrementing array cells, the address in PRI must be saved for
         * incremening the value, whereas the current value must be in PRI
         * on exit.
         */
        saveresult= (lval->ident==iARRAYCELL || lval->ident==iARRAYCHAR);
        if (saveresult)
          push1();              /* save address in PRI */
        rvalue(lval);           /* read current value into PRI */
        if (saveresult)
          swap1();              /* save PRI on the stack, restore address in PRI */
        if (!check_userop(user_inc,lval->tag,0,1,lval,&lval->tag))
          inc(lval);            /* increase variable afterwards */
        if (saveresult)
          pop1();               /* restore PRI (result of rvalue()) */
        sideeffect=TRUE;
        return FALSE;           /* result is no longer lvalue */
      case tDEC:                /* lval-- */
        if (!lvalue)
          return error(22);     /* must be lvalue */
        assert(lval->sym!=NULL);
        if ((lval->sym->usage & uCONST)!=0)
          return error(22);     /* assignment to const argument */
        saveresult= (lval->ident==iARRAYCELL || lval->ident==iARRAYCHAR);
        if (saveresult)
          push1();              /* save address in PRI */
        rvalue(lval);           /* read current value into PRI */
        if (saveresult)
          swap1();              /* save PRI on the stack, restore address in PRI */
        if (!check_userop(user_dec,lval->tag,0,1,lval,&lval->tag))
          dec(lval);            /* decrease variable afterwards */
        if (saveresult)
          pop1();               /* restore PRI (result of rvalue()) */
        sideeffect=TRUE;
        return FALSE;
      case tCHAR:               /* char (compute required # of cells */
        if (lval->ident==iCONSTEXPR) {
          lval->constval *= charbits/8;   /* from char to bytes */
          lval->constval = (lval->constval + sizeof(cell)-1) / sizeof(cell);
        } else {
          if (lvalue)
            rvalue(lval);       /* fetch value if not already in PRI */
          char2addr();          /* from characters to bytes */
          addconst(sizeof(cell)-1);     /* make sure the value is rounded up */
          addr2cell();          /* truncate to number of cells */
        } /* if */
        return FALSE;
      default:
        lexpush();
        return lvalue;
      } /* switch */
    } /* if */
  } /* switch */
}

/*  hier1
 *
 *  The highest hierarchy level: it looks for pointer and array indices
 *  and function calls.
 *  Generates code to fetch a pointer value if it is indexed and code to
 *  add to the pointer value or the array address (the address is already
 *  read at primary()). It also generates code to fetch a function address
 *  if that hasn't already been done at primary() (check lval[4]) and calls
 *  callfunction() to call the function.
 */
static int hier1(value *lval1)
{
  int lvalue,index,tok,close;
  cell val,cidx;
  value lval2;
  char *st;
  symbol *sym;

  lvalue=primary(lval1);
restart:
  sym=lval1->sym;
  if (matchtoken('[') || matchtoken('{') || matchtoken('(')){
    tok=tokeninfo(&val,&st);
    if (tok=='[' || tok=='{') { /* subscript */
      close = (tok=='[') ? ']' : '}';
      if (sym==NULL) {  /* sym==NULL if lval is a constant or a literal */
        error(28);      /* cannot subscript */
        needtoken(close);
        return FALSE;
      } else if (sym->ident!=iARRAY && sym->ident!=iREFARRAY){
        error(28);      /* cannot subscript, variable is not an array */
        needtoken(close);
        return FALSE;
      } else if (sym->dim.array.level>0 && close!=']') {
        error(51);      /* invalid subscript, must use [ ] */
        needtoken(close);
        return FALSE;
      } /* if */
      stgget(&index,&cidx);     /* mark position in code generator */
      push1();                  /* save base address of the array */
      if (hier14(&lval2))       /* create expression for the array index */
        rvalue(&lval2);
      if (lval2.ident==iARRAY || lval2.ident==iREFARRAY)
        error(33,lval2.sym->name);      /* array must be indexed */
      needtoken(close);
      if (!matchtag(sym->x.idxtag,lval2.tag,TRUE))
        error(213);
      if (lval2.ident==iCONSTEXPR) {    /* constant expression */
        stgdel(index,cidx);             /* scratch generated code */
        if (close==']') {
          /* normal array index */
          if (sym->dim.array.length!=0 && sym->dim.array.length<=lval2.constval)
            error(32,sym->name);        /* array index out of bounds */
          if (lval2.constval!=0) {
            /* don't add offsets for zero subscripts */
            #if defined(BIT16)
              const2(lval2.constval<<1);
            #else
              const2(lval2.constval<<2);
            #endif
            ob_add();
          } /* if */
        } else {
          /* character index */
          if (sym->dim.array.length!=0 && sym->dim.array.length*(32/charbits)<=lval2.constval)
            error(32,sym->name);        /* array index out of bounds */
          if (lval2.constval!=0) {
            /* don't add offsets for zero subscripts */
            if (charbits==16)
              const2(lval2.constval<<1);/* 16-bit character */
            else
              const2(lval2.constval);   /* 8-bit character */
            ob_add();
          } /* if */
          charalign();                  /* align character index into array */
        } /* if */
      } else {
        if (close==']') {
          if (sym->dim.array.length!=0)
            ffbounds(sym->dim.array.length);/* run time check for array bounds */
          cell2addr();  /* normal array index */
        } else {
          if (sym->dim.array.length!=0)
            ffbounds(sym->dim.array.length*(32/charbits));
          char2addr();  /* character array index */
        } /* if */
        pop2();
        ob_add();       /* base address was popped into secondary register */
        if (close!=']')
          charalign();  /* align character index into array */
      } /* if */
      /* the indexed item may be another array (multi-dimensional arrays) */
      assert(lval1->sym==sym && sym!=NULL);     /* should still be set */
      if (sym->dim.array.level>0) {
        assert(close==']');     /* checked earlier */
        /* read the offset to the subarray and add it to the current address */
        lval1->ident=iARRAYCELL;
        push1();        /* the optimizer makes this to a MOVE.alt */
        rvalue(lval1);
        pop2();
        ob_add();
        /* adjust the "value" structure and find the referenced array */
        lval1->ident=iREFARRAY;
        lval1->sym=finddepend(sym);
        assert(lval1->sym!=NULL);
        assert(lval1->sym->dim.array.level==sym->dim.array.level-1);
        /* try to parse subsequent array indices */
        lvalue=FALSE;   /* for now, a iREFARRAY is no lvalue */
        goto restart;
      } /* if */
      assert(sym->dim.array.level==0);
      /* set type to fetch... INDIRECTLY */
      lval1->ident= (close==']') ? iARRAYCELL : iARRAYCHAR;
      lval1->tag=sym->tag;
      /* a cell in an array is an lvalue, a character in an array is not
       * always a *valid* lvalue */
      return TRUE;
    } else {            /* tok=='(' -> function(...) */
      if (sym==NULL
          || (sym->ident!=iFUNCTN && sym->ident!=iREFFUNC))
      {
        return error(12);       /* invalid function call */
      } else {
        callfunction(sym);
      } /* if */
      lval1->ident=iEXPRESSION;
      lval1->constval=0;
      lval1->tag=sym->tag;
      return FALSE;     /* result of function call is no lvalue */
    } /* if */
  } /* if */
  if (sym!=NULL && lval1->ident==iFUNCTN) {
    assert(sym->ident==iFUNCTN);
    address(sym);
    lval1->sym=NULL;
    lval1->ident=iREFFUNC;
    /* ??? however... function pointers (or function references are not (yet) allowed */
    error(29);                  /* expression error, assumed 0 */
    return FALSE;
  } /* if */
  return lvalue;
}

/*  primary
 *
 *  Returns 1 if the operand is an lvalue (everything except arrays, functions
 *  constants and -of course- errors).
 *  Generates code to fetch the address of arrays. Code for constants is
 *  already generated by constant().
 *  This routine first clears the entire lval array (all fields are set to 0).
 *
 *  Global references: intest  (may be altered, but restored upon termination)
 */
static int primary(value *lval)
{
  char *st;
  int lvalue,tok;
  cell val;
  symbol *sym;

  if (matchtoken('(')){         /* sub-expression - (expression,...) */
    pushstk((stkitem)intest);
    pushstk((stkitem)sc_allowtags);

    intest=0;                   /* no longer in "test" expression */
    sc_allowtags=TRUE;          /* allow tagnames to be used in parenthised expressions */
    do
      lvalue=hier14(lval);
    while (matchtoken(','));
    needtoken(')');

    sc_allowtags=(int)(long)popstk();
    intest=(int)(long)popstk();
    return lvalue;
  } /* if */

  lval->sym=NULL;        /* clear lval */
  lval->ident=0;
  lval->constval=0;
  lval->tag=0;
  tok=lex(&val,&st);
  if (tok==tSYMBOL && !findconst(st)) {
    /* first look for a local variable */
    if ((sym=findloc(st))!=0) {
      if (sym->ident==iLABEL) {
        error(29);      /* expression error, assumed 0 */
        const1(0);      /* load 0 */
        return FALSE;   /* return 0 for labels (expression error) */
      } /* if */
      lval->sym=sym;
      lval->ident=sym->ident;
      lval->tag=sym->tag;
      if (sym->ident==iARRAY || sym->ident==iREFARRAY) {
        address(sym);   /* get starting address in primary register */
        return FALSE;   /* return 0 for array (not lvalue) */
      } else {
        return TRUE;    /* return 1 if lvalue (not label or array) */
      } /* if */
    } /* if */
    /* now try a global variable */
    if ((sym=findglb(st))!=0) {
      if (sym->ident!=iFUNCTN) {
        lval->sym=sym;
        lval->ident=sym->ident;
        lval->tag=sym->tag;
        if (sym->ident==iARRAY || sym->ident==iREFARRAY) {
          address(sym);         /* get starting address in primary register */
          return FALSE;         /* return 0 for array (not lvalue) */
        } else {
          return TRUE;          /* return 1 if lvalue (not function or array) */
        } /* if */
      } /* if */
    } else {
      return error(17,st);      /* undefined symbol */
    } /* endif */
    assert(sym!=NULL);
    assert(sym->ident==iFUNCTN);
    lval->sym=sym;
    lval->ident=sym->ident;
    lval->tag=sym->tag;
    return FALSE;       /* return 0 for function (not an lvalue) */
  } /* if */
  lexpush();            /* push the token, it is analyzed by constant() */
  if (constant(lval)==0) {
    error(29);          /* expression error, assumed 0 */
    const1(0);          /* load 0 */
  } /* if */
  return FALSE;         /* return 0 for constants (or errors) */
}

static void setdefarray(cell *string,cell size,cell arraysize,cell *dataaddr,int fconst)
{
  /* The routine must copy the default array data onto the heap, as to avoid
   * that a function can change the default value. An optimization is that
   * the default array data is "dumped" into the data segment only once (on the
   * first use).
   */
  assert(string!=NULL);
  assert(size>0);
  /* check whether to dump the default array */
  assert(dataaddr!=NULL);
  if (sc_status==statWRITE && *dataaddr<0) {
    int i;
    *dataaddr=(litidx+glb_declared)*sizeof(cell);
    for (i=0; i<size; i++)
      stowlit(*string++);
  } /* if */

  /* if the function is known not to modify the array (meaning that it also
   * does not modify the default value), directly pass the address of the
   * array in the data segment.
   */
  if (fconst) {
    const1(*dataaddr);
  } else {
    /* Generate the code:
     *  CONST.pri dataaddr                ;address of the default array data
     *  HEAP      arraysize*sizeof(cell)  ;heap address in ALT
     *  MOVS      size*sizeof(cell)       ;copy data from PRI to ALT
     *  MOVE.PRI                          ;PRI = address on the heap
     */
    const1(*dataaddr);
    /*  "arraysize" is the size of the argument (the value between the brackets),
     * "size" is the size of the default array data.
     */
    assert(arraysize>=size);
    modheap((int)arraysize*sizeof(cell));
    /* ??? should perhaps fill with zeros first */
    memcopy(size*sizeof(cell));
    moveto1();
  } /* if */
}

static int findnamedarg(arginfo *arg,char *name)
{
  int i;

  for (i=0; arg[i].ident!=0 && arg[i].ident!=iVARARGS; i++)
    if (strcmp(arg[i].name,name)==0)
      return i;
  return -1;
}

static int checktag(int tags[],int numtags,int exprtag)
{
  int i;

  assert(tags!=0);
  assert(numtags>0);
  for (i=0; i<numtags; i++)
    if (matchtag(tags[i],exprtag,TRUE))
      return TRUE;    /* matching tag */
  return FALSE;       /* no tag matched */
}

/*  callfunction
 *
 *  Generates code to call a function. This routine handles default arguments
 *  and positional as well as named parameters.
 */
static void callfunction(symbol *sym)
{
  int close,lvalue;
  int argpos;       /* index in the output stream (argpos==nargs if positional parameters) */
  int argidx=0;     /* index in "arginfo" list */
  int nargs=0;      /* number of arguments */
  int heapalloc=0;
  int namedparams=FALSE;
  value lval;
  arginfo *arg;
  char arglist[sMAXARGS];
  cell lexval;
  char *lexstr;

  assert(sym!=NULL);
  arg=sym->dim.arglist;
  assert(arg!=NULL);
  stgmark(sSTARTREORDER);
  for (argpos=0; argpos<sMAXARGS; argpos++)
    arglist[argpos]=FALSE;
  if (!matchtoken(')')) {
    do {
      if (matchtoken('.')) {
        namedparams=TRUE;
        needtoken(tSYMBOL);
        tokeninfo(&lexval,&lexstr);
        argpos=findnamedarg(arg,lexstr);
        if (argpos<0) {
          error(17,lexstr);     /* undefined symbol */
          break;                /* exit loop, argpos is invalid */
        } /* if */
        needtoken('=');
        argidx=argpos;
      } else {
        if (namedparams)
          error(44);            /* positional parameters must precede named parameters */
        argpos=nargs;
      } /* if */
      stgmark(sEXPRSTART+argpos);/* mark beginning of new expression in stage */
      if (arglist[argpos])
        error(58);              /* argument already set */
      arglist[argpos]=TRUE;     /* flag argument as "present" */
      if (matchtoken('_')) {
        if (arg[argidx].ident==0 || arg[argidx].ident==iVARARGS) {
          error(202);           /* argument count mismatch */
        } else if (!arg[argidx].hasdefault) {
          error(34,nargs+1);    /* argument has no default value */
        } else {
          if (arg[argidx].ident==iREFARRAY) {
            setdefarray(arg[argidx].defvalue.array.data,
                        arg[argidx].defvalue.array.size,
                        arg[argidx].defvalue.array.arraysize,
                        &arg[argidx].defvalue.array.addr,
                        (arg[argidx].usage & uCONST)!=0);
            if ((arg[argidx].usage & uCONST)==0)
              heapalloc+=arg[argidx].defvalue.array.arraysize;
          } else if (arg[argidx].ident==iREFERENCE) {
            setheap(arg[argidx].defvalue.val);
            /* address of the value on the heap in PRI */
            heapalloc++;
          } else {
            const1(arg[argidx].defvalue.val);
          } /* if */
        } /* if */
        if (arg[argidx].ident!=0 && arg[argidx].ident!=iVARARGS)
          argidx++;
      } else {
        lvalue=hier14(&lval);
        switch (arg[argidx].ident) {
        case 0:
          error(202);           /* argument count mismatch */
          break;
        case iVARARGS:
          /* always pass by reference */
          if (lval.ident==iVARIABLE || lval.ident==iREFERENCE) {
            assert(lval.sym!=NULL);
            if ((lval.sym->usage & uCONST)!=0 && (arg[argidx].usage & uCONST)==0) {
              /* treat a "const" variable passed to a function with a non-const
               * "variable argument list" as a constant here */
              assert(lvalue);
              rvalue(&lval);    /* get value in PRI */
              setheap_pri();    /* address of the value on the heap in PRI */
              heapalloc++;
            } else {
              address(lval.sym);
            } /* if */
          } else if (lval.ident==iCONSTEXPR || lval.ident==iEXPRESSION
                     || lval.ident==iARRAYCHAR)
          {
            /* fetch value if needed */
            if (lval.ident==iARRAYCHAR)
              rvalue(&lval);
            /* allocate a cell on the heap and store the
             * value (already in PRI) there */
            setheap_pri();      /* address of the value on the heap in PRI */
            heapalloc++;
          } /* if */
          /* ??? handle const array passed by reference */
          /* otherwise, the address is already in PRI */
          if (lval.sym!=NULL)
            lval.sym->usage|=uWRITTEN;
          if (!checktag(arg[argidx].tags,arg[argidx].numtags,lval.tag))
            error(213);
          break;
        case iVARIABLE:
          if (lval.ident==iLABEL || lval.ident==iFUNCTN || lval.ident==iREFFUNC
              || lval.ident==iARRAY || lval.ident==iREFARRAY)
            error(35,argidx+1); /* argument type mismatch */
          if (lvalue)
            rvalue(&lval);      /* get value (direct or indirect) */
          /* otherwise, the expression result is already in PRI */
          if (!checktag(arg[argidx].tags,arg[argidx].numtags,lval.tag))
            error(213);
          argidx++;             /* argument done */
          break;
        case iREFERENCE:
          if (!lvalue || lval.ident==iARRAYCHAR)
            error(35,argidx+1); /* argument type mismatch */
          if (lval.sym!=NULL && (lval.sym->usage & uCONST)!=0 && (arg[argidx].usage & uCONST)==0)
            error(35,argidx+1); /* argument type mismatch */
          if (lval.ident==iVARIABLE || lval.ident==iREFERENCE) {
            assert(lval.sym!=NULL);
            address(lval.sym);
          } /* if */
          /* otherwise, the address is already in PRI */
          if (!checktag(arg[argidx].tags,arg[argidx].numtags,lval.tag))
            error(213);
          argidx++;             /* argument done */
          if (lval.sym!=NULL)
            lval.sym->usage|=uWRITTEN;
          break;
        case iREFARRAY:
          if (lval.ident!=iARRAY && lval.ident!=iREFARRAY
              && lval.ident!=iARRAYCELL)
            error(35,argidx+1); /* argument type mismatch */
          if (lval.sym!=NULL && (lval.sym->usage & uCONST)!=0 && (arg[argidx].usage & uCONST)==0)
            error(35,argidx+1); /* argument type mismatch */
          /* in the case that lval.ident == iARRAYCELL, verify that the
           * dimension of the array in arg[argidx] is 1
           */
          if (lval.ident==iARRAYCELL && arg[argidx].numdim!=1)
            error(35,argidx+1); /* argument type mismatch */
          /* In the case that lval.ident == iARRAY or iREFARRAY, verify that
           * the dimensions match with those in arg[argidx]. A literal
           * array always has a single dimension.
           */
          if (lval.ident==iARRAY || lval.ident==iREFARRAY) {
            if (lval.sym==NULL) {
              if (arg[argidx].numdim!=1) {
                error(48);      /* array dimensions must match */
              } else if (arg[argidx].dim[0]!=0) {
                assert(arg[argidx].dim[0]>0);
                assert(lval.constval!=0); /* literal array must have a size */
                /* A literal array must have exactly the same size as the
                 * function argument; a literal string may be smaller than
                 * the function argument.
                 */
                if (lval.constval>0 && arg[argidx].dim[0]!=lval.constval
                    || lval.constval<0 && arg[argidx].dim[0] < -lval.constval)
                  error(47);    /* array sizes must match */
              } /* if */
            } else {
              symbol *sym=lval.sym;
              int level=0;
              assert(sym!=NULL);
              if (sym->dim.array.level+1!=arg[argidx].numdim)
                error(48);      /* array dimensions must match */
              /* the lengths for all dimensions must match, except for the
               * last dimension
               */
              while (sym->dim.array.level>0) {
                assert(level<sDIMEN_MAX);
                if (sym->dim.array.length!=arg[argidx].dim[level])
                  error(47);    /* array sizes must match */
                sym=finddepend(sym);
                assert(sym!=NULL);
                level++;
              } /* if */
              /* the last dimension is optionally checked */
              assert(level<sDIMEN_MAX);
              assert(sym!=NULL);
              if (arg[argidx].dim[level]!=0 && sym->dim.array.length!=arg[argidx].dim[level])
                error(47);      /* array sizes must match */
            } /* if */
          } /* if */
          /* address already in PRI */
          if (!checktag(arg[argidx].tags,arg[argidx].numtags,lval.tag))
            error(213);
          // ??? set uWRITTEN?
          argidx++;             /* argument done */
          break;
        } /* switch */
      } /* switch */
      push1();                  /* store the function argument on the stack */
      nargs++;
      close=matchtoken(')');
      if (!close)               /* if not paranthese... */
        needtoken(',');         /* ...should be comma */
    } while (!close && freading && !matchtoken(tENDEXPR)); /* do */
  } /* if */
  /* check remaining function arguments (they may have default values) */
  for (argidx=0; arg[argidx].ident!=0 && arg[argidx].ident!=iVARARGS; argidx++) {
    if (arglist[argidx])
      continue;                 /* already seen this argument */
    stgmark(sEXPRSTART+argidx); /* mark beginning of new expression in stage */
    if (arg[argidx].hasdefault) {
      if (arg[argidx].ident==iREFARRAY) {
        setdefarray(arg[argidx].defvalue.array.data,
                    arg[argidx].defvalue.array.size,
                    arg[argidx].defvalue.array.arraysize,
                    &arg[argidx].defvalue.array.addr,
                    (arg[argidx].usage & uCONST)!=0);
        if ((arg[argidx].usage & uCONST)==0)
          heapalloc+=arg[argidx].defvalue.array.arraysize;
      } else if (arg[argidx].ident==iREFERENCE) {
        setheap(arg[argidx].defvalue.val);
        heapalloc++;
      } else {
        const1(arg[argidx].defvalue.val);
      } /* if */
      push1();                  /* store the function argument on the stack */
    } else {
      error(202,argidx);        /* argument count mismatch */
    } /* if */
    nargs++;
  } /* while */
  stgmark(sENDREORDER);         /* mark end of reversed evaluation */
  pushval((cell)nargs*sizeof(cell));
  ffcall(sym,nargs);
  if (sc_status!=statSKIP)
    sym->usage|=uREFER;         /* do not mark as "used" when this call itself is skipped */
  if (sym->x.lib!=NULL)
    sym->x.lib->value += 1;     /* increment "usage count" of the library */
  modheap(-heapalloc*sizeof(cell));
  sideeffect=TRUE;              /* assume functions carry out a side-effect */
}

/*  dbltest
 *
 *  Returns a non-zero value if lval1 an array and lval2 is not an array and
 *  the operation is addition or subtraction.
 *
 *  Returns the "shift" count (1 for 16-bit, 2 for 32-bit) to align a cell
 *  to an array offset.
 */
static int dbltest(void (*oper)(),value *lval1,value *lval2)
{
  if ((oper!=ob_add) && (oper!=ob_sub))
    return 0;
  if (lval1->ident!=iARRAY)
    return 0;
  if (lval2->ident==iARRAY)
    return 0;
  return sizeof(cell)/2;        /* 1 for 16-bit, 2 for 32-bit */
}

/*  commutative
 *
 *  Test whether an operator is commutative, i.e. x oper y == y oper x.
 *  Commutative operators are: +  (addition)
 *                             *  (multiplication)
 *                             == (equality)
 *                             != (inequality)
 *                             &  (bitwise and)
 *                             ^  (bitwise xor)
 *                             |  (bitwise or)
 *
 *  If in an expression, code for the left operand has been generated and
 *  the right operand is a constant and the operator is commutative, the
 *  precautionary "push" of the primary register is scrapped and the constant
 *  is read into the secondary register immediately.
 */
static int commutative(void (*oper)())
{
  return oper==ob_add || oper==os_mult
         || oper==ob_eq || oper==ob_ne
         || oper==ob_and || oper==ob_xor || oper==ob_or;
}

/*  constant
 *
 *  Generates code to fetch a number, a literal character (which is returned
 *  by lex() as a number as well) or a literal string (lex() stores the
 *  strings in the literal queue). If the operand was a number, it is stored
 *  in lval->constval.
 *
 *  The function returns 1 if the token was a constant or a string, 0
 *  otherwise.
 */
static int constant(value *lval)
{
  int tok,index,constant;
  cell val,item,cidx;
  char *st;
  symbol *sym;

  tok=lex(&val,&st);
  if (tok==tSYMBOL && (sym=findconst(st))!=0){
    lval->constval=sym->addr;
    const1(lval->constval);
    lval->ident=iCONSTEXPR;
    lval->tag=sym->tag;
    sym->usage |= uREAD;
  } else if (tok==tNUMBER) {
    lval->constval=val;
    const1(lval->constval);
    lval->ident=iCONSTEXPR;
  } else if (tok==tRATIONAL) {
    lval->constval=val;
    const1(lval->constval);
    lval->ident=iCONSTEXPR;
    lval->tag=sc_rationaltag;
  } else if (tok==tSTRING) {
    /* lex() stores starting index of string in the literal table in 'val' */
    const1((val+glb_declared)*sizeof(cell));
    lval->ident=iARRAY;         /* pretend this is a global array */
    lval->constval=val-litidx;  /* constval == the negative value of the
                                 * size of the literal array; using a negative
                                 * value distinguishes between literal arrays
                                 * and literal strings (this was done for
                                 * array assignment). */
  } else if (tok=='{') {
    int tag,lasttag=-1;
    val=litidx;
    do {
      /* cannot call constexpr() here, because "staging" is already turned
       * on at this point */
      assert(staging);
      stgget(&index,&cidx);     /* mark position in code generator */
      expression(&constant,&item,&tag);
      stgdel(index,cidx);       /* scratch generated code */
      if (constant==0)
        error(8);               /* must be constant expression */
      if (lasttag<0)
        lasttag=tag;
      else if (!matchtag(lasttag,tag,FALSE))
        error(213);             /* tagname mismatch */
      stowlit(item);            /* store expression result in literal table */
    } while (matchtoken(','));
    needtoken('}');
    const1((val+glb_declared)*sizeof(cell));
    lval->ident=iARRAY;         /* pretend this is a global array */
    lval->constval=litidx-val;  /* constval == the size of the literal array */
  } else {
    return FALSE;               /* no, it cannot be interpreted as a constant */
  } /* if */
  return TRUE;                  /* yes, it was a constant value */
}

