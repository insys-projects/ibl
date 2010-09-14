@echo off

rem  Setup the ibl build environment


set PATH=t:\gen\gnu\99-11-01\cygwin-b20\H-i586-cygwin32\bin;c:\tools\c6xx\cgen6_1_12\c6000\cgtools\bin
set PATH=%PATH%;%SystemRoot%\system32;%SystemRoot%;t:\ti_pdsp_cgen\20091120

set PERL=//t/gen/perl/activestate/5_6_1_635/bin/perl
set PERLDOS=t:\gen\perl\activestate\5_6_1_635\bin\perl

set CYGWINPATH=//t/gen/gnu/99-11-01/cygwin-b20/H-i586-cygwin32/bin
set TOOLSC6X=c:/tools/c6xx/cgen6_1_12/c6000
set TOOLSC6XDOS=c:\tools\c6xx\cgen6_1_12\c6000\

set TOOLC6XSRC=t:\c6xx\cgen6_1_12\c6000
set TOOLC6XDST=c:\tools\c6xx\cgen6_1_12\c6000

rem splint libraries
set LARCH_PATH=e:\splint-3.1.1\lib

rem Generate path to makedep, then set it 
rm -f mkdeppath.bat
rem sh -c "makedepdir=`pwd`;echo set MAKEDEPPATH=`dirname $makedepdir`/make/makedep/makedep.exe" >mkdeppath.bat
sh -c "makedepdir=`pwd`;echo set MAKEDEPPATH=`pwd`/make/makedep/makedep.exe" >mkdeppath.bat
call mkdeppath.bat


rem ************************* Tools Copy/Validation **************************
rem If the "bypass" option is not provided, validate/copy the tools
rem *********************************************************************
if "%1" == "bypass" goto environment

rem check, but don't copy tools
if "%1" == "" goto toolcheck

rem copy tools if check fails
if "%1" == "enable_write" goto toolcopy

rem error: invalid argument
echo =========================== INVALID ARGUMENT ============================
echo Option "%1" is invalid.  Valid options are:
echo "bypass":       configure environment without checking tools
echo "enable_write": *****ERASE***** all files in %TOOLDST%, 
echo                 and replace with correct tools
echo =========================== INVALID ARGUMENT ============================
goto end

:toolcheck
echo Verifying local tools
goto toolperl

:toolcopy
echo UPDATING local tools
:toolperl
%PERLDOS% make\cpytools\cpytools.pl %TOOLC6XSRC% %TOOLC6XDST% %1
rem note: windows is stupid; "errorlevel 1" means "retval >= 1"
if errorlevel 1 goto error

echo Local tools are good
goto environment


rem ************************** Bad Tools *************************************
rem Tools are bad; deconfigure environment and print error
rem **************************************************************************
:error
set TOOLS=
set TOOLSDOS=
set MAKEDEPPATH=

echo ============================== BAD TOOLS =================================
echo * Build environment is NOT configured.  You may rerun script with:
echo *
echo * %0 bypass       
echo *    configure environment without checking tools.  This option is
echo *    is used when the user will manually configure the tools.
echo *
echo * %0 enable_write 
echo *    [*** DANGEROUS OPTION ***]
echo *    This option 
echo *        1) *** DANGER *** Recursively deletes all files in subdirectories
echo *           %DST365% and %TOOLDST%
echo *        2) Copies correct tools from the subdirectories
echo *           %SRC365% and %TOOLSRC%
echo *               
echo ============================== BAD TOOLS =================================

:environment
:end

