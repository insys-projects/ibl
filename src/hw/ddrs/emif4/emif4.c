/*************************************************************************************************
 * FILE PURPOSE: EMIF4 driver
 *************************************************************************************************
 * FILE NAME: emif4.c
 *
 * DESCRIPTION: The boot emif4 driver
 *
 *************************************************************************************************/
#include "types.h"
#include "ibl.h"
#include "emif4_api.h"
#include "emif4_loc.h"
#include "device.h"

#define CHIP_LEVEL_REG  0x02620000

#define KICK0			*(volatile unsigned int*)(CHIP_LEVEL_REG + 0x0038)
#define KICK1			*(volatile unsigned int*)(CHIP_LEVEL_REG + 0x003C)
#define KICK0_UNLOCK		0x83e70b13
#define KICK1_UNLOCK		0x95a4f1e0

#define DDR3PLLCTL0		*(volatile unsigned int*)(CHIP_LEVEL_REG + 0x0330)
#define DDR3PLLCTL1		*(volatile unsigned int*)(CHIP_LEVEL_REG + 0x0334)

// DDR3 definitions
#define DDR_BASE_ADDR 0x21000000

#define DDR_MIDR               (*(volatile unsigned int*)(DDR_BASE_ADDR + 0x00000000))
#define DDR_SDCFG              (*(volatile unsigned int*)(DDR_BASE_ADDR + 0x00000008))
#define DDR_SDRFC              (*(volatile unsigned int*)(DDR_BASE_ADDR + 0x00000010))
#define DDR_SDTIM1             (*(volatile unsigned int*)(DDR_BASE_ADDR + 0x00000018))
#define DDR_SDTIM2             (*(volatile unsigned int*)(DDR_BASE_ADDR + 0x00000020))
#define DDR_SDTIM3             (*(volatile unsigned int*)(DDR_BASE_ADDR + 0x00000028))
#define DDR_PMCTL              (*(volatile unsigned int*)(DDR_BASE_ADDR + 0x00000038))
#define DDR_RDWR_LVL_RMP_CTRL  (*(volatile unsigned int*)(DDR_BASE_ADDR + 0x000000D8))
#define DDR_RDWR_LVL_CTRL      (*(volatile unsigned int*)(DDR_BASE_ADDR + 0x000000DC))
#define DDR_DDRPHYC            (*(volatile unsigned int*)(DDR_BASE_ADDR + 0x000000E4))
#define DDR_ZQCFG              (*(volatile unsigned int*)(DDR_BASE_ADDR + 0x000000C8))

#define DATA0_GTLVL_INIT_RATIO	(*(volatile unsigned int*)(0x0262043C))
#define DATA1_GTLVL_INIT_RATIO	(*(volatile unsigned int*)(0x02620440))
#define DATA2_GTLVL_INIT_RATIO	(*(volatile unsigned int*)(0x02620444))
#define DATA3_GTLVL_INIT_RATIO	(*(volatile unsigned int*)(0x02620448))
#define DATA4_GTLVL_INIT_RATIO	(*(volatile unsigned int*)(0x0262044C))
#define DATA5_GTLVL_INIT_RATIO	(*(volatile unsigned int*)(0x02620450))
#define DATA6_GTLVL_INIT_RATIO	(*(volatile unsigned int*)(0x02620454))
#define DATA7_GTLVL_INIT_RATIO	(*(volatile unsigned int*)(0x02620458))
#define DATA8_GTLVL_INIT_RATIO	(*(volatile unsigned int*)(0x0262045C))

#define DATA0_WRLVL_INIT_RATIO  (*(unsigned int*)(0x0262040C))
#define DATA1_WRLVL_INIT_RATIO  (*(unsigned int*)(0x02620410))
#define DATA2_WRLVL_INIT_RATIO  (*(unsigned int*)(0x02620414))
#define DATA3_WRLVL_INIT_RATIO  (*(unsigned int*)(0x02620418))
#define DATA4_WRLVL_INIT_RATIO  (*(unsigned int*)(0x0262041C))
#define DATA5_WRLVL_INIT_RATIO  (*(unsigned int*)(0x02620420))
#define DATA6_WRLVL_INIT_RATIO  (*(unsigned int*)(0x02620424))
#define DATA7_WRLVL_INIT_RATIO  (*(unsigned int*)(0x02620428))
#define DATA8_WRLVL_INIT_RATIO  (*(unsigned int*)(0x0262042C))

#define RDWR_INIT_RATIO_0	(*(volatile unsigned int*)(0x0262040C))
#define RDWR_INIT_RATIO_1	(*(volatile unsigned int*)(0x02620410))
#define RDWR_INIT_RATIO_2	(*(volatile unsigned int*)(0x02620414))
#define RDWR_INIT_RATIO_3	(*(volatile unsigned int*)(0x02620418))
#define RDWR_INIT_RATIO_4	(*(volatile unsigned int*)(0x0262041C))
#define RDWR_INIT_RATIO_5	(*(volatile unsigned int*)(0x02620420))
#define RDWR_INIT_RATIO_6	(*(volatile unsigned int*)(0x02620424))
#define RDWR_INIT_RATIO_7	(*(volatile unsigned int*)(0x02620428))
#define RDWR_INIT_RATIO_8	(*(volatile unsigned int*)(0x0262042C))


#define DDR3_CONFIG_REG_0   (*(volatile unsigned int*)(0x02620404))
#define DDR3_CONFIG_REG_12  (*(volatile unsigned int*)(0x02620434))
#define DDR3_CONFIG_REG_23  (*(volatile unsigned int*)(0x02620460))
#define DDR3_CONFIG_REG_24  (*(volatile unsigned int*)(0x02620464))

#define RD_DQS_SLAVE_RATIO 0x34
#define WR_DQS_SLAVE_RATIO 0xA9
#define WR_DATA_SLAVE_RATIO 0xE9
#define FIFO_WE_SLAVE_RATIO 0x106


static void ddr3_wait (uint32 del)
{
    volatile unsigned int i;

    for (i = 0; i < del; i++) asm (" nop ");

}

/*************************************************************************************************
 * FUNCTION PUROPSE: Initial EMIF4 setup
 *************************************************************************************************
 * DESCRIPTION: Emif configuration
 *************************************************************************************************/
SINT16 hwEmif4p0Enable (iblEmif4p0_t *cfg)
{
    UINT32 v, i, TEMP;
    
    v = DEVICE_REG32_R(DEVICE_JTAG_ID_REG);
    v &= DEVICE_JTAG_ID_MASK;
    
    if ( (v == DEVICE_C6678_JTAG_ID_VAL) ||
         (v == DEVICE_C6670_JTAG_ID_VAL) )
    {


	KICK0 = KICK0_UNLOCK;
	KICK1 = KICK1_UNLOCK;
        
       /**************** 3.3 Leveling register configuration ********************/
        DDR3_CONFIG_REG_0 &= ~(0x007FE000);  // clear ctrl_slave_ratio field
        DDR3_CONFIG_REG_0 |= 0x00200000;     // set ctrl_slave_ratio to 0x100
        DDR3_CONFIG_REG_12 |= 0x08000000;    // Set invert_clkout = 1
        DDR3_CONFIG_REG_0 |= 0xF;            // set dll_lock_diff to 15
        DDR3_CONFIG_REG_23 |= 0x00000200;    // See section 4.2.1, set for partial automatic levelling
            
      /**************** 3.3 Partial Automatic Leveling ********************/
      DATA0_WRLVL_INIT_RATIO = 0x5E;
      DATA1_WRLVL_INIT_RATIO = 0x5E;
      DATA2_WRLVL_INIT_RATIO = 0x5E;
      DATA3_WRLVL_INIT_RATIO = 0x51;
      DATA4_WRLVL_INIT_RATIO = 0x38;
      DATA5_WRLVL_INIT_RATIO = 0x3A;
      DATA6_WRLVL_INIT_RATIO = 0x24;
      DATA7_WRLVL_INIT_RATIO = 0x20;
      DATA8_WRLVL_INIT_RATIO = 0x44;

      DATA0_GTLVL_INIT_RATIO = 0xDD;
      DATA1_GTLVL_INIT_RATIO = 0xDD;
      DATA2_GTLVL_INIT_RATIO = 0xBE;
      DATA3_GTLVL_INIT_RATIO = 0xCA;
      DATA4_GTLVL_INIT_RATIO = 0xA9;
      DATA5_GTLVL_INIT_RATIO = 0xA7;
      DATA6_GTLVL_INIT_RATIO = 0x9E;
      DATA7_GTLVL_INIT_RATIO = 0xA1;
      DATA8_GTLVL_INIT_RATIO = 0xBA;
  
      //Do a PHY reset. Toggle DDR_PHY_CTRL_1 bit 15 0->1->0
      DDR_DDRPHYC &= ~(0x00008000);
      DDR_DDRPHYC |= (0x00008000);
      DDR_DDRPHYC &= ~(0x00008000);

      /***************** 3.4 Basic Controller and DRAM configuration ************/
      DDR_SDRFC    = 0x00005162;    // enable configuration 

      /* DDR_SDTIM1   = 0x1113783C; */
       TEMP = 0;
       TEMP |= 0x8 << 25; // T_RP bit field 28:25
       TEMP |= 0x8 << 21; // T_RCD bit field 24:21
       TEMP |= 0x9 << 17; // T_WR bit field 20:17
       TEMP |= 0x17 << 12; // T_RAS bit field 16:12
       TEMP |= 0x20 << 6; // T_RC bit field 11:6
       TEMP |= 0x7 << 3; // T_RRD bit field 5:3
       TEMP |= 0x4; // T_WTR bit field 2:0
       DDR_SDTIM1 = TEMP;

      /* DDR_SDTIM2   = 0x30717FE3; */
       TEMP = 0;
       TEMP |= 0x3 << 28; // T_XP bit field 30:28
       TEMP |= 0x71 << 16; // T_XSNR bit field 24:16
       TEMP |= 0x1ff << 6; // T_XSRD bit field 15:6
       TEMP |= 0x4 << 3; // T_RTP bit field 5:3
       TEMP |= 0x3; // T_CKE bit field 2:0
       DDR_SDTIM2 = TEMP;

      /*  DDR_SDTIM3   = 0x559F86AF; */
       TEMP = 0;
       TEMP |= 0x5 << 28; // T_PDLL_UL bit field 31:28 (fixed value)
       TEMP |= 0x5 << 24; // T_CSTA bit field 27:24 (fixed value)
       TEMP |= 0x4 << 21; // T_CKESR bit field 23:21
       TEMP |= 0x3f << 15; // T_ZQCS bit field 20:15
       TEMP |= 0x6a << 4; // T_RFC bit field 12:4
       TEMP |= 0xf; // T_RAS_MAX bit field 3:0 (fixed value)
       DDR_SDTIM3 = TEMP; 

        DDR_DDRPHYC  = 0x0010010F;
     
        DDR_ZQCFG    = 0x70073214; 

        DDR_PMCTL    = 0x0;
     
        DDR_SDRFC = 0x00005162; // enable configuration

        /* DDR_SDCFG    = 0x63062A32; */
        /* New value with DYN_ODT disabled and SDRAM_DRIVE = RZQ/7 //0x63222A32;    // last config write DRAM init occurs */
         TEMP = 0;
         TEMP |= 0x3 << 29; // SDRAM_TYPE bit field 31:29 (fixed value)
         TEMP |= 0x0 << 27; // IBANK_POS bit field 28:27
         TEMP |= 0x3 << 24; // DDR_TERM bit field 26:24
         TEMP |= 0x0 << 21; // DYN_ODT bit field 22:21
         TEMP |= 0x1 << 18; // SDRAM_DRIVE bit field 19:18
         TEMP |= 0x2 << 16; // CWL bit field 17:16
         TEMP |= 0x0 << 14; // NM bit field 15:14
         TEMP |= 0xA << 10; // CL bit field 13:10
         TEMP |= 0x4 << 7; // ROWSIZE bit field 9:7
         TEMP |= 0x3 << 4; // IBANK bit field 6:4
         TEMP |= 0x0 << 3; // EBANK bit field 3:3
         TEMP |= 0x2; // PAGESIZE bit field 2:0
         DDR_SDCFG = TEMP;

 	/* assuming max device speed, 1.4GHz, 1 cycle = 0.714 ns *
  	* so, 100 us = 100000 ns = 140056 cycles
          thereby, 600us=840336 */
         ddr3_wait(840336);             //Wait 600us for HW init to complete

        DDR_SDRFC = 0x00001450;       //Refresh rate = (7.8*666MHz]

      /***************** 4.2.1 Partial automatic leveling ************/
        DDR_RDWR_LVL_RMP_CTRL = 0x80000000; //enable automatic leveling
   
        /*Trigger automatic leveling - This ignores read DQS leveling result and uses ratio forced value
          Wait for min 1048576 DDR clock cycles for leveling to complete = 1048576 * 1.5ns = 1572864ns = 1.57ms.
          Actual time = ~10-15 ms */
        DDR_RDWR_LVL_CTRL = 0x80000000; 

 	/* assuming max device speed, 1.4GHz, 1 cycle = 0.714 ns *
  	* so, 100 us = 100000 ns = 140056 cycles
          thereby, 3ms=3000us=4201680 */
	ddr3_wait(4201680); //Wait 3ms for leveling to complete
    }
    else if (v == DEVICE_C6657_JTAG_ID_VAL)
    {
	KICK0 = KICK0_UNLOCK;
	KICK1 = KICK1_UNLOCK;
        
       /**************** 3.3 Leveling register configuration ********************/
        DDR3_CONFIG_REG_0 &= ~(0x007FE000);  // clear ctrl_slave_ratio field
        DDR3_CONFIG_REG_0 |= 0x00200000;     // set ctrl_slave_ratio to 0x100
        DDR3_CONFIG_REG_12 |= 0x08000000;    // Set invert_clkout = 1
        DDR3_CONFIG_REG_0 |= 0xF;            // set dll_lock_diff to 15
        DDR3_CONFIG_REG_23 |= 0x00000200;    // See section 4.2.1, set for partial automatic levelling
            
      /**************** 3.3 Partial Automatic Leveling ********************/
      DATA0_WRLVL_INIT_RATIO = 0x00;
      DATA1_WRLVL_INIT_RATIO = 0x00;
      DATA2_WRLVL_INIT_RATIO = 0x00;
      DATA3_WRLVL_INIT_RATIO = 0x00;
      DATA4_WRLVL_INIT_RATIO = 0x33;
      DATA5_WRLVL_INIT_RATIO = 0x3A;
      DATA6_WRLVL_INIT_RATIO = 0x2C;
      DATA7_WRLVL_INIT_RATIO = 0x2C;
      DATA8_WRLVL_INIT_RATIO = 0x21;

      DATA0_GTLVL_INIT_RATIO = 0x00;
      DATA1_GTLVL_INIT_RATIO = 0x00;
      DATA2_GTLVL_INIT_RATIO = 0x00;
      DATA3_GTLVL_INIT_RATIO = 0x00;
      DATA4_GTLVL_INIT_RATIO = 0xB7;
      DATA5_GTLVL_INIT_RATIO = 0xB1;
      DATA6_GTLVL_INIT_RATIO = 0xA4;
      DATA7_GTLVL_INIT_RATIO = 0xA4;
      DATA8_GTLVL_INIT_RATIO = 0x98;
  
      //Do a PHY reset. Toggle DDR_PHY_CTRL_1 bit 15 0->1->0
      DDR_DDRPHYC &= ~(0x00008000);
      DDR_DDRPHYC |= (0x00008000);
      DDR_DDRPHYC &= ~(0x00008000);

      /***************** 3.4 Basic Controller and DRAM configuration ************/
      DDR_SDRFC    = 0x0000515C;    // enable configuration

      /* DDR_SDTIM1   = 0x1113783C; */
       TEMP = 0;
       TEMP |= 0x9 << 25; // T_RP bit field 28:25
       TEMP |= 0x9 << 21; // T_RCD bit field 24:21
       TEMP |= 0x9 << 17; // T_WR bit field 20:17
       TEMP |= 0x17 << 12; // T_RAS bit field 16:12
       TEMP |= 0x20 << 6; // T_RC bit field 11:6
       TEMP |= 0x1 << 3; // T_RRD bit field 5:3
       TEMP |= 0x4; // T_WTR bit field 2:0
       DDR_SDTIM1 = TEMP;

      /* DDR_SDTIM2   = 0x304F7FE3; */
       TEMP = 0;
       TEMP |= 0x3 << 28; // T_XP bit field 30:28
       TEMP |= 0x71 << 16; // T_XSNR bit field 24:16
       TEMP |= 0x1ff << 6; // T_XSRD bit field 15:6
       TEMP |= 0x4 << 3; // T_RTP bit field 5:3
       TEMP |= 0x3; // T_CKE bit field 2:0
       DDR_SDTIM2 = TEMP;

      /*  DDR_SDTIM3   = 0x559F849F; */
       TEMP = 0;
       TEMP |= 0x5 << 28; // T_PDLL_UL bit field 31:28 (fixed value)
       TEMP |= 0x5 << 24; // T_CSTA bit field 27:24 (fixed value)
       TEMP |= 0x4 << 21; // T_CKESR bit field 23:21
       TEMP |= 0x3f << 15; // T_ZQCS bit field 20:15
       TEMP |= 0x6A << 4; // T_RFC bit field 12:4
       TEMP |= 0xf; // T_RAS_MAX bit field 3:0 (fixed value)
       DDR_SDTIM3 = TEMP; 

        DDR_DDRPHYC  = 0x0010010F;
     
        DDR_ZQCFG    = 0x70074C1F; 

        DDR_PMCTL    = 0x0;
     
        DDR_SDRFC = 0x0000515C; // enable configuration

        /* DDR_SDCFG    = 0x63062A32; */
        /* New value with DYN_ODT disabled and SDRAM_DRIVE = RZQ/7 //0x63222A32;    // last config write DRAM init occurs */
        TEMP = 0;
        TEMP |= 0x3 << 29; // SDRAM_TYPE bit field 31:29 (fixed value)
        TEMP |= 0x0 << 27; // IBANK_POS bit field 28:27
        TEMP |= 0x2 << 24; // DDR_TERM bit field 26:24
        TEMP |= 0x2 << 21; // DYN_ODT bit field 22:21
        TEMP |= 0x1 << 18; // SDRAM_DRIVE bit field 19:18
        TEMP |= 0x3 << 16; // CWL bit field 17:16
        TEMP |= 0x1 << 14; // NM bit field 15:14
        TEMP |= 0xE << 10; // CL bit field 13:10
        TEMP |= 0x5 << 7; // ROWSIZE bit field 9:7
        TEMP |= 0x3 << 4; // IBANK bit field 6:4
        TEMP |= 0x0 << 3; // EBANK bit field 3:3
        TEMP |= 0x2; // PAGESIZE bit field 2:0
        DDR_SDCFG = TEMP;

 	/* assuming max device speed, 1.4GHz, 1 cycle = 0.714 ns *
  	* so, 100 us = 100000 ns = 140056 cycles
          thereby, 600us=840336 */
         ddr3_wait(840336);             //Wait 600us for HW init to complete

        DDR_SDRFC = 0x0000144F;       //Refresh rate = (7.8*666MHz]

      /***************** 4.2.1 Partial automatic leveling ************/
        DDR_RDWR_LVL_RMP_CTRL = 0x80000000; //enable automatic leveling
   
        /*Trigger automatic leveling - This ignores read DQS leveling result and uses ratio forced value
          Wait for min 1048576 DDR clock cycles for leveling to complete = 1048576 * 1.5ns = 1572864ns = 1.57ms.
          Actual time = ~10-15 ms */
        DDR_RDWR_LVL_CTRL = 0x80000000; 

 	/* assuming max device speed, 1.4GHz, 1 cycle = 0.714 ns *
  	* so, 100 us = 100000 ns = 140056 cycles
          thereby, 3ms=3000us=4201680 */
	ddr3_wait(4201680); //Wait 3ms for leveling to complete
    
    }		
    else
    {
        /* C64x configuration */
        /* If the config registers or refresh control registers are being written
         * disable the initialization sequence until they are all setup */
        if ((cfg->registerMask & EMIF4_INIT_SEQ_MASK) != 0)  {
            
            v = DEVICE_REG32_R (DEVICE_EMIF4_BASE + EMIF_REG_SDRAM_REF_CTL);
            EMIF_REG_VAL_SDRAM_REF_CTL_SET_INITREF_DIS(v,1);
            DEVICE_REG32_W (DEVICE_EMIF4_BASE + EMIF_REG_SDRAM_REF_CTL, v);
        }
        
        if ((cfg->registerMask & ibl_EMIF4_ENABLE_sdRamTiming1) != 0)
            DEVICE_REG32_W (DEVICE_EMIF4_BASE + EMIF_REG_TIMING1, cfg->sdRamTiming1);
        
        if ((cfg->registerMask & ibl_EMIF4_ENABLE_sdRamTiming2) != 0)
            DEVICE_REG32_W (DEVICE_EMIF4_BASE + EMIF_REG_TIMING2, cfg->sdRamTiming2);
        
        if ((cfg->registerMask & ibl_EMIF4_ENABLE_sdRamTiming3) != 0)
            DEVICE_REG32_W (DEVICE_EMIF4_BASE + EMIF_REG_TIMING3, cfg->sdRamTiming3);
        
        if ((cfg->registerMask & ibl_EMIF4_ENABLE_lpDdrNvmTiming) != 0)
            DEVICE_REG32_W (DEVICE_EMIF4_BASE + EMIF_REG_NVM_TIMING, cfg->lpDdrNvmTiming);
        
        if ((cfg->registerMask & ibl_EMIF4_ENABLE_powerManageCtl) != 0)
            DEVICE_REG32_W (DEVICE_EMIF4_BASE + EMIF_REG_PWR_MNG_CTL, cfg->powerManageCtl);
        
        if ((cfg->registerMask & ibl_EMIF4_ENABLE_iODFTTestLogic) != 0)
            DEVICE_REG32_W (DEVICE_EMIF4_BASE + EMIF_REG_IODFT_TST_LOGIC, cfg->iODFTTestLogic);
        
        if ((cfg->registerMask & ibl_EMIF4_ENABLE_performCountCfg) != 0)
            DEVICE_REG32_W (DEVICE_EMIF4_BASE + EMIF_REG_PERFORM_CNT_CFG, cfg->performCountCfg);
        
        if ((cfg->registerMask & ibl_EMIF4_ENABLE_performCountMstRegSel) != 0)
            DEVICE_REG32_W (DEVICE_EMIF4_BASE + EMIF_REG_PERFORM_CNT_MST_REG_SEL, cfg->performCountMstRegSel);
        
        if ((cfg->registerMask & ibl_EMIF4_ENABLE_readIdleCtl) != 0)
            DEVICE_REG32_W (DEVICE_EMIF4_BASE + EMIF_REG_IDLE_CTL, cfg->readIdleCtl);
        
        if ((cfg->registerMask & ibl_EMIF4_ENABLE_sysVbusmIntEnSet) != 0)
            DEVICE_REG32_W (DEVICE_EMIF4_BASE + EMIF_REG_INT_EN_SET, cfg->sysVbusmIntEnSet);
        
        if ((cfg->registerMask & ibl_EMIF4_ENABLE_sdRamOutImpdedCalCfg) != 0)
            DEVICE_REG32_W (DEVICE_EMIF4_BASE + EMIF_REG_OUT_IMP_CAL_CFG, cfg->sdRamOutImpdedCalCfg);
        
        if ((cfg->registerMask & ibl_EMIF4_ENABLE_tempAlterCfg) != 0)
            DEVICE_REG32_W (DEVICE_EMIF4_BASE + EMIF_REG_TEMP_ALTER_CFG, cfg->tempAlterCfg);
        
        if ((cfg->registerMask & ibl_EMIF4_ENABLE_ddrPhyCtl1) != 0)
            DEVICE_REG32_W (DEVICE_EMIF4_BASE + EMIF_REG_PHY_CTL1, cfg->ddrPhyCtl1);
        
        if ((cfg->registerMask & ibl_EMIF4_ENABLE_ddrPhyCtl2) != 0)
            DEVICE_REG32_W (DEVICE_EMIF4_BASE + EMIF_REG_PHY_CLT2, cfg->ddrPhyCtl2);
        
        if ((cfg->registerMask & ibl_EMIF4_ENABLE_priClassSvceMap) != 0)
            DEVICE_REG32_W (DEVICE_EMIF4_BASE + EMIF_REG_PRI_CLASS_SVC_MAP, cfg->priClassSvceMap);
        
        if ((cfg->registerMask & ibl_EMIF4_ENABLE_mstId2ClsSvce1Map) != 0)
            DEVICE_REG32_W (DEVICE_EMIF4_BASE + EMIF_REG_ID2CLS_SVC_1MAP, cfg->mstId2ClsSvce1Map);
        
        if ((cfg->registerMask & ibl_EMIF4_ENABLE_mstId2ClsSvce2Map) != 0)
            DEVICE_REG32_W (DEVICE_EMIF4_BASE + EMIF_REG_ID2CLS_SVC_2MAP, cfg->mstId2ClsSvce2Map);
        
        if ((cfg->registerMask & ibl_EMIF4_ENABLE_eccCtl) != 0)
            DEVICE_REG32_W (DEVICE_EMIF4_BASE + EMIF_REG_ECC_CTL, cfg->eccCtl);
        
        if ((cfg->registerMask & ibl_EMIF4_ENABLE_eccRange1) != 0)
            DEVICE_REG32_W (DEVICE_EMIF4_BASE + EMIF_REG_ECC_RANGE1, cfg->eccRange1);
        
        if ((cfg->registerMask & ibl_EMIF4_ENABLE_eccRange2) != 0)
            DEVICE_REG32_W (DEVICE_EMIF4_BASE + EMIF_REG_ECC_RANGE2, cfg->eccRange2);
        
        if ((cfg->registerMask & ibl_EMIF4_ENABLE_rdWrtExcThresh) != 0)
            DEVICE_REG32_W (DEVICE_EMIF4_BASE + EMIF_REG_RD_WRT_EXC_THRESH, cfg->rdWrtExcThresh);
        
        /* Allow the configuration to occur */
        v = DEVICE_REG32_R (DEVICE_EMIF4_BASE + EMIF_REG_SDRAM_REF_CTL);
        EMIF_REG_VAL_SDRAM_REF_CTL_SET_INITREF_DIS(v,0);
        DEVICE_REG32_W (DEVICE_EMIF4_BASE + EMIF_REG_SDRAM_REF_CTL, v);
        
        if ((cfg->registerMask & ibl_EMIF4_ENABLE_sdRamConfig) != 0)
            DEVICE_REG32_W (DEVICE_EMIF4_BASE + EMIF_REG_SD_RAM_CFG, cfg->sdRamConfig);
        
        if ((cfg->registerMask & ibl_EMIF4_ENABLE_sdRamConfig2) != 0)
            DEVICE_REG32_W (DEVICE_EMIF4_BASE + EMIF_REG_SD_RAM_CFG2, cfg->sdRamConfig2);
        
        v = cfg->sdRamRefreshCtl;
        EMIF_REG_VAL_SDRAM_REF_CTL_SET_INITREF_DIS(v,0);
        DEVICE_REG32_W (DEVICE_EMIF4_BASE + EMIF_REG_SDRAM_REF_CTL, v);
    }
    
    return (0);

} /* hwEmif4p0Enable */


