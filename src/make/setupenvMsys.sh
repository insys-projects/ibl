#!/bin/bash

# Environment setup to be done if using MSYS Bash shell for build

# Specify the base directory of the c6000 compiler with UNIX style path separator
export C6X_BASE_DIR='"G:/Program Files/Texas Instruments/ccsv5/tools/compiler/c6000"'

# Specify the base directory of the c6000 compiler in format understandable by the MSYS Bash shell 
export C6X_BASE_DIR_MSYS=/g/Program\ Files/Texas\ Instruments/ccsv5/tools/compiler/c6000

# Don't modify the below variables. They are derived from the above definitions 
export PATH=$PATH:$C6X_BASE_DIR_MSYS/bin
export TOOLSC6X=$C6X_BASE_DIR
export TOOLSC6XDOS=$C6X_BASE_DIR

