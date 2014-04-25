#!/bin/bash

cp input.cfg input.txt

./iblConfig.out input.txt

echo "-------- Build for I2C_BUS_ADDRESS : ${I2C_BUS_ADDRESS} --------"

cp ibl.bin i2crom_${I2C_BUS_ADDRESS}_c6678_le.bin
cp i2crom_${I2C_BUS_ADDRESS}_c6678_le.bin ../../../../../../product/
cp i2crom_${I2C_BUS_ADDRESS}_c6678_le.bin ../../../../../../../embedded/c6x-bin
