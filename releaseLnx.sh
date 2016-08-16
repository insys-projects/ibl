#!/bin/sh
# Generate the source release tarballs
export IBL_VERSION="1_0_0_17"
cd ../
if [ -f ibl_src_$IBL_VERSION.tgz ]; then rm ibl_src_$IBL_VERSION.tgz; fi

tar -czf ibl_src_$IBL_VERSION.tgz ibl/src ibl/doc

cd ibl
# Create directories for binary builds
if [ -d ibl_bin_$IBL_VERSION ]; then rm -r ibl_bin_$IBL_VERSION; fi

mkdir ibl_bin_$IBL_VERSION
mkdir ibl_bin_$IBL_VERSION/c6678/
mkdir ibl_bin_$IBL_VERSION/c6678/le

# Copy i2cConfig GEL file
cp -f src/util/i2cConfig/i2cConfig.gel ibl_bin_$IBL_VERSION/i2cConfig.gel

# Set Build environment
source src/make/setupenvLnx.sh

export I2C_BUS_ADDRESS=0x50

# Set INSYS_BOARD for choose board depending sources
export INSYS_BOARD=INSYS_UNKNOWN
#export INSYS_BOARD=INSYS_PEX_SRIO
#export INSYS_BOARD=INSYS_AC_DSP
#export INSYS_BOARD=INSYS_FMC110P
#export INSYS_BOARD=INSYS_FMC111P
#export INSYS_BOARD=INSYS_FMC112CP
#export INSYS_BOARD=INSYS_FMC114V
#export INSYS_BOARD=INSYS_FMC116V
#export INSYS_BOARD=INSYS_FMC117CP
#export INSYS_BOARD=INSYS_ABC
#export INSYS_BOARD=INSYS_FM408C
#export INSYS_BOARD=INSYS_FM408C_1G
#export INSYS_BOARD=INSYS_FM408C_2G
#export INSYS_BOARD=INSYS_DSP6678PEX

export INSYS_CPU=CPU0
#export INSYS_CPU=CPU1

#
# Prepare C-source file with git repo information to revison control
#
git_info=`git log --pretty=format:"%h - %cd" -n 1 | cut -c 1-34`

echo "" > ./src/main/iblinfo.c
echo "#include \"ibl.h\"" >> ./src/main/iblinfo.c
echo "" >> ./src/main/iblinfo.c
echo "iblInfo_t iblInfo = {" >> ./src/main/iblinfo.c
echo " \"commit: \"," >> ./src/main/iblinfo.c
echo -n " \"" >> ./src/main/iblinfo.c
echo -n ${git_info} >> ./src/main/iblinfo.c
echo "\"," >> ./src/main/iblinfo.c
echo "};" >> ./src/main/iblinfo.c
echo "" >> ./src/main/iblinfo.c

# Build c6678 EVM LE
pushd src/make
make clean
make evm_c6678_i2c ENDIAN=little I2C_BUS_ADDR=${I2C_BUS_ADDRESS} INSYS_BOARD=${INSYS_BOARD} INSYS_CPU=${INSYS_CPU}
popd
cp -f src/make/bin/i2crom_${I2C_BUS_ADDRESS}_c6678_le.bin ibl_bin_$IBL_VERSION/c6678/le/
cp -f src/make/bin/i2cparam_${I2C_BUS_ADDRESS}_c6678_le_0x500.out ibl_bin_$IBL_VERSION/c6678/le/

# Create Tar archive for binary package
tar -czf ibl_bin_$IBL_VERSION.tgz ibl_bin_$IBL_VERSION

# Configure ibl image
pushd src/util/iblConfig/build
./iblconfig.sh
popd
