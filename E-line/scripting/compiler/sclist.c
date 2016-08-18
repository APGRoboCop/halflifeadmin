/*  Small compiler  - maintenance of various lists
 *
 *  Name list (aliases)
 *  Include path list
 *
 *  Copyright (c) ITB CompuPhase, 2001-2002
 *  This file may be freely used. No warranties of any kind.
 *
 *  Version: $Id: sclist.c,v 1.1.1.1 2003/05/07 14:50:08 darope Exp $
 */
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "sc.h"

static namealias alias_tab = { NULL, "", ""};  /* alias table */

SC_FUNC namealias *insert_alias(char *name,char *alias)
{
  namealias *cur;

  assert(name!=NULL);
  assert(strlen(name)<=sNAMEMAX);
  assert(alias!=NULL);
  assert(strlen(alias)<=sEXPMAX);
  if ((cur=(namealias*)malloc(sizeof(namealias)))==NULL)
    error(103);       /* insufficient memory (fatal error) */
  memset(cur,0,sizeof(namealias));
  strcpy(cur->name,name);
  strcpy(cur->alias,alias);
  cur->next=alias_tab.next;
  alias_tab.next=cur;
  return cur;
}

SC_FUNC namealias *find_alias(char *name)
{
  namealias *cur = alias_tab.next;

  assert(name!=NULL);
  assert(strlen(name)<=sNAMEMAX);
  while (cur!=NULL) {
    if (strcmp(name,cur->name)==0)
      return cur;
    cur=cur->next;
  } /* while */
  return NULL;
}

SC_FUNC int lookup_alias(char *target,char *name)
{
  namealias *cur = find_alias(name);
  if (cur!=NULL) {
    assert(strlen(cur->alias)<=sEXPMAX);
    strcpy(target,cur->alias);
  } /* if */
  return cur!=NULL;
}

SC_FUNC void delete_aliastable(void)
{
  namealias *cur=alias_tab.next, *next;

  while (cur!=NULL) {
    next=cur->next;
    free(cur);
    cur=next;
  } /* while */
  memset(&alias_tab,0,sizeof(namealias));
}


static stringlist includepaths = { NULL, NULL};/* directory list for include files */

SC_FUNC stringlist *insert_path(char *path)
{
  stringlist *cur;

  assert(path!=NULL);
  if ((cur=(stringlist*)malloc(sizeof(stringlist)))==NULL)
    error(103);       /* insufficient memory (fatal error) */
  if ((cur->line=strdup(path))==NULL)
    error(103);       /* insufficient memory (fatal error) */
  cur->next=includepaths.next;
  includepaths.next=cur;
  return cur;
}

SC_FUNC char *get_path(int index)
{
  stringlist *cur = includepaths.next;

  while (cur!=NULL && index-->0)
    cur=cur->next;
  if (cur!=NULL) {
    assert(cur->line!=NULL);
    return cur->line;
  } /* if */
  return NULL;
}

SC_FUNC void delete_pathtable(void)
{
  stringlist *cur=includepaths.next, *next;

  while (cur!=NULL) {
    next=cur->next;
    assert(cur->line!=NULL);
    free(cur->line);
    free(cur);
    cur=next;
  } /* while */
  memset(&includepaths,0,sizeof(stringlist));
}

