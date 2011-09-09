#!/bin/bash
export OS="Linux"

# The below is only valid in TI Germantown network and should really not be here
#export C6X_BASE_DIR=/apps/ti/cgt/TI_CGT_C6000_6.1.12
#export C6X_BASE_DIR=/apps/ti/cgt/C6000CGT7.2.0

if [ -z "$C6X_BASE_DIR" ]; then
	for dir in {~,}/opt/ti/ccsv5/tools/compiler/c6000 ; do
		if [ -x $dir/bin/cl6x ]; then
			C6X_BASE_DIR=$dir
			break
		fi
	done
fi

if [ ! -x $C6X_BASE_DIR/bin/cl6x ] ; then
	echo "You must define the C6X_BASE_DIR to point to TI CGT compiler for C6000"
	exit 2
fi

#make sure its exported
export C6X_BASE_DIR

export PATH=$C6X_BASE_DIR/bin:$PATH
export TOOLSC6X=$C6X_BASE_DIR
export TOOLSC6XDOS=$C6X_BASE_DIR
export TOOLSBIOSC6XDOS=$C6X_BASE_DIR
