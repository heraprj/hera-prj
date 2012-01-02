/*
 * @title Hera Project
 * @part interaction with the ICAP controller
 * @author Davide Bartolini
 * @author Fabio Cancare
 * @author Matteo Carminati
 */

#include "icap.h"

/*
 * Create the frame bitstream of a candidate solution part
 * It works with parts occupying a single frame (same X)
 *
 * 	Data format
 * 	the two luts are within an int32:
 * 		the lutF 0:15 bit are the int32 31:16 bit
 * 		the lutG 0:15 bit are the int32 15:0 bit
 * 	if T/B == 0:
 * 		the frame is composed of 5-byte block built in the following way:
 * 			[lutF_(n<=8 offset bits)_lutG_(8-n filling bits)]
 * 		starting from cell (x,y), then (x,y + 1), (x,y + 2), ... , (x,y + 31)
 * 	if T/B == 1:
 * 		the frame is composed of 5-byte block built in the following way:
 * 			[(8-n filling bits)_lutG'_(n<=8 offset bits] (mirror structure of T/B == 0)
 * 		starting from cell (x,y + 31), then (x,y + 30), (x,y + 29), ... , (x,y)
 *  a complete frame is composed of 20 words, a central word, 20 words => 16 blocks, central word, 16 blocks
 *  in the frame buffer the frame is written (word by word) in the following way:
 * 		word of the generated frame: B0 _B1 _B2_ B3
 * 		array cell:		   B0'_B1'_B2'_B3' , Bi' is the reverse of Bi (Es: 10001100 instead of 00110001)
 *
 * @param Part part: the part that will be translated to a frame
 * @param int x: X coordinate within the FPGA
 * @param int y: Y coordinate within the FPGA
 * @param int32* frameBuffer: buffer in which the frame is written
 */
void computeFrame(Part *part, int x, int y, int32* frameBuffer) {
	byte buffer[20];
	int32 TB1buffer[41];
	int32 tmp;
	int i, j, k, l, offset, writtenBytes, bufFree, longBufFree;
	int16 lutF, lutG;
	
	offset = computeBitStart(y, LUTF) - computeBitStart(y, LUTG);
	if(DEBUG_LEVEL >= 3) {
		printf("LUTF: %d\n", computeBitStart(y, LUTF));
		printf("LUTG: %d\n", computeBitStart(y, LUTG));
	}
	if(offset < 0) {
		offset = -offset;
	}
	offset = offset - 16;
	writtenBytes = 0;
	bufFree = 0;
	longBufFree = 0;
	for(i = 0; i < MODULES_PER_PART; i++) {
		for(j = 0; j < CELLS_PER_MODULE; j++) {

			lutF = part->modules[i].cells[j].luts >> 16;
			lutG = part->modules[i].cells[j].luts;

			buffer[bufFree++] = lutF >> 8;
			buffer[bufFree++] = lutF;
			buffer[bufFree] = lutG >> 8;
			buffer[bufFree] = buffer[bufFree] >> offset;
			bufFree++;
			buffer[bufFree++] = lutG >> (offset);
			buffer[bufFree] = lutG;
			buffer[bufFree] = buffer[bufFree] << (8 - offset);
			bufFree++;
			if(DEBUG_LEVEL >= 3) {
				printf("lutF:%X ", lutF);
				printf("lutG:%X\n\r", lutG);
			}
			if(bufFree == 20) {
				for(k = 0; k < 20; k = k + 4) {
					frameBuffer[longBufFree] = 0x00000000;
					for(l = 3; l >= 0; l--) {
						frameBuffer[longBufFree] = frameBuffer[longBufFree] << 8;
						frameBuffer[longBufFree] = frameBuffer[longBufFree] | reverseByte(buffer[k + l]);
						if(DEBUG_LEVEL >= 3) {
							printf("%02X", reverseByte(buffer[k + l]));
						}
					}
					if(DEBUG_LEVEL >= 3) {
						printf("\t");
					}

					longBufFree++;
				}
				bufFree = 0;
				writtenBytes += 20;
				if(writtenBytes == 2 * (FRAMESIZE -1)) {
					frameBuffer[longBufFree] = computeCWord(y, part);
					longBufFree++;
				}
			}
		}
	}

	if(y < 64) {
		for(i = 0; i < 41; i++) {
			TB1buffer[i] = frameBuffer[i];
		}
		for(i = 0; i < 41; i++) {
			frameBuffer[i] = reverseWord(TB1buffer[40 - i]);
		}
		frameBuffer[20] = TB1buffer[20];
	}
	return;
}

/*
 * Reverse a byte
 * ES: input = 11100001  ==>  output = 10000111
 * @param 	byte input:	byte to be reversed
 * @returns	byte: reversed byte
 */
byte reverseByte(byte input) {
	int i;
	byte output = 0x00;
	byte mask[8] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};
	for(i = 0; i < 8; i++) {
		if(input & mask[i]) {
			output = output | mask[7 - i];
		}
	}
	return output;
}

int32 reverseWord(int32 input) {
	int i;
	int32 output = 0;
	int32 mask = 0x00000001;
	for(i = 0; i < 32; i++) {
		if(input & (mask << i)) {
			output = output | (mask << (31 - i));
		}
	}
	return output;
}

/*
 * Funzione che compute il valore di Top/Bottom per la slice con coordinate x, y
 * @param int x:			 coordinata x della slice
 * 		  int y:			 coordinata y della slice
 */
long computeTB(int x, int y) {
	if(0 <= y && y <= 63) {
		return 1;
	}
	else return 0;
}

 /*
 * Compute the row value for slice  x, y
 * @param int x: x coordinate of the slice
 * @param int y: y coordinate of the slice
 */
long computeRow(int x, int y) {
	if((0 <= y && y <= 31) || (96 <= y && y <= 127)) {
		return 1;
	}
	else return 0;
}

/*
 * Compute the Major address for slice x, y
 * @param int x: x coordinate of the slice
 * @param int y: y coordinate of the slice
 */
long computeMajor(int x, int y) {
	int major;
	major = x / 2;
	if(0<= x && x<= 23) {
		major++;
	}
	else if(24 <= x && x <= 31) {
		major = major + 3;
	}
	else {
		major = major + 4;
	}
	return major;
}

/*
 * Compute the Minor address for slice x, y
 * @param int x: x coordinate of the slice
 * @param int y: y coordinate of the slice
 */
long computeMinor(int x, int y) {
	if(x % 2 == 0) {
		return 21;
	}
	else {
		return 19;
	}
}

/*
 * Compute the frame bit at which data will be written
 * @param int y: column of the used frame
 * @param int lutType: LUT type (LUTF or LUTG)
 */
int computeBitStart(int y, int lutType) {
	int topByteStart, adjY, adjG, adjLut, byteStart, adjBit, bitStart;
	if(lutType == LUTF) {
		adjLut = 0;
		adjG = 0;
	} 
	else {
		adjLut = 2;
		adjG = 1;	
	}
	if(y % 32 > 15) {
		adjY = 4; 
	}
	else {
		adjY = 0;
	}
	topByteStart = 5 * (y % 32) + adjLut + adjY;
	if(y >= 64) {
		byteStart = topByteStart;
	}
	else {
		byteStart = 162 - topByteStart - adjG;
	}
	if(lutType == LUTG) {
		if(y >= 64) {
			adjBit = 7;
		}
		else {
			adjBit = 1;
		}
	}
	else {
		adjBit = 0;
	}
	bitStart = 8 * byteStart + adjBit;
	return bitStart;
}

/*
 * Compute the central frame word
 * @param int y: column of the used frame
 * @param Individual *part: the part written to the frame
 * @returns int32: the central word
 */
int32 computeCWord(int y, Part *part) {
	int i, j, k, count;
	int32 cWord, temp, mask;
	cWord = 0x00000000;
	for(i = 0; i < MODULES_PER_PART; i++) {
		for(j = 0; j < CELLS_PER_MODULE; j++) {
			for(k = 0; k < LUT_SIZE; k++) {
				if(DEBUG_LEVEL >= 3) {
					printf("lutF_y:%d.%d.%d->%d", y,  i, j, y + CELLS_PER_MODULE * i + j);
				}
				if(getBit(part->modules[i].cells[j], LUTF, LUT_SIZE - 1 - k)) {
					temp = computeCWordBit(y + CELLS_PER_MODULE * i + j, LUTF, k);
					cWord = cWord ^ temp;
				}
				if(DEBUG_LEVEL >= 3) {
					printf("lutG_y:%d.%d.%.d->%d_", y, i, j, y + CELLS_PER_MODULE * i + j);
				}
				if(getBit(part->modules[i].cells[j], LUTG, LUT_SIZE - 1 - k)) {
					temp = computeCWordBit(y + CELLS_PER_MODULE * i + j, LUTG, k);
					cWord = cWord ^ temp;
				}
			}
		}
	}
	//compute the odd parity bit
	mask = 0x00000001;
	count = 0;
	for(i = 0; i < 8 * sizeof(int32); i++) {
		if(cWord & mask) {
			count++;
		}
		mask = mask * 2;
	}
	if(count % 2) {
		cWord = cWord | 0x0000800;
	}
	return cWord;
}

/*
 * Determines how the central word is changed by the value of a single bit of a LUT
 * @param int y: column of the used frame
 * @param int lutType: LUT type (LUTF or LUTG)
 * @param int bitPos: (bit index)
 * @returns int32: new central word
 */
int32 computeCWordBit(int y, int lutType, int bitPos) {
	int32 cWord;
	cWord = (int32)(704 + computeBitStart(y, lutType));
	//if T/B == 0
	if(63 < y && y <= 127) {
		cWord = cWord + (int32)bitPos;
		if(y % 32 > 7) {
			cWord = cWord + (int32)32;
		}
	}
	//if T/B == 1
	if((0 <= y && y <= 63)) {
		cWord = cWord + (int32)(15 - bitPos);
		if(y % 32 <= 23) {
			cWord = cWord - (int32)32;
		}
	}
	return cWord;
}

/*
 * Init ICAP
 * @param XHwIcap ICAP instance
 * @return int: the outcome of the operation
 */
#if SEDPR == 0
int initIcap(XHwIcap *HwIcap) {
	int Status;
	XHwIcap_Config *ConfigPtr;
	ConfigPtr = XHwIcap_LookupConfig(HWICAP_DEVICEID);
	if (ConfigPtr == NULL) {
		printf("ICAP init error\n\r");
		return XST_FAILURE;
	}
	
	//Init configuration
	Status = XHwIcap_CfgInitialize(HwIcap, ConfigPtr, ConfigPtr->BaseAddress);
	if (Status != XST_SUCCESS) {
		printf("ICAP configuration error, status: %d\n\r", Status);
		return XST_FAILURE;
	}
	//Test functioning
	Status = XHwIcap_SelfTest(HwIcap);
	if (Status != XST_SUCCESS) {
		printf("ICAP failure, status:\n\r, %d", Status);
		return XST_FAILURE;
	}
	return 0;
}
#endif

/*
  * Read a frame from the FPGA and print its content
  * @param	XHwIcap *HwIcap: ICAP instance
  * @param int x: frame x coordinate
  * @param int y: frame y coordinate
  */
#if SEDPR == 0
void printFrame(XHwIcap *HwIcap, int x, int y) {
	u32 frameBuffer[FBUFFERSIZE];
	int i, Status;
	printf("reading T/B:%ld, R:%ld, M:%ld, m:%ld\n", computeTB(x, y), computeRow(x, y), computeMajor(x, y), computeMinor(x, y));
	Status = XHwIcap_DeviceReadFrame(HwIcap, computeTB(x, y), 0, computeRow(x, y), computeMajor(x, y), computeMinor(x, y), (u32 *)&frameBuffer[0]);
	if (Status != XST_SUCCESS) {
		printf("Frame reading error, Status: %d \n", Status);
		return;
	}
	printf("Frame content: \n");
	for (i = FRAMESIZE + 1; i < FBUFFERSIZE ; i++) {
		printf("%d -> %08X    ", (i - FRAMESIZE), (unsigned int)frameBuffer[i]);
		if(i % 8 == 0) {
			printf("\n");
		}
	}
}
#endif

/*
 * Deploy a candidate solution to the FPGA
 * @param XHwIcap *HwIcap:		ICAP instance
 * @param Candidate *candidate:	candidate solution to be deployed
 * @param int x: frame x coordinates
 * @param int y: frame y coordinates
 * @returns int: the outcome of the operation
 */
#if SEDPR == 0
int deployToFPGA(XHwIcap *HwIcap, Candidate *candidate, const int* x, const int* y) {
#else
int deployToFPGA(Candidate *candidate, const int *x, const int *y) {
#endif
	int32 frameBuffer[FRAMESIZE];
	u32 writeBuffer[FBUFFERSIZE];
	int i, j, k, l, Status;

	for(i = 0; i < PARTS_PER_CANDIDATE_SOLUTION; i++) {
		//Reset the write buffer
		for(j = 0; j < FBUFFERSIZE; j++) {
			writeBuffer[j] = ALL_ZEROS;
		}
		//Generate the part bitstream
		computeFrame(&candidate->parts[i], x[i], y[i], frameBuffer);
		if(DEBUG_LEVEL >= 2) {
			for(k = 0; k < MODULES_PER_PART; k++) {
				for(l = 0; l < CELLS_PER_MODULE; l++) {
					printf("0x%08X", candidate->parts[0].modules[k].cells[l].luts);
					if(k < MODULES_PER_PART - 1 || l < CELLS_PER_MODULE - 1) {
						printf(", ");
					}
				}
			}
			printf("\n\n");
		}
#if SEDPR == 0
		//Read the actual frame content
		if(DEBUG_LEVEL >= 2) {
			printf("TB:%ld Row:%ld Maj:%ld, Min:%ld\n\r", computeTB(x[i], y[i]), computeRow(x[i], y[i]), computeMajor(x[i], y[i]), computeMinor(x[i], y[i]));
		}
		Status = XHwIcap_DeviceReadFrame(HwIcap, computeTB(x[i], y[i]), 0, computeRow(x[i], y[i]), computeMajor(x[i], y[i]), computeMinor(x[i], y[i]), (u32 *)&writeBuffer[0]);
		if (Status != XST_SUCCESS) {
			printf("Frame reading error, Status: %d \n\r", Status);
			return XST_FAILURE;
		}
#endif
		//Compute the frame buffer
		for (j = FRAMESIZE + 1; j < FBUFFERSIZE ; j++) {
			writeBuffer[j] = (u32) frameBuffer[j - (FRAMESIZE + 1)];
		};
		//Write the frame
#if SEDPR == 0
		Status = XHwIcap_DeviceWriteFrame(HwIcap, computeTB(x[i], y[i]), 0, computeRow(x[i], y[i]), computeMajor(x[i], y[i]), computeMinor(x[i], y[i]), (u32 *)&writeBuffer[0]);
#else
		Status = XHwIcap_DeviceWriteFrame(computeTB(x[i], y[i]), 0, computeRow(x[i], y[i]), computeMajor(x[i], y[i]), computeMinor(x[i], y[i]), (u32 *)&writeBuffer[0]);
#endif
		if (Status != XST_SUCCESS) {
			printf("Frame writing error, Status: %d \n\r", Status);
			return XST_FAILURE;
		}
#if SEDPR == 0
		if(DEBUG_LEVEL >= 0) {
			//check if the frame has been correctly written
			frameCheck(frameBuffer, HwIcap, x[i], y[i]);
		}
#endif
	}
	return XST_SUCCESS;
}

/*
 * Check if a given frame logic has been correctly written to the FPGA
 * @param int32 *givenFrame: frame in memory
 * @param XHwIcap *HwIcap: ICAP instance
 * @param int x: frame X coordinates
 * @param int y: frame Y coordinates
 * @returns	int diffCount: number of differences found
 */
#if SEDPR == 0
int frameCheck(int32 *givenFrame, XHwIcap *HwIcap , int x, int y) {
	int i, j;
	static u32 readFrame[FBUFFERSIZE];
	int diffCount;
	int32 mask;
	XHwIcap_DeviceReadFrame(HwIcap, computeTB(x, y), 0, computeRow(x, y), computeMajor(x, y), computeMinor(x, y), (u32 *)&readFrame[0]);
	diffCount = 0;
	for(i = 0; i < FRAMESIZE; i++) {
		if(givenFrame[i] != (int32)readFrame[i + FRAMESIZE + 1]) {
			for(j = 0; j < 32; j++) {
				mask = 1 << j;
				if ((mask & givenFrame[i]) != (mask & readFrame[i + FRAMESIZE + 1])) {
					diffCount++;
					printf("Found a difference in bit %d of word %d\n\r", j, i);
					printf("given: %08X _-_ read: %08lX\n\r", givenFrame[i], readFrame[i + FRAMESIZE + 1]);
				}		
			}
		}
	}
	return diffCount;
}
#endif
