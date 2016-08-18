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

:: turn long filenames on for the FOR loop
lfnfor on

:: I hate stupid error messages. If the compile
:: directory already exists, don't tell me about it.
@ctty nul
mkdir binaries
ctty con

if exist output.txt del output.txt
echo Compiling all *.sma files in:
cd
echo.
FOR %%I IN (*.SMA) DO ..\COMPILER\SC.EXE -i..\INCLUDE %%I -obinaries\%%I >> output.txt
@ctty nul
copy binaries\*.sma binaries\*.amx
del binaries\*.sma
ctty con
echo Complete.
type output.txt |more
goto veryend


:::::::::::::::::::::::::::::::::::::::::::::::
:: If windows NT or w2k, then do this version
:DoWinNT
:::::::::::::::::::::::::::::::::::::::::::::::

if exist output.txt del output.txt
if not exist binaries mkdir binaries
echo Compiling all *.sma files in:
cd
echo.
for %%I in (*.sma) do call :compile_sub %%~nI
echo.
for %%I in (*.sma) do call :print_error %%~nI
echo.
echo Complete.
pause
exit

:compile_sub
echo Compiling %1.sma...
..\compiler\sc -i..\include -e%1_error.txt -obinaries\%1.amx %1
goto :EOF

:print_error
if exist %1_error.txt type %1_error.txt
goto :EOF
echo %1

:veryend