/* ibl_init_objs_template.inc
 *
 *  list of object files tagged with the endian field for replacement during make
 */

../main/c64x/make/iblinit.ENDIAN_TAG.oc
../device/c64x/make/c66xinit.ENDIAN_TAG.oc
../device/c64x/make/c66xutil.ENDIAN_TAG.oc
../device/c64x/make/c64x.ENDIAN_TAG.oa
../hw/c64x/make/pll.ENDIAN_TAG.oc
../hw/c64x/make/cfgpll.ENDIAN_TAG.oc
../hw/c64x/make/cfgpll2.ENDIAN_TAG.oc
../interp/c64x/make/btblwrap.ENDIAN_TAG.oc
../interp/c64x/make/btblpr.ENDIAN_TAG.oc
../interp/c64x/make/gem.ENDIAN_TAG.oc
../hw/c64x/make/psc.ENDIAN_TAG.oc

#ifndef EXCLUDE_I2C
../main/c64x/make/ibliniti2c.ENDIAN_TAG.oc
../hw/c64x/make/i2c.ENDIAN_TAG.oc
#endif

#ifndef EXCLUDE_SPI_NOR
../main/c64x/make/iblinitspinor.ENDIAN_TAG.oc
../hw/c64x/make/spi.ENDIAN_TAG.oc
#endif





