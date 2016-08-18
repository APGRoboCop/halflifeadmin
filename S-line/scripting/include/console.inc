/* Console input/output functions
 *
 * (c) Copyright 1998-2002, ITB CompuPhase
 * This file is provided as is (no warranties).
 */
#if defined _console_included
  #endinput
#endif
#define _console_included

enum
    {
    black,      /* colours as per the ANSI Escape sequences, ISO 6429 */
    red,
    green,
    yellow,
    blue,
    magenta,
    cyan,
    white,
    }

native getchar(echo=true);
native getstring(string[], maxlength, bool:pack=false);
native getvalue(base=10, end='^r', ...);
native print(const string[], foreground=-1, background=-1);
native printf(const format[], ...);
