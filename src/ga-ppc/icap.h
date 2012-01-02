/*
 * @title Hera Project
 * @author Davide Bartolini
 * @author Fabio Cancare
 * @author Matteo Carminati
 */

#ifndef ICAP_H_
#define ICAP_H_

#include "ehw.h"

//device ID
#define HWICAP_DEVICEID XPAR_HWICAP_0_DEVICE_ID

//frame size (32bit words)
#define FRAMESIZE		XHI_NUM_FRAME_WORDS

//frame size including a dummy frame (32bit words)
#define FBUFFERSIZE		XHI_NUM_WORDS_FRAME_INCL_NULL_FRAME

void computeFrame(Part *part, int x, int y, int32 *frameBuffer);
byte reverseByte(byte input);
int32 reverseWord(int32 input);
long computeTB(int x, int y);
long computeRow(int x, int y);
long computeMajor(int x, int y);
long computeMinor(int x, int y); 
int computeBitStart(int y, int lutType);
int32 computeCWord(int y, Part *part);
int32 computeCWordBit(int y, int lutType, int bitPos);

#if SEDPR == 0
int initIcap(XHwIcap *HwIcap);
void printFrame(XHwIcap *HwIcap, int x, int y);
int deployToFPGA(XHwIcap *HwIcap, Candidate *candidate, const int *x, const int *y);
int frameCheck(int32 *logico, XHwIcap *HwIcap , int x, int y);
#else
int deployToFPGA(Candidate *candidate, const int *x, const int *y);
#endif

#endif /*ICAP_H_*/
