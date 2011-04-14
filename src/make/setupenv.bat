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
@REM *
@REM * Copyright (C) 2010, Texas Instruments, Inc.
@REM *****************************************************************************

@echo off

REM  Setup the ibl build environment

REM Modify following lines based on target environment for the toolset installed 
REM Dependency is related to path for Cygwin and CGEN installed

REM PATH=G:\cygwin\bin;C:\PROGRA~1\TEXASI~1\C6000C~1.12\bin

set PATH=G:\cygwin\bin;t:\c6xx\cgen6_1_12\c6000\cgtools\bin
set PATH=%PATH%;%SystemRoot%\system32;%SystemRoot%;
set CYGWINPATH=G:/cygwin/bin
set BISONSKEL=yacc.c
set TOOLSC6X=t:/c6xx/cgen6_1_12/c6000/cgtools
set TOOLSC6XDOS=t:\c6xx\cgen6_1_12\c6000\cgtools

