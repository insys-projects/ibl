/*
 *
 * Copyright (C) 2010-2012 Texas Instruments Incorporated - http://www.ti.com/
 *
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
*/

/**
 * @file c66xk2xutil.c
 *
 * @brief
 *	  c66xk2x functions used by both the ibl and utility programs
 */

#include "device.h"
#include "target.h"
#include "pllapi.h"
#include "types.h"

/**
 *  @brief
 *     Configure the predivider for the main PLL, which resides outside the PLL controller
 */
SINT16 chipPllSetExternalPrediv(UINT16 pllNum, UINT32 predivRegVal)
{
    UINT32 reg;

    reg = DEVICE_REG32_R (DEVICE_MAIN_PLL_CTL_0);
    reg = BOOT_SET_BITFIELD(reg, predivRegVal, 5, 0);
    DEVICE_REG32_W (DEVICE_MAIN_PLL_CTL_0, reg);


    return (0);

} /* chipPllSetExternalPrediv */


/**
 *  @brief
 *      Configure the bandwidth adjustment for the main PLL, which resides outside the PLL controller
 */
SINT16 chipPllExternalBwAdj (UINT16 pllNum, UINT16 mult)
{
    UINT32 reg;
    UINT32 bwAdj;

    bwAdj = (mult >> 1) - 1;

    reg = DEVICE_REG32_R (DEVICE_MAIN_PLL_CTL_0);
    reg = BOOT_SET_BITFIELD(reg, bwAdj & 0x00ff, 31, 24);
    DEVICE_REG32_W (DEVICE_MAIN_PLL_CTL_0, reg);

    reg = DEVICE_REG32_R (DEVICE_MAIN_PLL_CTL_1);
    reg = BOOT_SET_BITFIELD(reg, bwAdj >> 8, 3, 0);
    DEVICE_REG32_W (DEVICE_MAIN_PLL_CTL_1, reg);

    return (0);

} /* chipPllExternalBwAdj */


/**
 *  @brief
 *      Configure the multiplier fields for the main PLL which reside outside the PLL controller
 */
UINT32 chipPllExternalMult (UINT16 pllNum, UINT16 mult)
{
    UINT32 pmult;
    UINT32 reg;
    UINT32 v;


    pmult = mult-1;

    v = BOOT_READ_BITFIELD(pmult, 12, 6);


    reg = DEVICE_REG32_R (DEVICE_MAIN_PLL_CTL_0);
    reg = BOOT_SET_BITFIELD(reg, v, 18, 12);
    DEVICE_REG32_W (DEVICE_MAIN_PLL_CTL_0, reg);


    v = BOOT_READ_BITFIELD(pmult, 5, 0);

    return (v);

} /* chipPllExternalMult */

/**
 *  @brief return the PSC module number for SPI
 */
int32 deviceSpiPscNum (void)
{
    uint32 v;

    /* SPI is module number 3 for c66xk2x devices. the SPI is in the
     * always on domain */
    v = *((Uint32 *)DEVICE_JTAG_ID_REG);
    v &= DEVICE_JTAG_ID_MASK;
    if (v == DEVICE_TCI6636K2H_JTAG_ID_VAL)
        return (TARGET_SPI_EMIF_TCI6634K2K);

    return (-1);  /* A negative number indicates the always on domain */
}

/**
 *  @brief Power up a peripheral
 *
 *  @details
 *    Boot peripherals are powered up
 */
int32 devicePowerPeriph (int32 modNum)
{
    int32 ret;

    /* If the input value is < 0 there is nothing to power up */
    if (modNum < 0)
        return (0);


    if (modNum >= TARGET_PWR_MAX_MOD)
        return (-1);


    /* Note that if the sgmii power enable is requested the PA must be
     * powered up first */
    if (modNum == TARGET_PWR_ETH(x))  {
        ret = (int32)pscEnableModule (TARGET_PWR_PA);
        if (ret != 0)
            return (ret);
    }

    return ((int32)pscEnableModule(modNum));

}

#ifdef PLL_REINIT_WORKAROUND
/**
 *  @brief Simple DDR3 test
 *
 *  @details
 *      This function performs a simple DDR3 test for a memory range
 *      specified below and returns -1 for failure and 0 for success.
 */


UINT32 ddr3_memory_test (void)
{
	 UINT32 index, value;

	/* Write a pattern */
	for (index = DDR3_TEST_START_ADDRESS; index < DDR3_TEST_END_ADDRESS; index += 4) {
		*(VUint32 *) index = (UINT32)index;
	}

	/* Read and check the pattern */
	for (index = DDR3_TEST_START_ADDRESS; index < DDR3_TEST_END_ADDRESS; index += 4) {

		value = *(UINT32 *) index;

		if (value  != index) {
			return -1;
		}
	}

	/* Write a pattern for complementary values */
	for (index = DDR3_TEST_START_ADDRESS; index < DDR3_TEST_END_ADDRESS; index += 4) {
		*(VUint32 *) index = (UINT32)~index;
	}

	/* Read and check the pattern */
	for (index = DDR3_TEST_START_ADDRESS; index < DDR3_TEST_END_ADDRESS; index += 4) {

		value = *(UINT32 *) index;

		if (value  != ~index) {
			return -1;
		}
	}

	return 0;
}
#endif

