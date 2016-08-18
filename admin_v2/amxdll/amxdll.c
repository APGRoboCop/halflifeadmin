 /*  Abstract Machine DLL interface functions
 *
 *  Copyright (c) ITB CompuPhase, 1999-2000
 *  This file may be freely used. No warranties of any kind.
 */
#define WIN32_LEAN_AND_MEAN
#include <assert.h>
#include <limits.h>
#include <malloc.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>

/* redirect amx_Init() to a special version */
#if !defined amx_Init
  #error "amx_Init" must be defined when compiling these sources
  // #define amx_Init   amx_InitAMX
#endif
#undef amx_Init /* redirection is over here */

#include "osdefs.h"
#include "amx.h"


int AMXAPI amx_InitAMX(AMX *amx, void *program);
static cell AMX_NATIVE_CALL _time(AMX *amx,cell *params);
static cell AMX_NATIVE_CALL _messagebox(AMX *amx,cell *params);

#ifdef WIN32
static HINSTANCE hinstAMX;
static HWND hwndConsole;
#endif

AMX_NATIVE_INFO dll_Natives[] = {
#if 0  
{ "messagebox", _messagebox },
#endif
  { NULL, NULL }        /* terminator */
};

#define _namemax        19
typedef struct {
  cell address;
  char name[_namemax+1];
} FUNCSTUB;

typedef AMX_NATIVE_INFO FAR *(FAR AMXAPI *AMX_ENTRY)(void);

void core_Init(void);   /* two functions from AMX_CORE.C */
void core_Exit(void);


int AMXAPI amx_Init(AMX *amx, void *program)
{
extern AMX_NATIVE_INFO FAR core_Natives[];
extern AMX_NATIVE_INFO FAR console_Natives[];
//extern AMX_NATIVE_INFO FAR fixed_Natives[];
// CEM - 03/05/01 - Rope's string functions
extern AMX_NATIVE_INFO FAR string_Natives[];
///CEM
  int err,i;
  AMX_HEADER *hdr;
  FUNCSTUB *lib;
#if 0
  HINSTANCE hlib;
#endif

  AMX_ENTRY entry;
  AMX_NATIVE_INFO *list;


  if ((err=amx_InitAMX(amx,program)) != AMX_ERR_NONE)
    return err;

#if 0
  /* load all listed libraries */
  hdr=(AMX_HEADER *)amx->base;
  for (i=0; i<hdr->num_libraries; i++) {
    lib=(FUNCSTUB *)((BYTE *)hdr+(int)hdr->libraries+i*sizeof(FUNCSTUB));
    hlib=GetModuleHandle(lib->name);
    if (hlib!=NULL) {
      /* a library that is not present (in memory) or that does not have
       * a amx_NativeList() function is simply ignored */
      entry=(AMX_ENTRY)GetProcAddress(hlib,"amx_NativeList");
      if (entry!=NULL && (list=entry())!=NULL)
        amx_Register(amx,list,-1);
    } /* if */
  } /* for */
#endif


  /* load standard libraries */
  amx_Register(amx,core_Natives,-1);
  core_Init();
  amx_Register(amx,console_Natives,-1);
  amx_Register(amx,dll_Natives,-1);
	// CEM - 03/05/01 - Rope's string functions
    amx_Register(amx,string_Natives,-1);
	///CEM
  //err = amx_Register(amx,fixed_Natives,-1);

  return err;
}



#if 0

#define NUMLINES        30
#define NUMCOLUMNS      80
#define DEFWIDTH        640
#define DEFHEIGHT       480
#define U_PRINT         (WM_USER+1)
#define U_GETCHAR       (WM_USER+2)

static void scrollup(char **lines,int numlines)
{
  int ln;

  for (ln=1; ln<numlines; ln++)
    strcpy(lines[ln-1],lines[ln]);
  lines[numlines-1][0]='\0';
}

#define KEYQUEUE_SIZE 16
static WORD keyqueue[KEYQUEUE_SIZE];
static int key_start, key_end;

long CALLBACK _export AMXConsoleFunc(HWND hwnd,unsigned message,WPARAM wParam,
                                     LPARAM lParam)
{
static char **lines;
static int x,y;
static HFONT hfont;
static BOOL showcaret;
  PAINTSTRUCT ps;
  RECT rect;
  char *line;
  int ln,height;
  WORD key;

  switch (message) {
  case WM_CHAR:
    /* store in a queue */
    if ((key_end+1)%KEYQUEUE_SIZE==key_start) {
	MessageBeep(MB_OK);
      break;
    } /* if */
    keyqueue[key_end]=(WORD)wParam;
    key_end=(key_end+1)%KEYQUEUE_SIZE;
    break;

  case WM_CREATE:
    /* allocate memory for the lines */
    lines=(char **)malloc(NUMLINES*sizeof(char *));
    if (lines==NULL) {
      DestroyWindow(hwnd);
      break;
    } /* if */
    for (y=0; y<NUMLINES; y++)
      lines[y]=NULL;    /* preset */
    for (y=0; y<NUMLINES; y++) {
      lines[y]=(char *)malloc((NUMCOLUMNS+1)*sizeof(char));
      if (lines[y]==NULL) {
        DestroyWindow(hwnd);
        break;
      } /* if */
      lines[y][0]='\0';
    } /* for */
    /* create a (monospaced) font */
    hfont=CreateFont(16, 0, 0, 0, FW_NORMAL, FALSE, 0, 0,
                     DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                     CLIP_CHARACTER_PRECIS, PROOF_QUALITY,
                     FIXED_PITCH|FF_DONTCARE, "Courier New");
    /* set initial cursor position */
    x=y=0;
    /* create a caret */
    GetClientRect(hwnd,&rect);
    CreateCaret(hwnd,NULL,rect.right/NUMCOLUMNS,2);
    showcaret=FALSE;
    key_start=key_end=0;
    break;

  case WM_DESTROY:
    if (lines!=NULL) {
      for (y=0; y<NUMLINES; y++)
        if (lines[y]!=NULL)
          free(lines[y]);
      free(lines);
    } /* if */
    if (hfont!=0)
      DeleteObject(hfont);
    DestroyCaret();
    break;

  case WM_PAINT:
    if (showcaret)
      HideCaret(hwnd);
    GetClientRect(hwnd,&rect);
    height=rect.bottom/NUMLINES;
    BeginPaint(hwnd, &ps);
    hfont=SelectObject(ps.hdc,hfont);
    for (ln=0; ln<NUMLINES; ln++) {
      SetRect(&rect,0,ln*height,rect.right,(ln+1)*height);
      ExtTextOut(ps.hdc,0,ln*height,ETO_OPAQUE|ETO_CLIPPED,&rect,
                 lines[ln],strlen(lines[ln]),NULL);
    } /* for */
    hfont=SelectObject(ps.hdc,hfont);
    EndPaint(hwnd, &ps);
    if (showcaret)
      ShowCaret(hwnd);
    break;

  case WM_SIZE:
    if (hfont!=0)
      DeleteObject(hfont);
    /* make a new font, redraw everything */
    GetClientRect(hwnd,&rect);
    hfont=CreateFont(rect.bottom/NUMLINES, rect.right/NUMCOLUMNS,
                     0, 0, FW_NORMAL, FALSE, 0, 0, DEFAULT_CHARSET,
                     OUT_DEFAULT_PRECIS, CLIP_CHARACTER_PRECIS, PROOF_QUALITY,
                     FIXED_PITCH|FF_DONTCARE, "Courier New");
    InvalidateRect(hwnd,NULL,FALSE);
    if (showcaret)
      SetCaretPos((rect.right/NUMCOLUMNS)*x,(rect.bottom/NUMLINES)*(y+1)-2);
    break;

  case U_PRINT:
    line=(char *)lParam;
    while (*line!='\0') {
      assert(x<NUMCOLUMNS);
      assert(y<NUMLINES);
      switch (*line) {
      case '\b':
        if (x>0)
          x--;
        break;
      case '\t':
        if ((x/8+1)*8 < NUMCOLUMNS) {
          while (x%8!=0)
            lines[y][x++]=' ';
          lines[y][x]='\0';
        } /* if */
        break;
      case '\n':
        x=0; y++;
        break;
      case '\x1b':
        // ??? parse the text output, support ANSI escape sequences used by the debugger
        break;
      default:
        lines[y][x++]=*line;
        lines[y][x]='\0';
      } /* if */
      line++;
      /* handle wrap-around and scrolling */
      if (x>=NUMCOLUMNS) {
        x=0; y++;
      } /* if */
      if (y>=NUMLINES) {
        scrollup(lines,NUMLINES);
        y--;
        assert(y<NUMLINES);
      } /* if */
    } /* while */
    InvalidateRect(hwnd,NULL,FALSE);
    UpdateWindow(hwnd);
    break;

  case U_GETCHAR:
    showcaret=TRUE;
    ShowCaret(hwnd);
    GetClientRect(hwnd,&rect);
    SetCaretPos((rect.right/NUMCOLUMNS)*x,(rect.bottom/NUMLINES)*(y+1)-2);
    while (key_start==key_end) {
      MSG msg;
      if (PeekMessage(&msg,NULL,0,0,PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
      } /* if */
    } /* while */
    showcaret=FALSE;
    HideCaret(hwnd);
    key=keyqueue[key_start];
    key_start=(key_start+1)%KEYQUEUE_SIZE;
    return key;

  default:
    return DefWindowProc(hwnd,message,wParam,lParam);
  } /* switch */
  return 0L;
}

static BOOL InitWindowClass(void)
{
  WNDCLASS wc;

  wc.style=CS_GLOBALCLASS|CS_NOCLOSE;
  wc.lpfnWndProc=(WNDPROC)AMXConsoleFunc;
  wc.cbClsExtra=0;
  wc.cbWndExtra=0;
  wc.hInstance=hinstAMX;
  wc.hIcon=LoadIcon(hinstAMX,"amxdll");
  wc.hCursor=LoadCursor(NULL, IDC_ARROW);
  wc.hbrBackground=(HBRUSH)(COLOR_WINDOW+1);
  wc.lpszMenuName=(LPSTR)NULL;
  wc.lpszClassName="AMX_console";
  return RegisterClass(&wc);
}

static BOOL MakeConsoleWindow(void)
{
  if (!IsWindow(hwndConsole)) {
    RECT rect;
    SetRect(&rect,0,0,DEFWIDTH,DEFHEIGHT);
    AdjustWindowRect(&rect,WS_OVERLAPPEDWINDOW,FALSE);
    hwndConsole=CreateWindow("AMX_console","Small console",WS_OVERLAPPEDWINDOW|WS_VISIBLE,
                             CW_USEDEFAULT,CW_USEDEFAULT,rect.right-rect.left,rect.bottom-rect.top,
                             HWND_DESKTOP,NULL,hinstAMX,NULL);
  } /* if */
  return IsWindow(hwndConsole);
}

//#pragma argsused
int amx_fflush(void *stream)
{
  return 0;
}

void amx_putchar(int ch)
{
  char str[]={0,0};
  str[0]=(char)ch;
  if (MakeConsoleWindow())
    SendMessage(hwndConsole,U_PRINT,0,(LPARAM)(LPSTR)str);
}

// this doesn't get declared in stdio.h if _WINDOWS is defined
// although it is in the Windows libraries!

/*
int vsprintf(char *, const char *, va_list);

int amx_printf(const char * strFmt, ...)
{
  va_list marker;
  int len=0;

  if (MakeConsoleWindow()) {
    char str[1024]={0};
    va_start(marker,strFmt);
    len = vsprintf(str,strFmt,marker);
    SendMessage(hwndConsole,U_PRINT,0,(LPARAM)(LPSTR)str);
  } // if
  return len;
}
*/

int amx_getch(void)
{
  if (MakeConsoleWindow())
    return (int)SendMessage(hwndConsole,U_GETCHAR,0,0L);
  return 0;
}

#if 0
/* enum timezone
 *     {
 *     localtime,  // local time
 *     gmtime      // Greenwich mean time
 *     }
 * time(&hour, &minute, &second, timezone:timezone=local);
 * date
 * native timestring(string[], timezone:timezone=localtime, bool:pack=false);
 * datestring
 */
static cell AMX_NATIVE_CALL _timestring(AMX *amx,cell *params)
{
  time_t t;
  struct tm *ct;
  cell *cptr;

  t = time(NULL);
  if (params[2]!=0)
    ct = gmtime(&t);
  else
    ct = localtime(&t);
  amx_GetAddr(amx,params[1],&cptr);
  amx_SetString(cptr,asctime(area),(int)params[3]);
}
#endif

/* enum { Ok, Okcancel, okCancel, Yesno, yesNo, Yesnocancel, yesNocancel, yesnoCancel }
 * enum { noicon, information, exclamation, question, stop }
 * messagebox(message[], caption[], buttons=Ok, icons=noicon, timeout=0)
 */
static cell AMX_NATIVE_CALL _messagebox(AMX *amx,cell *params)
{
static long btn_style[] = { MB_OK, MB_OKCANCEL, MB_YESNO, MB_RETRYCANCEL, MB_ABORTRETRYIGNORE };
static long icon_style[] = { MB_ICONINFORMATION, MB_ICONEXCLAMATION, MB_ICONQUESTION, MB_ICONSTOP };
  char message[256], caption[128];
  long style;
  cell *cptr;
  int len;
  HWND hwnd;

  amx_GetAddr(amx,params[1],&cptr);
  amx_StrLen(cptr,&len);
  if (len<256)
    amx_GetString(message,cptr);
  else
    lstrcpy(message,"(message too long)");

  amx_GetAddr(amx,params[2],&cptr);
  amx_StrLen(cptr,&len);
  if (len<128)
    amx_GetString(caption,cptr);
  else
    lstrcpy(caption,"(caption too long)");

  style=0;
  if (params[3]>=0 && params[3]<5)
    style|=btn_style[(int)params[3]];
  if (params[4]>=0 && params[4]<4)
    style|=btn_style[(int)params[4]];

  /* remove previous messagebox */
  if ((hwnd=FindWindow("#32770",caption))!=0)
    EndDialog(hwnd,IDCANCEL);

  return MessageBox(GetFocus(),message,caption,style);
}

#endif // (if 0)

#if defined(__WIN32__)

  #if defined(__WATCOMC__)
    #define DllEntryPoint(h,dw,lp)      LibMain(h,dw,lp)
  #endif

//#pragma argsused
#ifdef WIN32
BOOL WINAPI DllEntryPoint(HINSTANCE hinstDLL, DWORD dwReason, LPVOID lpRes)
{
  switch (dwReason) {
  case DLL_PROCESS_ATTACH:
    hinstAMX=hinstDLL;
      //InitWindowClass();  /* console window class */
    break;
  case DLL_PROCESS_DETACH:
    UnregisterClass("AMX_console",hinstAMX);
    break;
  } /* switch */
  return TRUE;
}

#endif// ( WIN32)

#endif
