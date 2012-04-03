#!/bin/sh
# Generate the source release tarballs
export IBL_VERSION="1_0_0_14"
cd ../
if [ -f ibl_src_$IBL_VERSION.tgz ]; then rm ibl_src_$IBL_VERSION.tgz; fi

tar -czf ibl_src_$IBL_VERSION.tgz ibl/src ibl/doc

cd ibl
# Create directories for binary builds
if [ -d ibl_bin_$IBL_VERSION ]; then rm -r ibl_bin_$IBL_VERSION; fi

mkdir ibl_bin_$IBL_VERSION
mkdir ibl_bin_$IBL_VERSION/c6455/
mkdir ibl_bin_$IBL_VERSION/c6455/be
mkdir ibl_bin_$IBL_VERSION/c6455/le

mkdir ibl_bin_$IBL_VERSION/c6472/
mkdir ibl_bin_$IBL_VERSION/c6472/be
mkdir ibl_bin_$IBL_VERSION/c6472/le

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
mkdir ibl_bin_$IBL_VERSION/c6678/be

mkdir ibl_bin_$IBL_VERSION/c6670/
mkdir ibl_bin_$IBL_VERSION/c6670/le
mkdir ibl_bin_$IBL_VERSION/c6670/be

# Copy i2cConfig GEL file
cp -f src/util/i2cConfig/i2cConfig.gel ibl_bin_$IBL_VERSION/i2cConfig.gel

# Set Build environment
source src/make/setupenvLnx.sh

# Build C6455 EVM LE
pushd src/make
make clean
make evm_c6455 ENDIAN=little
popd
# Copy the files to the Binary package
cp -f src/make/bin/i2crom_0x50_c6455_le.bin ibl_bin_$IBL_VERSION/c6455/le/
cp -f src/make/bin/i2cparam_0x50_c6455_le_0x500.out ibl_bin_$IBL_VERSION/c6455/le/

# Build C6455 EVM BE
pushd src/make
make clean
make evm_c6455 ENDIAN=big
popd
# Copy the files to the Binary package
cp -f src/make/bin/i2crom_0x50_c6455_be.bin ibl_bin_$IBL_VERSION/c6455/be/
cp -f src/make/bin/i2cparam_0x50_c6455_be_0x500.out ibl_bin_$IBL_VERSION/c6455/be/


# Build C6472 EVM LE
pushd src/make
make clean
make evm_c6472 ENDIAN=little
popd
# Copy the files to the Binary package
cp -f src/make/bin/i2crom_0x50_c6472_le.bin ibl_bin_$IBL_VERSION/c6472/le/
cp -f src/make/bin/i2cparam_0x50_c6472_le_0x500.out ibl_bin_$IBL_VERSION/c6472/le/

# Build C6472 EVM BE
pushd src/make
make clean
make evm_c6472 ENDIAN=big
popd
# Copy the files to the Binary package
cp -f src/make/bin/i2crom_0x50_c6472_be.bin ibl_bin_$IBL_VERSION/c6472/be/
cp -f src/make/bin/i2cparam_0x50_c6472_be_0x500.out ibl_bin_$IBL_VERSION/c6472/be/

# Build c6474 EVM little endian
pushd src/make
make clean
make evm_c6474 ENDIAN=little
popd
# Copy the files to the Binary package
cp -f src/make/bin/i2crom_0x50_c6474_le.bin ibl_bin_$IBL_VERSION/c6474/le/
cp -f src/make/bin/i2cparam_0x50_c6474_le_0x200.out ibl_bin_$IBL_VERSION/c6474/le/


# Build c6474 EVM Big endian
pushd src/make
make clean
make evm_c6474 ENDIAN=big
popd
# Copy the files to the Binary package
cp -f src/make/bin/i2crom_0x50_c6474_be.bin ibl_bin_$IBL_VERSION/c6474/be/
cp -f src/make/bin/i2cparam_0x50_c6474_be_0x200.out ibl_bin_$IBL_VERSION/c6474/be/

# Build c6474l EVM little Endian
pushd src/make
make clean
make evm_c6474l ENDIAN=little
popd
# Copy the files to the Binary package
cp -f src/make/bin/i2crom_0x50_c6474l_le.bin ibl_bin_$IBL_VERSION/c6474l/le/
cp -f src/make/bin/i2cparam_0x50_c6474l_le_0x200.out ibl_bin_$IBL_VERSION/c6474l/le/

# Build c6474l EVM Big Endian
pushd src/make
make clean
make evm_c6474l ENDIAN=big
popd
# Copy the files to the Binary package
cp -f src/make/bin/i2crom_0x50_c6474l_be.bin ibl_bin_$IBL_VERSION/c6474l/be/
cp -f src/make/bin/i2cparam_0x50_c6474l_be_0x200.out ibl_bin_$IBL_VERSION/c6474l/be/

# Build c6457 EVM little
pushd src/make
make clean
make evm_c6457 ENDIAN=little
popd
cp -f src/make/bin/i2crom_0x50_c6457_le.bin ibl_bin_$IBL_VERSION/c6457/le/
cp -f src/make/bin/i2cparam_0x50_c6457_le_0x200.out ibl_bin_$IBL_VERSION/c6457/le/

# Build c6457 EVM big
pushd src/make
make clean
make evm_c6457 ENDIAN=big
popd
cp -f src/make/bin/i2crom_0x50_c6457_be.bin ibl_bin_$IBL_VERSION/c6457/be/
cp -f src/make/bin/i2cparam_0x50_c6457_be_0x200.out ibl_bin_$IBL_VERSION/c6457/be/

# Build c6678 EVM LE
pushd src/make
make clean
make evm_c6678_i2c ENDIAN=little I2C_BUS_ADDR=0x51 
popd
cp -f src/make/bin/i2crom_0x51_c6678_le.bin ibl_bin_$IBL_VERSION/c6678/le/
cp -f src/make/bin/i2cparam_0x51_c6678_le_0x500.out ibl_bin_$IBL_VERSION/c6678/le/

# Build c6678 EVM BE
pushd src/make
make clean
make evm_c6678_i2c ENDIAN=big I2C_BUS_ADDR=0x51 
popd
cp -f src/make/bin/i2crom_0x51_c6678_be.bin ibl_bin_$IBL_VERSION/c6678/be/
cp -f src/make/bin/i2cparam_0x51_c6678_be_0x500.out ibl_bin_$IBL_VERSION/c6678/be/

# Build c6670 EVM LE
pushd src/make
make clean
make evm_c6670_i2c ENDIAN=little I2C_BUS_ADDR=0x51 
popd
cp -f src/make/bin/i2crom_0x51_c6670_le.bin ibl_bin_$IBL_VERSION/c6670/le/
cp -f src/make/bin/i2cparam_0x51_c6670_le_0x500.out ibl_bin_$IBL_VERSION/c6670/le/

# Build c6670 EVM BE
pushd src/make
make clean
make evm_c6670_i2c ENDIAN=big I2C_BUS_ADDR=0x51 
popd
cp -f src/make/bin/i2crom_0x51_c6670_be.bin ibl_bin_$IBL_VERSION/c6670/be/
cp -f src/make/bin/i2cparam_0x51_c6670_be_0x500.out ibl_bin_$IBL_VERSION/c6670/be/

# Create Tar archive for binary package
tar -czf ibl_bin_$IBL_VERSION.tgz ibl_bin_$IBL_VERSION

