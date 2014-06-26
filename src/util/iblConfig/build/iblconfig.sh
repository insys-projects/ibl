#!/bin/bash

cp input.cfg input.txt

./iblConfig.out input.txt

echo "-------- Build for BOARD: ${INSYS_BOARD} --- I2C : ${I2C_BUS_ADDRESS} --------"


if [ ${INSYS_BOARD} == INSYS_PEX_SRIO ] ;
then
cp ibl.bin ibl_c6678_pex_srio_${I2C_BUS_ADDRESS}.bin;
cp ibl_c6678_pex_srio_${I2C_BUS_ADDRESS}.bin ../../../../bin/
fi


#cp i2crom_${I2C_BUS_ADDRESS}_c6678_le.bin ../../../../../../product/
#cp i2crom_${I2C_BUS_ADDRESS}_c6678_le.bin ../../../../../../../embedded/c6x-bin
