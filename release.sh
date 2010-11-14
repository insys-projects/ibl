#!/bin/sh
# Generate the source release tarballs
export IBL_VERSION="0_5_1"
cd ../
if [ -f ibl_src_$IBL_VERSION.tar ]; then rm ibl_src_$IBL_VERSION.tar; fi

tar -cvf ibl_src_$IBL_VERSION.tar ibl/src ibl/doc

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

#mkdir ibl_bin_$IBL_VERSION/c6474l/
#mkdir ibl_bin_$IBL_VERSION/c6474l/be
#mkdir ibl_bin_$IBL_VERSION/c6474l/le

#mkdir ibl_bin_$IBL_VERSION/c6457/
#mkdir ibl_bin_$IBL_VERSION/c6457/be
#mkdir ibl_bin_$IBL_VERSION/c6457/le


# Set Build environment
source setupenvLnx.sh

# Build C6455 LE
pushd src/make
make clean
make c6455 ENDIAN=little
popd
# Copy the files to the Binary package
cp -f src/make/ibl_c6455/i2crom_le.dat ibl_bin_$IBL_VERSION/c6455/le/
cp -f src/util/i2cWrite/i2cWrite.out   ibl_bin_$IBL_VERSION/c6455/le/
cp -f src/util/i2cConfig/i2cparam_c6455.out   ibl_bin_$IBL_VERSION/c6455/le/


# Copy i2cConfig GEL file
cp -f src/util/i2cConfig/i2cConfig.gel ibl_bin_$IBL_VERSION/i2cConfig.gel


# Build C6455 BE
pushd src/make
make clean
make c6455 ENDIAN=big
popd
# Copy the files to the Binary package
cp -f src/make/ibl_c6455/i2crom_be.dat ibl_bin_$IBL_VERSION/c6455/be/
cp -f src/util/i2cWrite/i2cWrite.out   ibl_bin_$IBL_VERSION/c6455/be/
cp -f src/util/i2cConfig/i2cparam_c6455.out   ibl_bin_$IBL_VERSION/c6455/be/

# Build C6472 le
pushd src/make
make clean
make c6472 ENDIAN=little
popd
# Copy the files to the Binary package
cp -f src/make/ibl_c6472/i2crom_le.dat ibl_bin_$IBL_VERSION/c6472/le/
cp -f src/util/i2cWrite/i2cWrite.out   ibl_bin_$IBL_VERSION/c6472/le/
cp -f src/util/i2cConfig/i2cparam_c6472.out   ibl_bin_$IBL_VERSION/c6472/le/


# Build c6472 be
pushd src/make
make clean
make c6472 ENDIAN=big
popd
# Copy the files to the Binary package
cp -f src/make/ibl_c6472/i2crom_be.dat ibl_bin_$IBL_VERSION/c6472/be/
cp -f src/util/i2cWrite/i2cWrite.out   ibl_bin_$IBL_VERSION/c6472/be/
cp -f src/util/i2cConfig/i2cparam_c6472.out   ibl_bin_$IBL_VERSION/c6472/be/

# Build c6474 le
pushd src/make
make clean
make c6474 ENDIAN=little
popd
# Copy the files to the Binary package
cp -f src/make/ibl_c6474/i2crom_le.dat ibl_bin_$IBL_VERSION/c6474/le/
cp -f src/util/i2cWrite/i2cWrite.out   ibl_bin_$IBL_VERSION/c6474/le/
cp -f src/util/i2cConfig/i2cparam_c6474.out   ibl_bin_$IBL_VERSION/c6474/le/


# Build c6474 be
pushd src/make
make clean
make c6474 ENDIAN=big
popd
# Copy the files to the Binary package
cp -f src/make/ibl_c6474/i2crom_be.dat ibl_bin_$IBL_VERSION/c6474/be/
cp -f src/util/i2cWrite/i2cWrite.out   ibl_bin_$IBL_VERSION/c6474/be/
cp -f src/util/i2cConfig/i2cparam_c6474.out   ibl_bin_$IBL_VERSION/c6474/be/

# Build c6474l le
#pushd src/make
#make clean
#make c6474l ENDIAN=little
#popd
# Copy the files to the Binary package
#cp -f src/make/ibl_c6474l/i2crom_le.dat ibl_bin_$IBL_VERSION/c6474l/le/
#cp -f src/util/i2cWrite/i2cWrite.out   ibl_bin_$IBL_VERSION/c6474l/le/
#cp -f src/util/i2cConfig/i2cparam_c6474l.out   ibl_bin_$IBL_VERSION/c6474l/le/


# Build c6474l be
#pushd src/make
#make clean
#make c6474l ENDIAN=big
#popd
# Copy the files to the Binary package
#cp -f src/make/ibl_c6474l/i2crom_be.dat ibl_bin_$IBL_VERSION/c6474l/be/
#cp -f src/util/i2cWrite/i2cWrite.out   ibl_bin_$IBL_VERSION/c6474l/be/
#cp -f src/util/i2cConfig/i2cparam_c6474l.out   ibl_bin_$IBL_VERSION/c6474l/be/


# Build c6457 le
#pushd src/make
#make clean
#make c6457 ENDIAN=little
#popd
# Copy the files to the Binary package
#cp -f src/make/ibl_c6457/i2crom_le.dat ibl_bin_$IBL_VERSION/c6457/le/
#cp -f src/util/i2cWrite/i2cWrite.out   ibl_bin_$IBL_VERSION/c6457/le/
#cp -f src/util/i2cConfig/i2cparam_c6457.out   ibl_bin_$IBL_VERSION/c6457/le/


# Build c6457 be
#pushd src/make
#make clean
#make c6457 ENDIAN=big
#popd
# Copy the files to the Binary package
#cp -f src/make/ibl_c6457/i2crom_be.dat ibl_bin_$IBL_VERSION/c6457/be/
#cp -f src/util/i2cWrite/i2cWrite.out   ibl_bin_$IBL_VERSION/c6457/be/
#cp -f src/util/i2cConfig/i2cparam_c6457.out   ibl_bin_$IBL_VERSION/c6457/be/

# Create Tar archive for binary package
tar -cvzf ibl_bin_$IBL_VERSION.tar ibl_bin_$IBL_VERSION

