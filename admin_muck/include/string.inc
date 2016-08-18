/*
 *  String manipulation functions
 *
 *  $Id: string.inc,v 1.2 2001/03/21 05:13:37 magua Exp $
 *
 */

#if defined _string_included
  #endinput
#endif
#define _string_included


/*
 *  Copies source to dest, to maximum length (including terminating null).
 *  Returns 1 on success
 */
native strcpy(sDest[], sSource[], iMaxLen);

/*
 *  Like strcpy, but only copies iNum characters
 */
native strncpy(sDest[], sSource[], iNum, iMaxLen);

/*
 *  Concatenates source to dest, to maximum length (including terminating
 *  null).  Returns 1 on success.
 */
native strcat(sDest[], sSource[], iMaxLen);

/*
 *  Like strcat, but only concatenates iNum characters.
 */
native strncat(sDest[], sSource[], iNum, iMaxLen);

/*
 *  Returns 0 if string1 == string2, 1 if string1 > string2, and -1 if
 *  string 1 < string2.  Case sensitive.
 */
native strcmp(sString1[], sString2[]);

/*
 *  Like strcmp, but only compares first iNum characters.
 */
native strncmp(sString1[], sString2[], iNum);

/*
 *  Like strcmp, but ignores case.
 */
native strcasecmp(sString1[], sString2[]);

/*
 *  Like strncmp, but ignores case.
 */
native strncasecmp(sString1[], sString2[], iNum);

/*
 *  Returns index of occurrence of character within string.
 *  Returns -1 if the character does not exist in the string.
 */
native strchr(sSource[], iChar);

/*
 *  Different alias for strchar
 */
native index(sSource[], iChar);

/*
 *  Like strchr, but begins searching from the right instead
 *  of the left
 */
native strrchr(sSource[], iChar);

/*
 *  Different alias for strrchar
 */
native rindex(sSource[], iChar);

/*
 *  Like strchr, but searches for substring instead of just one
 *  character.  Case sensitive.
 */
native strstr(sSource[], sSearch[]);

/*
 *  Like strstr, but will return -1 if sSearch is empty
 */
native strstrx(sSource[], sSearch[]);

/*
 *  Like strstr, but ignores case
 */
native strcasestr(sSource[], sSearch[]);

/*
 *  Like strstrx, but ignores case.
 */
native strcasestrx(sSource[], sSearch[]);

/*
 *  Returns length (not index) of initial substring of source
 *  that is composed entirely of characters in search
 *  (eg, strspn("aaabcdef","abc") -> 5)
 */
native strspn(sSource[], sSearch[]);

/*
 *  Like strspn, but returns length of initial substring of source
 *  that is composed entirely of characters NOT in seearch
 */
native strcspn(sSource[], sSearch[]);

/*
 *  Returns the first token from source (where token is defined as
 *  a string held within delimiters), up to the maximum length).
 *  sToken is filled with token, and the return value is the length
 *  of sToken.  -1 if no token found.
 *  This function can be called repeatedly to get further tokens;
 *  in this case, leave source blank.
 */
native strtok(sSource[], sDelimiters[], sToken[], iMaxLen);

/*
 *  Returns in sRest what is left over of a string on which strtok has been called.
 *  Return value is the length of sRest, -1 if no rest is left or if strtok hasn't 
 *  been called yet.
 */
native strtokrest(sRest[], iMaxLen);

/*
 *  Like strtok, but allows for grouping 'meta-token' that ignores other tokens.
 *  (eg, strgtok("'This is' a test", " ", "'", sToken, 20)
 *  will return sToken1 as "This is")
 *  strgtok uses a *different* internal buffer than strtok. You cannot 
 *  mix strtok and strgtok calls on the same buffer (empty sSource).
 */
native strgtok(sSource[], sDelimiters[], sGrouping[], sToken[], iMaxLen);

/*
 *  Returns in sRestwhat is left over of a string on which strgtok has been called.
 *  Return value is the length of sRest, -1 if no rest is left or if strgtok hasn't 
 *  been called yet.
 */
native strgtokrest(sRest[], iMaxLen);


/*
 *  Like strtok, but can return all of the tokens at once; just
 *  provide more parameters.
 *  (eg, strsplit("This is a test", " ", sToken1, 50, sToken2, 50, sToken3, 50)
 *  will return sToken1 as "This", sToken2 as "is", and sToken3 as "a")
 *  Returns the number of tokens found, -1 if none.
 */
native strsplit(sSource[], sDelimiters[], ...);

/*
 *  Like strsplit, but allows for grouping 'meta-token' that ignores other tokens.
 *  (eg, strgsplit("This is ^"a test^"", " ", "^"", sToken1, 50, sToken2, 50, sToken3, 50)
 *  will return sToken1 as "This", sToken2 as "is", and sToken3 as "a test")
 */
native strgsplit(sSource[], sDelimiters[], sGrouping[], ...);

/*
 *  Like strsplit, but the last token will hold the complete rest of the tokenized
 *  string. (eg, strsep( "This is a string", " ", sToken1, 10, sToken2, 10 sRest, 50)
 *  will return sToken1 as "This", sToken2 as "is" and sRest as "a string")
 */
native strsep(sSource[], sDelimiters[], ...);

/*
 *  Like strsep, but allows for grouping 'meta-tokens' that ignore other tokens.
 */
native strgsep(sSource[], sDelimiters[], sGrouping[], ...);

/*
 *  Returns how often a character occurs in a string
 */
native strcount(sSource[], iChar);

/*
 *  Removes characters found in sTrim from the beginning or the end of
 *  a string. With iWhere = 0 it trims the beginning, with iWhere = 1
 *  it trims the end and with iWhere = 2 it trims both ends. Returns the
 *  number of characters trimmed, -1 on error.
 */
native strtrim(sString[], sTrim[], iWhere = 2);

/*
 *  Substitutes all occurences of substring sSubst in string sString with
 *  the string sWith. Returns the number of substitutions, -1 on error.
 */
native strsubst(sString[], sSubst[], sWith[], iMaxLen);

/*
 *  Formats the source string with the format, plus supplied parameters,
 *  up to limit of maximum length.  Returns the number of characters written,
 *  or -1 on failure.
 */
native snprintf(sDest[], iLength, sFormat[], ...);
