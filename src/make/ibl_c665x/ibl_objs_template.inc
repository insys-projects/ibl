/* ibl_objs_template.inc
 *
 * list of oject files tagged with the endian field for replacement during make
 */

../main/c64x/make/iblmain.ENDIAN_TAG.oc
../device/c64x/make/c665x.ENDIAN_TAG.oc
../device/c64x/make/c665xutil.ENDIAN_TAG.oc
../driver/c64x/make/timer.ENDIAN_TAG.oc
../hw/c64x/make/t64.ENDIAN_TAG.oc
../hw/c64x/make/psc.ENDIAN_TAG.oc
../hw/c64x/make/emif4.ENDIAN_TAG.oc
../device/c64x/make/c64x.ENDIAN_TAG.oa
../hw/c64x/make/pll.ENDIAN_TAG.oc
../hw/c64x/make/cfgpll.ENDIAN_TAG.oc
../hw/c64x/make/cfgpll2.ENDIAN_TAG.oc
../hw/c64x/make/c66x_uart.ENDIAN_TAG.oc

#ifndef EXCLUDE_BIS
../interp/c64x/make/bis.ENDIAN_TAG.oc
#endif

#ifndef EXCLUDE_COFF
../interp/c64x/make/cload.ENDIAN_TAG.oc
../interp/c64x/make/osal.ENDIAN_TAG.oc
#endif

#ifndef EXCLUDE_BLOB
../interp/c64x/make/blob.ENDIAN_TAG.oc
#endif

#ifndef EXCLUDE_ELF
../interp/c64x/make/dload.ENDIAN_TAG.oc
../interp/c64x/make/elfwrap.ENDIAN_TAG.oc
../interp/c64x/make/dlw_client.ENDIAN_TAG.oc
../interp/c64x/make/dload_endian.ENDIAN_TAG.oc
../interp/c64x/make/ArrayList.ENDIAN_TAG.oc
#endif


#ifndef EXCLUDE_ETH
../ethboot/c64x/make/ethboot.ENDIAN_TAG.oc
../driver/c64x/make/net.ENDIAN_TAG.oc
../driver/c64x/make/arp.ENDIAN_TAG.oc
../driver/c64x/make/ip.ENDIAN_TAG.oc
../driver/c64x/make/udp.ENDIAN_TAG.oc
../driver/c64x/make/stream.ENDIAN_TAG.oc
../driver/c64x/make/bootp.ENDIAN_TAG.oc
../driver/c64x/make/tftp.ENDIAN_TAG.oc
../hw/c64x/make/mdio.ENDIAN_TAG.oc
../hw/c64x/make/serdes.ENDIAN_TAG.oc
../hw/c64x/make/sgmii.ENDIAN_TAG.oc
../hw/c64x/make/cpmacdrv.ENDIAN_TAG.oc
#endif


/* -------------- */
#if (!defined(EXCLUDE_NAND_EMIF) || !defined(EXCLUDE_NAND_SPI) || !defined(EXCLUDE_NAND_GPIO))

../nandboot/c64x/make/nandboot.ENDIAN_TAG.oc
../driver/c64x/make/nand.ENDIAN_TAG.oc
../ecc/c64x/make/3byte_ecc.ENDIAN_TAG.oc

 #ifndef EXCLUDE_NAND_EMIF
 ../hw/c64x/make/nandemif25.ENDIAN_TAG.oc
 #endif

 #ifndef EXCLUDE_NAND_GPIO
 ../hw/c64x/make/gpio.ENDIAN_TAG.oc
 ../hw/c64x/make/nandgpio.ENDIAN_TAG.oc
 #endif
 
 #ifndef EXCLUDE_NAND_SPI
  ../hw/c64x/make/nandspi.ENDIAN_TAG.oc
 #endif

#endif
/* -------------- */



/* -------------- */
#if (!defined(EXCLUDE_NOR_EMIF) || !defined(EXCLUDE_NOR_SPI))

../norboot/c64x/make/norboot.ENDIAN_TAG.oc
../driver/c64x/make/nor.ENDIAN_TAG.oc
../hw/c64x/make/emif25.ENDIAN_TAG.oc

 #ifndef EXCLUDE_NOR_SPI
 ../hw/c64x/make/norspi.ENDIAN_TAG.oc
 #endif

 #ifndef EXCLUDE_NOR_EMIF
 ../hw/c64x/make/noremif25.ENDIAN_TAG.oc
 #endif

#endif
/* -------------- */


#if (!defined(EXCLUDE_NOR_SPI) || !defined(EXCLUDE_NAND_SPI))
../hw/c64x/make/spi.ENDIAN_TAG.oc
#endif
