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

set PATH=G:\cygwin\bin;G:/PROGRA~1/TEXASI~1\ccsv5\tools\compiler\c6000\bin
set PATH=%PATH%;%SystemRoot%\system32;%SystemRoot%;
set CYGWINPATH=G:/cygwin/bin
set BISONSKEL=yacc.c
set TOOLSC6X=G:/PROGRA~1/TEXASI~1/ccsv5/tools/compiler/c6000
set TOOLSC6XDOS=G:\PROGRA~1\TEXASI~1\ccsv5\tools\compiler\c6000

