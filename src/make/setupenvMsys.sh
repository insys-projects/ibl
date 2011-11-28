#!/bin/bash

# Environment setup to be done if using MSYS Bash shell for build

# Specify the base directory of the c6000 compiler with UNIX style path separator
export C6X_BASE_DIR='"C:/Program Files/Texas Instruments/C6000 Code Generation Tools 7.2.4"'

# Specify the base directory of the c6000 compiler in format understandable by the MSYS Bash shell 
export C6X_BASE_DIR_MSYS=/c/Program\ Files/Texas\ Instruments/C6000\ Code\ Generation\ Tools\ 7\.2\.4

# Don't modify the below variables. They are derived from the above definitions 
export PATH=$PATH:$C6X_BASE_DIR_MSYS/bin
export TOOLSC6X=$C6X_BASE_DIR
export TOOLSC6XDOS=$C6X_BASE_DIR

