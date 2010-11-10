@REM ******************************************************************************
@REM * FILE PURPOSE: Environment Setup for building Bootloader
@REM ******************************************************************************
@REM * FILE NAME: setupenv.bat
@REM *
@REM * DESCRIPTION: 
@REM *  Configures and sets up the Build Environment for Bootloader in DOS environment. 
@REM *  
@REM *  Customers are expected to modify this file as per their build environment.
@REM *
@REM * USAGE:
@REM *  setupenv.bat  
@REM *              :bypass       
@REM *                  configure environment without checking tools.  This option is
@REM *                  is used when the user will manually configure the tools.
@REM *  
@REM *              :enable_write 
@REM *      [*** DANGEROUS OPTION ***]
@REM *                  Updates the tool in local drive  
@REM *               
@REM *              :No arguments: Sets the environment for building, Verifies local
@REM *               tools and reports if any tool update is required   
@REM *
@REM * Copyright (C) 2010, Texas Instruments, Inc.
@REM *****************************************************************************

@echo off

REM  Setup the ibl build environment

REM Modify following lines based on target environment for the toolset installed 
REM Dependency is related to path for Cygwin and CGEN installed

set PATH=t:\gen\gnu\99-11-01\cygwin-b20\H-i586-cygwin32\bin;C:\PROGRA~1\TEXASI~1\C6000C~1.12\bin
set PATH=%PATH%;%SystemRoot%\system32;%SystemRoot%;

set PERL=//t/gen/perl/activestate/5_6_1_635/bin/perl
set PERLDOS=t:\gen\perl\activestate\5_6_1_635\bin\perl

set CYGWINPATH=//t/gen/gnu/99-11-01/cygwin-b20/H-i586-cygwin32/bin
set TOOLSC6X=C:/PROGRA~1/TEXASI~1/C6000C~1.12
set TOOLSC6XDOS=C:\PROGRA~1\TEXASI~1\C6000C~1.12

set TOOLC6XSRC=t:\c6xx\cgen6_1_12\c6000\cgtools
set TOOLC6XDST=C:\PROGRA~1\TEXASI~1\C6000C~1.12


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

