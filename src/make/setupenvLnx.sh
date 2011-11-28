#!/bin/bash
export OS="Linux"

if [ -z $C6X_CGT_VERSION ]; then
    C6X_CGT_VERSION=7.2.4
fi

if [ -z "$C6X_BASE_DIR" ]; then
	for dir in {~,}{,/opt}/{ti,TI,texas_instruments}/TI_CGT_C6000_${C6X_CGT_VERSION} {~,}{,/opt}/{ti,texas_instruments}/ccsv5/tools/compiler/c6000 ; do
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
