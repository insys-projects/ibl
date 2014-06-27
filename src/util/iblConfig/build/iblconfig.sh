#!/bin/bash

if [ ${INSYS_BOARD} == INSYS_UNKNOWN ] ;
then
    echo "You need specify INSYS_BOARD environment variable to select board configuration!"
    exit -1;
fi

cp input.cfg input.txt

./iblConfig.out input.txt

echo "-------- Build for BOARD: ${INSYS_BOARD} --- I2C : ${I2C_BUS_ADDRESS} --------"

if [ ${INSYS_BOARD} == "INSYS_PEX_SRIO" ] ;
then
cp ibl.bin ibl_c6678_pex_srio_${I2C_BUS_ADDRESS}.bin;
cp ibl_c6678_pex_srio_${I2C_BUS_ADDRESS}.bin ../../../../bin/
fi

if [ ${INSYS_BOARD} == "INSYS_AC_DSP" ] ;
then
cp ibl.bin ibl_c6678_ac_dsp_${I2C_BUS_ADDRESS}.bin;
cp ibl_c6678_ac_dsp_${I2C_BUS_ADDRESS}.bin ../../../../bin/
fi

if [ ${INSYS_BOARD} == "INSYS_FMC110P" ] ;
then
cp ibl.bin ibl_c6678_fmc110p_${I2C_BUS_ADDRESS}.bin;
cp ibl_c6678_fmc110p_${I2C_BUS_ADDRESS}.bin ../../../../bin/
fi

if [ ${INSYS_BOARD} == "INSYS_FMC112CP" ] ;
then
cp ibl.bin ibl_c6678_fmc112cp_${I2C_BUS_ADDRESS}.bin;
cp ibl_c6678_fmc112cp_${I2C_BUS_ADDRESS}.bin ../../../../bin/
fi

if [ ${INSYS_BOARD} == "INSYS_FMC114V" ] ;
then
cp ibl.bin ibl_c6678_fmc114v_${I2C_BUS_ADDRESS}.bin;
cp ibl_c6678_fmc114v_${I2C_BUS_ADDRESS}.bin ../../../../bin/
fi

if [ ${INSYS_BOARD} == "INSYS_FMC116V" ] ;
then
cp ibl.bin ibl_c6678_fmc116v_${I2C_BUS_ADDRESS}.bin;
cp ibl_c6678_fmc116v_${I2C_BUS_ADDRESS}.bin ../../../../bin/
fi

if [ ${INSYS_BOARD} == "INSYS_FMC117CP" ] ;
then
cp ibl.bin ibl_c6678_fmc117cp_${I2C_BUS_ADDRESS}.bin;
cp ibl_c6678_fmc117cp_${I2C_BUS_ADDRESS}.bin ../../../../bin/
fi

if [ ${INSYS_BOARD} == "INSYS_FM408C" ] ;
then
cp ibl.bin ibl_c6678_fm408c_${I2C_BUS_ADDRESS}.bin;
cp ibl_c6678_fm408c_${I2C_BUS_ADDRESS}.bin ../../../../bin/
fi

#cp ibl.bin i2crom_${I2C_BUS_ADDRESS}_c6678_le.bin
#cp i2crom_${I2C_BUS_ADDRESS}_c6678_le.bin ../../../../../../product/
#cp i2crom_${I2C_BUS_ADDRESS}_c6678_le.bin ../../../../../../../embedded/c6x-bin
