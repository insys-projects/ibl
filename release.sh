#!/bin/sh
# Generate the source release tarballs
export IBL_VERSION="1_0_1"
cd ../
if [ -f ibl_src_$IBL_VERSION.tar ]; then rm ibl_src_$IBL_VERSION.tar; fi

tar -cvf ibl_src_$IBL_VERSION.tar ibl/src ibl/doc

cd ibl
# Create directories for binary builds
if [ -d ibl_bin_$IBL_VERSION ]; then rm -r ibl_bin_$IBL_VERSION; fi

mkdir ibl_bin_$IBL_VERSION
mkdir ibl_bin_$IBL_VERSION/c6455/
mkdir ibl_bin_$IBL_VERSION/c6455/le-be

mkdir ibl_bin_$IBL_VERSION/c6472/
mkdir ibl_bin_$IBL_VERSION/c6472/le-be

mkdir ibl_bin_$IBL_VERSION/c6474/
mkdir ibl_bin_$IBL_VERSION/c6474/be
mkdir ibl_bin_$IBL_VERSION/c6474/le

mkdir ibl_bin_$IBL_VERSION/c6474l/
mkdir ibl_bin_$IBL_VERSION/c6474l/be
mkdir ibl_bin_$IBL_VERSION/c6474l/le

mkdir ibl_bin_$IBL_VERSION/c6457/
mkdir ibl_bin_$IBL_VERSION/c6457/be
mkdir ibl_bin_$IBL_VERSION/c6457/le

mkdir ibl_bin_$IBL_VERSION/c6678/
mkdir ibl_bin_$IBL_VERSION/c6678/le

# Set Build environment
source src/make/setupenvLnx.sh

# Build C6455 EVM
pushd src/make
make clean
make evm_c6455 I2C_MAP_ADDR=0x500
popd
# Copy the files to the Binary package
cp -f src/make/ibl_c6455/i2crom.dat ibl_bin_$IBL_VERSION/c6455/le-be/i2crom.dat
cp -f src/util/i2cWrite/i2cWrite_le.out   ibl_bin_$IBL_VERSION/c6455/le-be/
cp -f src/util/i2cConfig/i2cparam_c6455_le.out ibl_bin_$IBL_VERSION/c6455/le-be/i2cparam_c6455_le_0x500.out

# Build C6455 EVM BE
pushd src/make
make clean
make evm_c6455 I2C_MAP_ADDR=0x800
popd
# Copy the files to the Binary package
cp -f src/util/i2cConfig/i2cparam_c6455_be.out ibl_bin_$IBL_VERSION/c6455/le-be/i2cparam_c6455_be_0x800.out

# Copy i2cConfig GEL file
cp -f src/util/i2cConfig/i2cConfig.gel ibl_bin_$IBL_VERSION/i2cConfig.gel

# Build C6472 le
pushd src/make
make clean
make evm_c6472 I2C_MAP_ADDR=0x500
popd
# Copy the files to the Binary package
cp -f src/make/ibl_c6472/i2crom.dat ibl_bin_$IBL_VERSION/c6472/le-be/i2crom.dat
cp -f src/util/i2cWrite/i2cWrite_le.out   ibl_bin_$IBL_VERSION/c6472/le-be/
cp -f src/util/i2cConfig/i2cparam_c6472_le.out ibl_bin_$IBL_VERSION/c6472/le-be/i2cparam_c6472_le_0x500.out


# Build C6472 EVM BE
pushd src/make
make clean
make evm_c6472 I2C_MAP_ADDR=0x800
popd
# Copy the files to the Binary package
cp -f src/util/i2cConfig/i2cparam_c6472_be.out ibl_bin_$IBL_VERSION/c6472/le-be/i2cparam_c6472_be_0x800.out

# Build c6474 EVM
pushd src/make
make clean
make evm_c6474 
popd
# Copy the files to the Binary package
cp -f src/make/ibl_c6474/i2crom_0x50_c6474_le.dat ibl_bin_$IBL_VERSION/c6474/le/
cp -f src/make/ibl_c6474/i2crom_0x50_c6474_be.dat ibl_bin_$IBL_VERSION/c6474/be/
cp -f src/util/i2cWrite/i2cWrite_le.out   ibl_bin_$IBL_VERSION/c6474/le/
cp -f src/util/i2cWrite/i2cWrite_be.out   ibl_bin_$IBL_VERSION/c6474/be/
cp -f src/util/i2cConfig/i2cparam_c6474_le.out   ibl_bin_$IBL_VERSION/c6474/le/
cp -f src/util/i2cConfig/i2cparam_c6474_be.out   ibl_bin_$IBL_VERSION/c6474/be/


# Build c6474l EVM
pushd src/make
make clean
make evm_c6474l
popd
# Copy the files to the Binary package
cp -f src/make/ibl_c6474/i2crom_0x50_c6474l_le.dat ibl_bin_$IBL_VERSION/c6474l/le/
cp -f src/make/ibl_c6474/i2crom_0x50_c6474l_be.dat ibl_bin_$IBL_VERSION/c6474l/be/
cp -f src/util/i2cWrite/i2cWrite_le.out   ibl_bin_$IBL_VERSION/c6474l/le/
cp -f src/util/i2cWrite/i2cWrite_be.out   ibl_bin_$IBL_VERSION/c6474l/be/
cp -f src/util/i2cConfig/i2cparam_c6474_le.out   ibl_bin_$IBL_VERSION/c6474l/le/
cp -f src/util/i2cConfig/i2cparam_c6474_be.out   ibl_bin_$IBL_VERSION/c6474l/be/


# Build c6457 EVM 
pushd src/make
make clean
make evm_c6457 
popd
cp -f src/make/ibl_c6457/i2crom_0x50_c6457_le.dat ibl_bin_$IBL_VERSION/c6457/le/
cp -f src/make/ibl_c6457/i2crom_0x50_c6457_be.dat ibl_bin_$IBL_VERSION/c6457/be/
cp -f src/util/i2cWrite/i2cWrite_le.out   ibl_bin_$IBL_VERSION/c6457/le/
cp -f src/util/i2cWrite/i2cWrite_be.out   ibl_bin_$IBL_VERSION/c6457/be/
cp -f src/util/i2cConfig/i2cparam_c6457_le.out   ibl_bin_$IBL_VERSION/c6457/le/
cp -f src/util/i2cConfig/i2cparam_c6457_be.out   ibl_bin_$IBL_VERSION/c6457/be/

# Build c6678 EVM 
pushd src/make
make clean
make evm_c6678_i2c ENDIAN=little I2C_BUS_ADDR=0x51 
popd
cp -f src/make/ibl_c661x/i2crom.dat ibl_bin_$IBL_VERSION/c6678/le/i2crom_0x51_c6678_le.dat
cp -f src/util/i2cWrite/i2cWrite_le.out   ibl_bin_$IBL_VERSION/c6678/le/
cp -f src/util/i2cConfig/i2cparam_c661x_le.out ibl_bin_$IBL_VERSION/c6678/le/i2cparam_c6678_le.out

# Create Tar archive for binary package
tar -czf ibl_bin_$IBL_VERSION.tgz ibl_bin_$IBL_VERSION

