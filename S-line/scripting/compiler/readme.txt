Introduction
------------
Important note:
  If you have previously used the Abstract Machine (AMX), you will find that
  the new version has one parameter less for amx_Init(). That is easy to fix
  in your code. Note, however, that you MUST set the AMX structure variable
  to zero before passing it to amx_Init(). This is a new requirement.

Small is a simple, typeless, 32-bit extension language with a C-like syntax.
The Small compiler outputs P-code (or bytecode) that subsequently runs on an
abstract machine. Execution speed, stability, simplicity and a small footprint
were essential design criterions for both the language and the abstract
machine.

Through the evolution of the Small toolkit, this README has steadily been
growing, as more and more compilers were tested and more components were added.
Below is a list of topics that this README covers, in this order:

o  Acknowledgements
   For components not written by CompuPhase

o  Compiling the software, general notes

o  Compiling the Abstract Machine
   Examples for various compilers, on how to include the "AMX" sources in your
   project. This section does *not* cover the use of the AMX DLL or the build
   of the DLL.

o  Compiling the Small compiler
   Notes on building the P-code compiler.

o  Using the AMX DLL
   How to create a program that uses the pre-built DLL.

o  Building the AMX DLL
   Notes on how the DLL must be built itself.

o  Building extension modules for the AMX DLL


Acknowledgements
----------------
This work is based on the "Small C Compiler" by Ron Cain and James E. Hendrix,
as published in the book "Dr. Dobb's Toolbook of C", Brady Books, 1986.

The assembler version of the abstract machine (five times faster than the ANSI
C version and over two times faster than the GNU C version) was written by
Marc Peter (mrp@rz.uni-jena.de). Marc holds the copyright of the file
AMXEXEC.ASM, but its distribution and license fall under the same conditions
as those stated in LICENSE.TXT.

The Just-In-Time compiler (JIT) included in this release was also written by
Marc Peter. As is apparent from the source code, the JIT is an evolutionary
step from his earlier abstract machine. The JIT falls under the same (liberal)
license as the other components in the Small toolkit.

Greg Garner from Artran Inc. (gmg@artran.com) compiled the source files as C++
files (rather than C), added type casts where needed and fixed two bugs in the
Small compiler in the process. Greg Garner also wrote (and contributed) the
extension module for floating point support.

Dark Fiber contributed an extension module for the AMX to provide decent
quality pseudo-random numbers, starting with any seed. To illustrate the
module, he also wrote a simple card game (TWENTY1.SMA) as a non-trivial
small script.


Compiling the software, general notes
-------------------------------------
The C sources contain sections of code that is conditionally compiled. See
your compiler manual how to specify options on the command line or in a
"project" to set these options.

Both the compiler and the abstract machine contain assertions to help me
catch bugs while maintaining the code. In the retail versions, you will want
to compile without assertions, because this code slows down the operation of
the tools. To do so, compile the compiler or the abstract machine with the
NDEBUG definition set.

The "default" build for the tools is for DOS/Windows. To compile for Linux,
add the macro definition LINUX on the compiler's command line.

I personally use "makefiles" and the command line utilities of compilers. It
is partly because of my inexperience with IDE's and "developer studio's", and
partly for the sake of brevity that I only give compiling examples with the
command line tools.

Note: the precompiled versions of the software are 32-bit Win32 console
programs. The abstract machine programs (SRUN.EXE and SDBG.EXE) use
Marc Peter's assembler interface.


Compiling the Abstract Machine
------------------------------
Compiling the AMX is fairly easy, because it was set up to be small and
portable. The abstract machine was intended to be linked with an application.
The file SRUN is the most primitive application that I could come up with. It
runs a compiled Small program from the command line. All of the example "Small"
programs run with SRUN.

Below are a few examples of compiler options. All these configurations have
been tested. They should work at your system if you use exactly the same
command line parameters. The compilers used are:
o  Microsoft Visual C/C++ versions 5.0 and 6.0
o  Borland C++ versions 3.1 (16-bit) and 5.02
o  Watcom C/C++ versions 10.6 and 11.0
o  GNU C version 2.8.1 (DJGPP 2.01)

1. ANSI C
---------
   Borland C++ version 3.1, 16-bit:
        bcc srun.c amx.c amxcore.c amxcons.c

   Microsoft Visual C/C++ version 5.0 or 6.0, 32-bit:
        cl srun.c amx.c amxcons.c amxcore.c

        (note: when running with warning level 4, option "-W4", Visual C/C++
        issues a few warnings for unused function arguments and changes of the
        default structure alignment)

   Watcom C/C++ version 11.0, 32-bit:
        wcl386 /l=nt srun.c amx.c amxcore.c amxcons.c


2. Assembler core for the virtual machine
-----------------------------------------
    Marc Peter's assembler implementation of the abstract machine currently
    runs with Watcom C/C++ version 10.5 and later. It is five times faster
    than the ANSI C version. As you can see on the command line, the C files
    need the ASM32 macro to be defined.

    There are two "calling convention" issues in the assembler implementation:
    o  The convention with which amx_exec_asm() itself is called. The default
       calling convention is Watcom's register calling convention. For other
       compilers, change this to __cdecl by setting the macro STACKARGS.
    o  The convention for calling the "hook" functions (native functions and
       the debugger callback). Again, the default is Watcom's register calling
       convention. Use the macros CDECL or STDECL for __cdecl and __stdcall
       respectively. (Since STDCALL is a reserved word on the assembler, I had
       to choose a different name for the macro, hence STDECL.)


   Borland C++ version 5.02, 32-bit:
        bcc32 -DASM32 -TdCDECL -TdSTACKARGS srun.c amx.c amxcore.c amxcons.c amxexec.asm

        (note: You must assemble AMXEXEC.ASM with the "CDECL" and "STACKARGS"
        options. The "-T" compiler option passes what follows on the TASM32.)

   Microsoft Visual C/C++ version 5.0 or 6.0, 32-bit:
        ml /c /DCDECL /DSTACKARGS /Cx /coff amxexec.asm
        cl -Gd -DASM32 srun.c amx.c amxcons.c amxcore.c amxexec.obj

        (note: Microsoft appears to use __cdecl calling convention by default,
        but I have forced the calling convention to be sure: option -Gd.)

   Microsoft Visual C/C++ version 5.0 or 6.0, 32-bit:
        ml /c /DSTDECL /DSTACKARGS /Cx /coff amxexec.asm
        cl -Gz -DASM32 -DAMXAPI=__stdcall srun.c amx.c amxcons.c amxcore.c amxexec.obj

        (note: Option -Gz forces __stdcall calling convention. The assembler
        file now uses STDECL (for __stdcall) too.)

   Watcom C/C++ version 11.0, 32-bit:
        wcl386 /l=nt /dASM32 srun.c amx.c amxcore.c amxcons.c amxexec.asm

        (note: Watcom C/C++ uses register calling convention, which is fastest
        in this case.)


3. Just-In-Time compiler
------------------------
   The third option for Watcom C/C++ is to add the Just-In-Time compiler, plus
   support routines. The JIT compiles the P-code of the AMX to native machine
   code at run-time. The resulting code is more than twice as fast as the
   assembler version of the abstract machine (which was pretty fast already).
   To add support for the JIT, you must define the macro "JIT" via a command
   line switch.

   There are, in fact, two versions of the JIT: JITR.ASM uses register based
   calling conventions and requires Watcom C/C++. JITS.ASM uses __cdecl
   calling convention (stack based) and should work with other Win32
   compilers. Apart from the calling conventions, both JIT versions are
   identical.

   The source files JITR.ASM and JITS.ASM (for the Just-In-Time compiler)
   contain several definitions with which you can trade performance for other
   options (like support for some debug opcodes).

   Note that the JIT is not completely up to date with the Small compiler. It
   does not support the "range" opcode, for example. So for successful use,
   you should compile your Small programs without symbolic information.


   Borland C++ version 5.02, 32-bit:
        bcc32 -DJIT -Tm2 srun.c amx.c amxcore.c amxcons.c jits.asm

        (note: You must force TASM to use at two passes, so that forward
        references are resolved. The -Tm2 option accomplishes this.)

   Watcom C/C++ version 11.0, 32-bit:
        wcl386 /l=nt /dJIT srun.c amx.c amxcore.c amxcons.c jitr.asm

        (note: Watcom C/C++ uses register calling convention, which is fastest
        in this case.)


4. Direct threaded interpreter with GNU C extensions
----------------------------------------------------
   The AMX.C file has special code for the GNU C compiler (GCC), which makes
   the abstract machine about twice as fast as the ANSI C version. However,
   the assembler core and the JIT are faster still.


   GNU GCC for DOS/Windows (DJGPP version 2.01), 32-bit:
        gcc -o srun.exe srun.c amx.c amxcore.c amxcons.c

        (note: when running with all warnings enabled, option "-Wall", the GNU
        C compiler suggests extra parantheses around subexpressions in a
        logical expression; I do not agree with the desirability of extra
        parantheses in this case, so I have not added them)


   GNU GCC for Linux
        gcc -o srun -DLINUX -I../linux srun.c amx.c amxcore.c amxcons.c


All compiling examples (listed above) have as few command line options as
needed. Consult the compiler documentation to add debugging information or to
enable optimizations. The source files also contain "assertions" at various
places. Again, consult the compiler documentation to remove the assertions
(and get a modest performance gain).

The "core" AMX routines do not use or depend on dynamic memory allocation or
console IO. The "property" functions in AMXCORE.C use malloc()/free(); you
can remove these property set/retrieval functions by compiling the AMXCORE.C
file with the definition NOPROPLIST.

The console IO functions in AMXCONS.C try to use standard C as much as
possible, but some of the functions also rely on ANSI terminal codes (ANSI.SYS
under DOS). In any case, in a "real" environment, you will probably want to
replace AMXCONS.C with an alternative that fits your application or operating
system.

Greg Garner added a floating point package to th AMX. The files are FLOAT.CPP
(to compile and link with the AMX) and FLOAT.INC. This addition required a
change in AMXCONS.C as well (to print out floating point numbers). If you do
not want floating point support in AMXCONS.C, define the macro NOFLOAT.
The files FLOAT.CPP and FLOAT.INC are copyright (c) Artran, Inc. 1999.

The abstract machine provides hooks for debugging support. These hooks slow
down the execution of a compiled program somewhat. For optimal performance
and a slightly smaller abstract machine, strip off the debugging support by
defining the macro NODBGCALLS.

Calling conventions are always an important issue in porting software. The
Small AMX specifies the calling convention it uses via two macros. These
macros are blank by default, in order to stay as close to ANSI C as possible.
By (re-)defining either (or both) of these macros, you can adjust the calling
conventions:
    AMX_NATIVE_CALL     The calling convention for the native functions and for
                        the debugger callback. You may want to set this to
                        __stdcall when compiling for Win32.
    AMXAPI              The calling convention used for all interface
                        functions of the abstract machine (e.g. amx_Init()).
                        You need to change this if you put the AMX in a
                        Windows DLL, for example.

Summary of definitions
    AMX_NATIVE_CALL     calling convention of native function
    AMXAPI              calling convention of interface functions
    ASM32               compile the assembler version
    JIT                 add support for the Just-In-Time compiler
    LINUX               compile for Linux (or perhaps other Unix versions)
    NDEBUG              compile without assertions
    NODBGCALLS          remove the debugger support
    NOFLOAT             remove floating point support in AMXCONS.C
    NOPROPLIST          remove the get/set property functions from AMXCORE.C


Compiling the Small compiler
----------------------------
The Small language is case sensitive, but you can make a case insensitive
version by adding the definition NO_CASE_SENSITIVE.

The compiler is a stand-alone program. If you want to link it to an
application, can compile the sources with the macro definition NO_MAIN. This
will strip the "main()" function from the program. To compile a source file,
you now call "sc_main(int argc,char **argv)". As you can see, this function
looks like the standard main(); when calling sc_main(), you must fill in an
array of arguments, including argv[0] (because the compiler constructs the
path for the include files from the path/filename in argv[0]).

If you want a Small compiler that outputs 16-bit P-code, add the definition
BIT16 to the compiler options. Note that this is unrelated to whether the
compiler itself is a 16-bit or a 32-bit executable program. The header file
boldly assumes that a "short int" is 16-bits and a "long int" is 32-bits. If
this is not true for your compiler, you must change the definition of the cell
type in SC.H, but you must also check the locations where there is an "#if
defined(BIT16)" around some code, because I use the constants SHORT_MAX and
LONG_MAX from LIMITS.H as well.
N.B. I have never tested, or even tried to compile, a 16-bit version of
     any of the Small tools.

The basic code generation is followed by a simple peephole optimizer. If you
stumble on a code generation bug, one of the first things that you may want
to find out is whether this bug is in the code generation or in the optimizer.
For that purpose, you can compile the compiler without peephole optimizer,
using the definition NO_OPTIMIZE.

In Small, one creates symbolic constants with the "const" keyword. But Small
also supports "#define" to provide some level of compatibility with C or other
C like tools. The "#define" directive of Small can only declare numeric
constants, it is much more limited than C's #define. Therefore, may may find
this "portability" clumsy and rather confusing. If so, compiling with option
NO_DEFINE removes the support for the #define directive in the Small compiler.

I have compiled and tested the compiler with:
* Borland C++ 3.1, 16-bit in small and large memory models
* Microsoft C 6.0, 16-bit in small and large memory models
* Watcom C/C++ 11.0, 32-bit flat mode (Executable for Windows NT)
* GNU GCC, 32-bit executable for Linux
Both 16-bit compilers are old, but this should not matter. The code is
intended to be ANSI C as much as possible.

Summary of definitions
    BIT16               cell is 16-bits, rather than 32-bits
    LINUX               compile for Linux (or perhaps other Unix versions)
    NDEBUG              compile without assertions
    NO_MAIN             remove main() function from the program
    NO_CASE_SENSITIVE   set compiler to case insensitive
    NO_DEFINE           remove support for #define directive
    NO_OPTIMIZE         remove the peephole optimizer step


Using the AMX DLL
-----------------
The 32-bit AMX DLL (file AMX32.DLL) uses __stdcall calling convention, which
is the most common calling convention for Win32 DLLs. If your compiler defaults
to a different calling convention (most do), you must specify the __stdcall
calling convention explicitly. This can be done in two ways:
1. a command line option for the C/C++ compiler (look up the manual)
2. setting the macros AMX_NATIVE_CALL and AMXAPI to __stdcall before including
   AMX.H. The macros AMX_NATIVE_CALL and AMXAPI are explained earlier in this
   README.

The 32-bit AMX DLL comes with import libraries for various Win32 compilers:
o  for Microsoft Visual C/C++ version 4.0 and above, use AMX32M.LIB
o  for Borland C++ version 5.0 and for Borland C++ Builder, use AMX32B.LIB
o  for Watcom C/C++ version 10.6 and 11.0, use AMX32W.LIB

The AMX DLL already includes "core" and "console" functions, which are the
equivalents of the C files AMXCORE.C and AMXCONS.C. Console output goes to a
text window (with 30 lines of 80 characters per line) that the DLL creates.
The core and console functions are automatically registered to any Small
program by amx_Init().

   Watcom C/C++ version 11.0, 32-bit:
        wcl386 /l=nt_win /dAMXAPI=__stdcall srun-dll.c amx32w.lib

        (Note: the "srun-dll" example does not register additional native
        functions. Therefore, AMX_NATIVE_CALL does not need to be defined.)

   Borland C++ version 3.1, 16-bit:
        bcc -DAMXAPI=__cdecl -W -ml srun-dll.c amx16.lib

        (Note: the program must be compiled as a Win16 application, because
        only Windows programs can use DLLs (option -W). Using large memory
        model, option -ml, is not strictly required, but it is the most
        convenient. Finally, note that the 16-bit DLL uses __cdecl calling
        convention for its exported functions, for reasons explained below.)


Building the AMX DLL
--------------------
The 32-bit DLL is made from the files AMX.C, AMXDLL.C, AMXCONS.C, AMXCORE.C
and AMXEXEC.ASM.

The first point to address is, again, that of calling conventions. AMXAPI and
AMX_NATIVE_CALL must be set to __stdcall. I did this by adding the macros onto
the command line for the compiler, but you could also create an include file
with these macros before including AMX.H.

The function amx_Init() of the DLL is not identical to the standard version:
it also registers the "core" and "console" modules and registers external
libraries if it can find these. The original amx_Init() in AMX.C is renamed to
amx_InitAMX(), via macros, at compile time. AMXDLL.C implements the new
amx_Init() and this one is not translated.

All in all, the makefile for the AMX DLL plays a few tricks with macros in
order to keep the original distribution untouched. When you need to recompile
the AMX DLL, you may, of course, also opt for modifying AMX.H and AMX.C to
suit the needs for Win32 DLLs.

If you rebuild the DLL for 16-bit Windows, keep the following points in mind:
o  You must use the ANSI C version of the abstract machine; there is no
   16-bit assembler implementation.
o  Use large memory model: pointers used in "interface" functions must be far
   pointers to bridge the data spaces of the .EXE and the DLL. The source
   code is (mostly) ANSI C, however, and "far pointers" are an extension to
   ANSI C. The easiest way out is to make all pointers "far" by using large
   memory model.
o  AMX_NATIVE_CALL are best set to "__far __pascal". This is the "standard"
   calling convention for have interface functions in 16-bit Windows.
o  The native functions should also be exported, so that the data segment is
   set to that of the module that the native functions reside in.
o  AMXAPI, however, must be set to "__cdecl", because amx_Exec() uses a
   variable length argument list. This is incompatible with the "pascal"
   calling convention.

The distribution for the AMX DLL comes with two makefiles: the makefile for the
32-bit DLL is for Watcom C/C++ and the makefile for the 16-bit DLL is for
Borland C++ (Inprise).


Building extension modules for the AMX DLL
------------------------------------------
The AMX DLL can automatically register the native functions that reside inside
"extension DLLs". The AMX DLL does this for libraries that are listed in the
compiled Small program with the "#pragma library ..." instruction.

The extension DLL must already be loaded in memory. The AMX DLL skips libraries
that are not loaded. The extension DLL must furthermore have an exported
function with the declaration:

   AMX_NATIVE_INFO FAR *amx_NativeList(void);

which must return a pointer to the list of native functions in the extension
DLL. This pointer must be in the format that amx_Register() expects (the list
must be terminated with a NULL pointer).

Refer to the Small manual for general information on native functions and
(statically linked) extension modules.
