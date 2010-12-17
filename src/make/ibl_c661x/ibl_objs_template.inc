/* ibl_objs_template.inc
 *
 * list of oject files tagged with the endian field for replacement during make
 */

../main/c64x/make/iblmain.ENDIAN_TAG.oc
../device/c64x/make/c661x.ENDIAN_TAG.oc
../device/c64x/make/c661xutil.ENDIAN_TAG.oc
../driver/c64x/make/timer.ENDIAN_TAG.oc
../hw/c64x/make/t64.ENDIAN_TAG.oc
../hw/c64x/make/psc.ENDIAN_TAG.oc
../hw/c64x/make/emif4.ENDIAN_TAG.oc
../device/c64x/make/c64x.ENDIAN_TAG.oa


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
../hw/c64x/make/cpsw.ENDIAN_TAG.oc
../hw/c64x/make/qm.ENDIAN_TAG.oc
../hw/c64x/make/cpdma.ENDIAN_TAG.oc
../hw/c64x/make/pa.ENDIAN_TAG.oc
../hw/c64x/make/serdes.ENDIAN_TAG.oc
../hw/c64x/make/sgmii.ENDIAN_TAG.oc
../hw/c64x/make/gmacsl.ENDIAN_TAG.oc
#endif

#ifndef EXCLUDE_NAND
../nandboot/c64x/make/nandboot.ENDIAN_TAG.oc
../hw/c64x/make/nandemif25.ENDIAN_TAG.oc
../driver/c64x/make/nand.ENDIAN_TAG.oc
../ecc/c64x/make/3byte_ecc.ENDIAN_TAG.oc
#endif
