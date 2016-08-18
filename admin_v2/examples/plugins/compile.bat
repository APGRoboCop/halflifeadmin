:: This file will compile all of the plugins in the examples directory
::  and put the output into the compile directory. It works in both win9x
::  and NT/w2k operating systems.
:: Author: Luke Sankey
:: Date: March 31, 2001

@ECHO OFF

:::::::::::::::::::::::::::::::::::::::::::::::::
:: Determine which version of windows we are on
:: Hopefully we can get most installations with
:: this simple OS check.
:::::::::::::::::::::::::::::::::::::::::::::::::
if "%os%"=="Windows_NT" goto DoWinNT




::::::::::::::::::::::::::::::::::::::::::::::
:: If windows 95, 98, or ME, do this version
:DoWin9x
::::::::::::::::::::::::::::::::::::::::::::::

:: I hate stupid error messages. If the binaries
:: directory already exists, don't tell me about it.
@ctty nul
mkdir binaries
ctty con

echo Compiling %1...
echo.
..\COMPILER\SC.EXE -i..\INCLUDE %1 -obinaries\%1
@ctty nul
copy binaries\*.sma binaries\*.amx
del binaries\%1
ctty con
goto veryend

:::::::::::::::::::::::::::::::::::::::::::::::
:: If windows NT or w2k, then do this version
:DoWinNT
:::::::::::::::::::::::::::::::::::::::::::::::

if not exist binaries mkdir binaries
echo Compiling %1...
..\compiler\sc -i..\include -e%1_error.txt -obinaries\%~n1.amx %1
echo Complete.
if exist %1_error.txt type %1_error.txt

:veryend