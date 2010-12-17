;  /** 
;   *  @file  c64x.s
;   *
;   *  @breif
;   *    Assembly functions for c64x compatible devices
;   */

	.text

; ********************************************************************************
; * Provide a rough delay, in CPU cycles.
; * On Entry:
; *   A4 = number of cycles to delay
; ********************************************************************************
	.def _chipDelay32
_chipDelay32:

			ZERO	.L2	  B4
			SHRU	.S1   A4,3,A0
			CMPLTU  .L2X  B4,A4,B0
	[!A0]   BNOP    .S1   cend, 5

	[!B0]   BNOP    .S1   cend, 3
            MVKL    .S1   1,A5
			MVKH	.S1	  1,A5	


dosub:
			SUB 	.L1   A0,A5,A0
			NOP		2
	[A0]	BNOP	.S1   dosub,5



cend:
			RETNOP.S2  B3, 5






