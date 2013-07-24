#!/bin/bash

cp input.cfg input.txt

./iblConfig.out input.txt

cp ibl.bin i2crom_0x51_c6678_le.bin

cp i2crom_0x51_c6678_le.bin ../../../../../../product/