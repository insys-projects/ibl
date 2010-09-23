#ifndef _NANDGPIOLOC_H
#define _NANDGPIOLOC_H
/**
 *  @file nandgpioloc.h
 *
 *  @brief
 *		Definitions/prototypes local to the nand gpio
 */




void ndelay(Uint32 uiDelay);
void ptNandWaitRdy(void);
void ptNandWriteDataByte(Uint8 data);
void ptNandAleSet(Uint32 addr);
void ptNandCmdSet(Uint32 cmd);
Uint32 swapBytes (Uint32 v);







#endif


