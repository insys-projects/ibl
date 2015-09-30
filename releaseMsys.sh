#!/bin/sh
# Generate the source release tarballs

# Set Build environment
source ./src/make/setupenvMsys.sh

export I2C_BUS_ADDRESS=0x50

# Set INSYS_BOARD for choose board depending sources
#export INSYS_BOARD=INSYS_UNKNOWN
#export INSYS_BOARD=INSYS_PEX_SRIO
#export INSYS_BOARD=INSYS_AC_DSP
#export INSYS_BOARD=INSYS_FMC110P
export INSYS_BOARD=INSYS_FMC112CP
#export INSYS_BOARD=INSYS_FMC112CP_V11
#export INSYS_BOARD=INSYS_FMC114V
#export INSYS_BOARD=INSYS_FMC116V
#export INSYS_BOARD=INSYS_FMC117CP
#export INSYS_BOARD=INSYS_ABC
#export INSYS_BOARD=INSYS_FM408C_1G
#export INSYS_BOARD=INSYS_FM408C_2G
#export INSYS_BOARD=INSYS_DSP6678PEX

#export INSYS_CPU=CPU0
export INSYS_CPU=CPU1

# Build c6678 EVM LE
pushd ./src/make
make clean
make evm_c6678_i2c ENDIAN=little I2C_BUS_ADDR=${I2C_BUS_ADDRESS} INSYS_BOARD=${INSYS_BOARD} INSYS_CPU=${INSYS_CPU}
popd
cp -f ./src/make/bin/i2crom_${I2C_BUS_ADDRESS}_c6678_le.bin ibl_bin_$IBL_VERSION/c6678/le/
cp -f ./src/make/bin/i2cparam_${I2C_BUS_ADDRESS}_c6678_le_0x500.out ibl_bin_$IBL_VERSION/c6678/le/

# Create Tar archive for binary package
#tar -czf ibl_bin_$IBL_VERSION.tgz ibl_bin_$IBL_VERSION

# Configure ibl image
pushd ./src/util/iblConfig/build
./iblconfig.sh
popd
