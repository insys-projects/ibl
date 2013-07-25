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

# Build c6678 EVM LE
pushd src/make
make clean
make evm_c6678_i2c ENDIAN=little I2C_BUS_ADDR=0x51 
popd
cp -f src/make/bin/i2crom_0x51_c6678_le.bin ibl_bin_$IBL_VERSION/c6678/le/
cp -f src/make/bin/i2cparam_0x51_c6678_le_0x500.out ibl_bin_$IBL_VERSION/c6678/le/

# Create Tar archive for binary package
tar -czf ibl_bin_$IBL_VERSION.tgz ibl_bin_$IBL_VERSION

# Configure ibl image
pushd src/util/iblConfig/build
./iblconfig.sh
popd
