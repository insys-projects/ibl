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

#define DDR3PLLCTL0		*(volatile unsigned int*)(CHIP_LEVEL_REG + 0x0330)
#define DDR3PLLCTL1		*(unsigned int*)(CHIP_LEVEL_REG + 0x0334)

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
#define DDR3_CONFIG_REG_13  (*(volatile unsigned int*)(0x02620460))
#define DDR3_CONFIG_REG_23  (*(volatile unsigned int*)(0x02620460))
#define DDR3_CONFIG_REG_24  (*(volatile unsigned int*)(0x02620464))

#define RD_DQS_SLAVE_RATIO 0x34
#define WR_DQS_SLAVE_RATIO 0xA9
#define WR_DATA_SLAVE_RATIO 0xE9
#define FIFO_WE_SLAVE_RATIO 0x106


static void ddr3_wait (uint32 del)
{
    volatile unsigned int i;

    for (i = 0; i < del; i++);

}

/*************************************************************************************************
 * FUNCTION PUROPSE: Initial EMIF4 setup
 *************************************************************************************************
 * DESCRIPTION: Emif configuration
 *************************************************************************************************/
SINT16 hwEmif4p0Enable (iblEmif4p0_t *cfg)
{
    UINT32 v, i;
    
    v = DEVICE_REG32_R(DEVICE_JTAG_ID_REG);
    
    if (v == DEVICE_C6618_JTAG_ID_VAL)
    {
#if 0
        /* C6678 Old 1066 configuration */
        DDR3PLLCTL0 = 0x100807C1;
        
        DDR_SDTIM1   = 0x0CCF369B;
        DDR_SDTIM2   = 0x3A3F7FDA;
        DDR_SDTIM3   = 0x057F83A8;
        DDR_PMCTL   |= (0x9 << 4); // Set up SR_TIM to Enter self-refresh after 4096 clocks
        
        DDR_DDRPHYC  = 0x0010010B;
        
        DDR_SDRFC = 0x00004111; //500us
        
        
        DDR_SDCFG    = 0x63C51A32; //0x63C51A32;    //row-col = 13-10
        
        
        //Values with invertclkout = 0
        DATA0_GTLVL_INIT_RATIO = 0x3C;
        DATA1_GTLVL_INIT_RATIO = 0x3C;
        DATA2_GTLVL_INIT_RATIO = 0x23;
        DATA3_GTLVL_INIT_RATIO = 0x2D;
        DATA4_GTLVL_INIT_RATIO = 0x13;
        DATA5_GTLVL_INIT_RATIO = 0x11;
        DATA6_GTLVL_INIT_RATIO = 0x9;
        DATA7_GTLVL_INIT_RATIO = 0xC;
        //DATA8_GTLVL_INIT_RATIO = 0x21; //ECC byte lane. Don't care as long as you don't enable ECC by software
        
        //Values with invertclkout = 0
        RDWR_INIT_RATIO_0 = 0x0;
        RDWR_INIT_RATIO_1 = 0x0;
        RDWR_INIT_RATIO_2 = 0x0;
        RDWR_INIT_RATIO_3 = 0x0;
        RDWR_INIT_RATIO_4 = 0x0;
        RDWR_INIT_RATIO_5 = 0x0;
        RDWR_INIT_RATIO_6 = 0x0;
        RDWR_INIT_RATIO_7 = 0x0;
        //RDWR_INIT_RATIO_8 = 0x0; //ECC byte lane. Don't care as long as you don't enable ECC by software
        
        
        
        //GEL_TextOut("\nProgrammed initial ratios.\n");
        
        DDR3_CONFIG_REG_0 = DDR3_CONFIG_REG_0 | 0xF;
        
        //DDR3_CONFIG_REG_23 = RD_DQS_SLAVE_RATIO_1066 | (WR_DQS_SLAVE_RATIO_1066 << 10) | (WR_DATA_SLAVE_RATIO_1066 << 20);
        
        DDR3_CONFIG_REG_23 |= 0x00000200; //Set bit 9 = 1 to use forced ratio leveling for read DQS
        //GEL_TextOut("\nSet bit 9 = 1 for forced ratio read eye leveling.\n");
        
        DDR_RDWR_LVL_RMP_CTRL = 0x80000000; //enable full leveling
        DDR_RDWR_LVL_CTRL = 0x80000000; //Trigger full leveling - This ignores read DQS leveling result and uses ratio forced value 							//(0x34) instead
        //GEL_TextOut("\n Triggered full leveling.\n");
        
        DDR_SDTIM1; //Read MMR to ensure full leveling is complete
        
        DDR_SDRFC    = 0x00001040; //Refresh rate = Round[7.8*666.5MHz] = 0x1450
#endif
#if 1
        /* C6678 1333 MHz data rate */
        /***************** 2.2 DDR3 PLL Configuration ************/
        DDR3PLLCTL1 |= 0x00000040;      //Set ENSAT bit = 1
        DDR3PLLCTL1 |= 0x00002000;      //Set RESET bit = 1
        DDR3PLLCTL0 = 0x090804C0;       //Configure CLKR, CLKF, CLKOD, BWADJ
        ddr3_wait(1000);               //Wait for reset to complete
        DDR3PLLCTL1 &= ~(0x00002000);   //Clear RESET bit
        ddr3_wait(1000);              //Wait for PLL lock
        
        /***************** 2.3 Basic Controller and DRAM configuration ************/
        DDR_SDRFC    = 0x80005162;    // inhibit configuration 
        
        DDR_SDTIM1   = 0x1113783C;
        DDR_SDTIM2   = 0x304F7FE3;
        DDR_SDTIM3   = 0x559F849F;
        
        DDR_DDRPHYC  = 0x0010010F;
        
        DDR_ZQCFG    = 0x70073214;
        
        DDR_PMCTL    = 0x0;
        
        DDR_SDRFC    = 0x00005162;    // enable configuration
        DDR_SDCFG    = 0x63222A32;    // last config write – DRAM init occurs
        
        ddr3_wait(1000);            //Wait for HW init to complete
        DDR_SDRFC = 0x00001450;       //Refresh rate = (7.8*666MHz]
        
        /**************** 3.0 Leveling Register Configuration ********************/
        /* Using partial automatic leveling due to errata */
        
        /**************** 3.2 Invert Clock Out ********************/
        DDR3_CONFIG_REG_0 &= ~(0x007FE000);  // clear ctrl_slave_ratio field
        DDR3_CONFIG_REG_0 |= 0x00200000;     // set ctrl_slave_ratio to 0x100
        DDR3_CONFIG_REG_12 |= 0x08000000;    // Set invert_clkout = 1
        DDR3_CONFIG_REG_0 |= 0xF;            // set dll_lock_diff to 15
        
        
        /**************** 3.3+3.4 Partial Automatic Leveling ********************/
        DATA0_WRLVL_INIT_RATIO = 0x20;
        DATA1_WRLVL_INIT_RATIO = 0x24;
        DATA2_WRLVL_INIT_RATIO = 0x3A;
        DATA3_WRLVL_INIT_RATIO = 0x38;
        DATA4_WRLVL_INIT_RATIO = 0x51;
        DATA5_WRLVL_INIT_RATIO = 0x5E;
        DATA6_WRLVL_INIT_RATIO = 0x5E;
        DATA7_WRLVL_INIT_RATIO = 0x5E;
        DATA8_WRLVL_INIT_RATIO = 0x44;
        
        DATA0_GTLVL_INIT_RATIO = 0xA1;
        DATA1_GTLVL_INIT_RATIO = 0x9E;
        DATA2_GTLVL_INIT_RATIO = 0xA7;
        DATA3_GTLVL_INIT_RATIO = 0xA9;
        DATA4_GTLVL_INIT_RATIO = 0xCA;
        DATA5_GTLVL_INIT_RATIO = 0xBE;
        DATA6_GTLVL_INIT_RATIO = 0xDD;
        DATA7_GTLVL_INIT_RATIO = 0xDD;
        DATA8_GTLVL_INIT_RATIO = 0xBA;
        
        DDR3_CONFIG_REG_23 |= 0x00000200;
        DDR_RDWR_LVL_RMP_CTRL = 0x80000000;
        DDR_RDWR_LVL_CTRL = 0x80000000;
#endif
        
#if 0
        /* C6678 New 1066 rate */
        /***************** 2.2 DDR3 PLL Configuration ************/
        DDR3PLLCTL1 |= 0x00000040;      //Set ENSAT bit = 1
        DDR3PLLCTL1 |= 0x00002000;      //Set RESET bit = 1
        DDR3PLLCTL0 = 0x0F0807C1;       //Configure CLKR, CLKF, CLKOD, BWADJ
        ddr3_wait(1000);              //Wait for reset to complete
        DDR3PLLCTL1 &= ~(0x00002000);   //Clear RESET bit
        ddr3_wait(1000);
        
        /***************** 2.3 Basic Controller and DRAM configuration ************/
        DDR_SDRFC    = 0x8000411B;    // inhibit configuration 
        DDR_SDTIM1   = 0x0CCF36B3;
        DDR_SDTIM2   = 0x303F7FDA;
        DDR_SDTIM3   = 0x559F83AF;
        
        DDR_DDRPHYC  = 0x0010010A;
        DDR_ZQCFG    = 0x70073214;
        
        //DDR_PMCTL    = 0x0;
        DDR_SDRFC    = 0x0000411B;    // enable configuration
        DDR_SDCFG    = 0x63211A32;    // last config write – DRAM init occurs
        
        ddr3_wait(1000);             //Wait for HW init to complete
        
        DDR_SDRFC = 0x00001040;       //Refresh rate = (7.8*666MHz]
        
        /**************** 3.0 Leveling Register Configuration ********************/
        /* Using partial automatic leveling due to errata */
        
        /**************** 3.2 Invert Clock Out ********************/
        DDR3_CONFIG_REG_0 &= ~(0x007FE000);  // clear ctrl_slave_ratio field
        DDR3_CONFIG_REG_0 |= 0x00200000;     // set ctrl_slave_ratio to 0x100
        DDR3_CONFIG_REG_12 |= 0x08000000;    // Set invert_clkout = 1
        DDR3_CONFIG_REG_0 |= 0xF;            // set dll_lock_diff to 15
        
        /**************** 3.3+3.4 Partial Automatic Leveling ********************/
        DATA0_WRLVL_INIT_RATIO = 0x19;//0x4C;
        DATA1_WRLVL_INIT_RATIO = 0x1C;//0x4C;
        DATA2_WRLVL_INIT_RATIO = 0x2F;//0x4C;
        DATA3_WRLVL_INIT_RATIO = 0x2D;//0x42;
        DATA4_WRLVL_INIT_RATIO = 0x42;//0x2D;
        DATA5_WRLVL_INIT_RATIO = 0x4C;//0x2F;
        DATA6_WRLVL_INIT_RATIO = 0x4C;//0x1C;
        DATA7_WRLVL_INIT_RATIO = 0x4C;//0x19;
        DATA8_WRLVL_INIT_RATIO = 0x37;
        
        DATA0_GTLVL_INIT_RATIO = 0x8D;//0xBC;
        DATA1_GTLVL_INIT_RATIO = 0x8A;//0xBC;
        DATA2_GTLVL_INIT_RATIO = 0x91;//0xA4;
        DATA3_GTLVL_INIT_RATIO = 0x93;//0xAE;
        DATA4_GTLVL_INIT_RATIO = 0xAE;//0x93;
        DATA5_GTLVL_INIT_RATIO = 0xA4;//0x91;
        DATA6_GTLVL_INIT_RATIO = 0xBC;//0x8A;
        DATA7_GTLVL_INIT_RATIO = 0xBC;//0x8D;
        DATA8_GTLVL_INIT_RATIO = 0xA1;
        
        DDR3_CONFIG_REG_23 |= 0x00000200;
        DDR_RDWR_LVL_RMP_CTRL = 0x80000000;
        DDR_RDWR_LVL_CTRL = 0x80000000;
#endif
    }
    else if (v == DEVICE_C6616_JTAG_ID_VAL)
    {
        /* C6670 800 M rate */
        DDR3PLLCTL1 |= 0x00000040;    //Set ENSAT = 1
        DDR3PLLCTL1 |= 0x00002000;    //Set RESET bit before programming DDR3PLLCTL0
        DDR3PLLCTL0 = 0x02000140;
        
        for(i=0;i<1000;i++); //Wait atleast 5us
        DDR3PLLCTL1 &= 0xFFFFDFFF;    //Clear RESET bit
        
        DDR_SDRFC    = 0x800030D4;    // inhibit configuration
        
        DDR_SDTIM1   = 0x0AAAE4E3;
        DDR_SDTIM2   = 0x20437FDA;
        DDR_SDTIM3   = 0x559F83FF;
        
        DDR_DDRPHYC  = 0x0010010F;
        
        DDR_SDRFC    = 0x000030D4;    // enable configuration
        
        DDR_SDCFG    = 0x63222AB2;    // DRAM Mode Register writes occur here - 31.25us long refresh periods
        
        DDR3_CONFIG_REG_0 |= 0xF;         // set dll_lock_diff to 15
        DDR3_CONFIG_REG_0 &= 0xFF801FFF;  // clear ctrl_slave_ratio field
        DDR3_CONFIG_REG_0 |= 0x00200000;  // set ctrl_slave_ratio field to 256 since INV_CLKOUT = 1
        
        DDR3_CONFIG_REG_12 |= 0x08000000; // Set INV_CLKOUT = 1
        
        DDR3_CONFIG_REG_23 = RD_DQS_SLAVE_RATIO | (WR_DQS_SLAVE_RATIO << 10) | (WR_DATA_SLAVE_RATIO << 20);
        DDR3_CONFIG_REG_24 = FIFO_WE_SLAVE_RATIO;
        
        
        DDR_SDRFC    = 0x00000C30; //Refresh rate = Round[7.8*400MHz] = 0x0C30
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


