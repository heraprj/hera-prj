#include "ehw.h"
#include "frameReadWrite.h"

//Read
#if SEDPR == 0
int XHwIcap_DeviceReadFrame(XHwIcap *InstancePtr, long Top, long Block, long HClkRow, long MajorFrame, long MinorFrame, u32 *FrameBuffer) {
	u32 Packet;
	u32 Data;
	u32 TotalWords;
	int Status;
	u32 WriteBuffer[READ_FRAME_SIZE];
	u32 Index = 0;

	XASSERT_NONVOID(InstancePtr != NULL);
	XASSERT_NONVOID(InstancePtr->IsReady == XCOMPONENT_IS_READY);
	XASSERT_NONVOID(FrameBuffer != NULL);

	/*
	 * DUMMY and SYNC
	 */
	WriteBuffer[Index++] = XHI_DUMMY_PACKET;
	WriteBuffer[Index++] = XHI_SYNC_PACKET;
	WriteBuffer[Index++] = XHI_NOOP_PACKET;
	WriteBuffer[Index++] = XHI_NOOP_PACKET;

	/*
	 * Reset CRC
	 */
	Packet = XHwIcap_Type1Write(XHI_CMD) | 1;
	WriteBuffer[Index++] = Packet;
	WriteBuffer[Index++] = XHI_CMD_RCRC;
	WriteBuffer[Index++] = XHI_NOOP_PACKET;
	WriteBuffer[Index++] = XHI_NOOP_PACKET;

	/*
	 * Setup CMD register to read configuration
	 */
	Packet = XHwIcap_Type1Write(XHI_CMD) | 1;
	WriteBuffer[Index++] = Packet;
	WriteBuffer[Index++] = XHI_CMD_RCFG;
	WriteBuffer[Index++] = XHI_NOOP_PACKET;
	WriteBuffer[Index++] = XHI_NOOP_PACKET;
	WriteBuffer[Index++] = XHI_NOOP_PACKET;

	/*
	 * Setup FAR register.
	 */
	Packet = XHwIcap_Type1Write(XHI_FAR) | 1;
#if XHI_FAMILY == XHI_DEV_FAMILY_V4 /* Virtex 4 */
	Data = XHwIcap_SetupFarV4(Top, Block, HClkRow,  MajorFrame, MinorFrame);
#elif XHI_FAMILY == XHI_DEV_FAMILY_V5 /* Virtex 5 */
   Data = XHwIcap_SetupFarV5(Top, Block, HClkRow,  MajorFrame, MinorFrame);
#endif
	/* Virtex 4 family device is forced */
	Data = ((Top << 22) | (Block << 19) | (HClkRow << 14) | (MajorFrame << 6) | (MinorFrame << 0));
	WriteBuffer[Index++] = Packet;
	WriteBuffer[Index++] = Data;

	/*
	 * Setup read data packet header.
	 * The frame will be preceeded by a dummy frame, and we need to read one
	 * extra word - see Configuration Guide Chapter 8
	 */
	TotalWords = (InstancePtr->WordsPerFrame << 1) + 1;

	/*
	 * Create Type one packet
	 */
	Packet = XHwIcap_Type1Read(XHI_FDRO) | TotalWords;
	WriteBuffer[Index++] = Packet;
	WriteBuffer[Index++] = XHI_NOOP_PACKET;
	WriteBuffer[Index++] = XHI_NOOP_PACKET;

	/*
	 * Write the data to the FIFO and initiate the transfer of data
	 * present in the FIFO to the ICAP device
	 */
	Status = XHwIcap_DeviceWrite(InstancePtr, (u32 *)&WriteBuffer[0], Index);
	if (Status != XST_SUCCESS)  {
		printf("Errore setup FIFO\n");
		return XST_FAILURE;
	}
	
	/*
	 * Wait till the write is done.
	 */
	while (XHwIcap_IsDeviceBusy(InstancePtr) != FALSE);


	/*
	 * Read the frame of the data including the NULL frame.
	 */
	Status = XHwIcap_DeviceRead(InstancePtr, FrameBuffer, TotalWords);
	if (Status != XST_SUCCESS)  {
		printf("Errore lettura frame\n");
		return XST_FAILURE;
	}

	/*
	 * Send DESYNC command
	 */
	Status = XHwIcap_CommandDesync(InstancePtr);
	if (Status != XST_SUCCESS)  {
		printf("Errore desync\n");
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}
#endif

//Write
#if SEDPR == 0
int XHwIcap_DeviceWriteFrame(XHwIcap *InstancePtr, long Top, long Block, long HClkRow, long MajorFrame, long MinorFrame, u32 *FrameData) {
#else
int XHwIcap_DeviceWriteFrame(long Top, long Block, long HClkRow, long MajorFrame, long MinorFrame, u32 *FrameData) {
#endif
	u32 Data;
#if SEDPR == 0
	u32 Packet;
	u32 TotalWords;
	u32 HeaderWords;
	int Status;
	u32 WriteBuffer[READ_FRAME_SIZE];
	u32 Index = 0;
#else
	u32 i;
#endif

	#if SEDPR == 0
	XASSERT_NONVOID(InstancePtr != NULL);
	XASSERT_NONVOID(InstancePtr->IsReady == XCOMPONENT_IS_READY);
	#endif
	XASSERT_NONVOID(FrameData != NULL);

	//DUMMY and SYNC
#if !SEDPR
	WriteBuffer[Index++] = XHI_DUMMY_PACKET;
	WriteBuffer[Index++] = XHI_SYNC_PACKET;
	WriteBuffer[Index++] = XHI_NOOP_PACKET;
	WriteBuffer[Index++] = XHI_NOOP_PACKET;
	//Reset CRC
	Packet = XHwIcap_Type1Write(XHI_CMD) | 1;
	Data = XHI_CMD_RCRC;
	WriteBuffer[Index++] = Packet;
	WriteBuffer[Index++] = Data;
	WriteBuffer[Index++] = XHI_NOOP_PACKET;
	WriteBuffer[Index++] = XHI_NOOP_PACKET;
	Packet = XHwIcap_Type1Write(XHI_COR) | 1;
	Data = 0x10042FDD;
	WriteBuffer[Index++] = Packet;
	WriteBuffer[Index++] = Data;
	//ID register
	Packet = XHwIcap_Type1Write(XHI_IDCODE) | 1;
	Data = InstancePtr->DeviceIdCode;
	WriteBuffer[Index++] = Packet;
	WriteBuffer[Index++] = Data;
#endif
	//Setup FAR
	Data = XHwIcap_SetupFarV4(Top, Block, HClkRow,  MajorFrame, MinorFrame);
	Data = ((Top << 22) | (Block << 19) | (HClkRow << 14) | (MajorFrame << 6) | (MinorFrame << 0));
#if SEDPR
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 52, Data);
#else
	Packet = XHwIcap_Type1Write(XHI_FAR) | 1;
	WriteBuffer[Index++] = Packet;
	WriteBuffer[Index++] = Data;
	//Setup CMD register - write configuration
	Packet = XHwIcap_Type1Write(XHI_CMD) | 1;
	Data = XHI_CMD_WCFG;
	WriteBuffer[Index++] = Packet;
	WriteBuffer[Index++] = Data;
	WriteBuffer[Index++] = XHI_NOOP_PACKET;
	//Setup Packet header.
	TotalWords = InstancePtr->WordsPerFrame << 1;
	//Create Type 1 Packet.
	Packet = XHwIcap_Type1Write(XHI_FDRI) | TotalWords;
	WriteBuffer[Index++] = Packet;
	HeaderWords = 18;
#endif
	//Write the Header data into the FIFO and intiate the transfer of
	//data present in the FIFO to the ICAP device
#if SEDPR == 0

	Status = XHwIcap_DeviceWrite(InstancePtr, (u32 *)&WriteBuffer[0], HeaderWords);
	if (Status != XST_SUCCESS)  {
		return XST_FAILURE;
	}

#endif
	//Write the modified frame data.
#if SEDPR
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 72, FrameData[42]);
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 76, FrameData[43]);
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 80, FrameData[44]);
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 84, FrameData[45]);
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 88, FrameData[46]);
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 92, FrameData[47]);
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 96, FrameData[48]);
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 100, FrameData[49]);
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 104, FrameData[50]);
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 108, FrameData[51]);
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 112, FrameData[52]);
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 116, FrameData[53]);
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 120, FrameData[54]);
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 124, FrameData[55]);
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 128, FrameData[56]);
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 132, FrameData[57]);
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 136, FrameData[58]);
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 140, FrameData[59]);
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 144, FrameData[60]);
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 148, FrameData[61]);
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 152, FrameData[62]);
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 156, FrameData[63]);
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 160, FrameData[64]);
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 164, FrameData[65]);
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 168, FrameData[66]);
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 172, FrameData[67]);
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 176, FrameData[68]);
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 180, FrameData[69]);
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 184, FrameData[70]);
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 188, FrameData[71]);
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 192, FrameData[72]);
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 196, FrameData[73]);
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 200, FrameData[74]);
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 204, FrameData[75]);
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 208, FrameData[76]);
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 212, FrameData[77]);
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 216, FrameData[78]);
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 220, FrameData[79]);
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 224, FrameData[80]);
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 228, FrameData[81]);
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 232, FrameData[82]);
#else
	Status = XHwIcap_DeviceWrite(InstancePtr, (u32 *) &FrameData[InstancePtr->WordsPerFrame + 1], InstancePtr->WordsPerFrame);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
	//Write out the pad frame. The pad frame was read from the device before
	//the data frame.
	Status = XHwIcap_DeviceWrite(InstancePtr, (u32 *) &FrameData[1], InstancePtr->WordsPerFrame);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
	//Add CRC
	Index = 0;
	Packet = XHwIcap_Type1Write(XHI_CRC) | 1;
	WriteBuffer[Index++] = Packet;
	WriteBuffer[Index++] = XHI_DISABLED_AUTO_CRC;
	//Park the FAR
	Packet = XHwIcap_Type1Write(XHI_FAR) | 1;
	Data = XHwIcap_SetupFarV4(0, 0, 3, 33, 0);
	WriteBuffer[Index++] = Packet;
	WriteBuffer[Index++] =  Data;
	//Add CRC
	Packet = XHwIcap_Type1Write(XHI_CRC) | 1;
	WriteBuffer[Index++] = Packet;
	WriteBuffer[Index++] = XHI_DISABLED_AUTO_CRC;
	//Intiate the transfer of data present in the FIFO to
	//the ICAP device
	Status = XHwIcap_DeviceWrite(InstancePtr, &WriteBuffer[0], Index);
	if (Status != XST_SUCCESS)  {
		return XST_FAILURE;
	}
	//Send DESYNC command
	Index = 0;
	//Create the data to be written to the ICAP.
	WriteBuffer[Index++] = (XHwIcap_Type1Write(XHI_CMD) | 1);
	WriteBuffer[Index++] = XHI_CMD_DESYNCH;
	WriteBuffer[Index++] = XHI_DUMMY_PACKET;
	WriteBuffer[Index++] = XHI_DUMMY_PACKET;
#endif

#if SEDPR
	//Write the data to the FIFO and intiate the transfer of data present
	//in the FIFO to the ICAP device.
	while(SEDPR_CNTLR_mReadSlaveReg1(XPAR_SEDPR_CNTLR_0_BASEADDR, 0));
	SEDPR_CNTLR_mWriteSlaveReg0(XPAR_SEDPR_CNTLR_0_BASEADDR, 0, 0xFFFFFFFF);
	while(!SEDPR_CNTLR_mReadSlaveReg1(XPAR_SEDPR_CNTLR_0_BASEADDR, 0));
	SEDPR_CNTLR_mWriteSlaveReg0(XPAR_SEDPR_CNTLR_0_BASEADDR, 0, ALL_ZEROS);

	if(DEBUG_LEVEL >= 3) {
		for(i=0; i<110; i++) {
			printf("Index: %d, Word: %08X\n",(int)i, (unsigned int)SEDPR_CNTLR_mReadMemory(i*4 + XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR));
		}
	}
#else
	//Write the final data to the ICAP
	Status = XHwIcap_DeviceWrite(InstancePtr, &WriteBuffer[0], Index);
	if (Status != XST_SUCCESS)  {
		return XST_FAILURE;
	}
#endif

	return XST_SUCCESS;
}

#if SEDPR
void BitstreamInit() {
	u32 Packet;
	u32 Data;
	u32 TotalWords;
	//DUMMY and SYNC
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR, XHI_DUMMY_PACKET);
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 4, XHI_SYNC_PACKET);
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 8, XHI_NOOP_PACKET);
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 12, XHI_NOOP_PACKET);
	//Reset CRC
	Packet = XHwIcap_Type1Write(XHI_CMD) | 1;
	Data = XHI_CMD_RCRC;
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 16, Packet);
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 20, Data);
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 24, XHI_NOOP_PACKET);
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 28, XHI_NOOP_PACKET);
	//Bypass CRC
	Packet = XHwIcap_Type1Write(XHI_COR) | 1;
	Data = 0x10042FDD;
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 32, Packet);
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 36, Data);
	//ID register
	Packet = XHwIcap_Type1Write(XHI_IDCODE) | 1;
	Data = 0x01E58093; //DEVICE ID
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 40, Packet);
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 44, Data);
	//Setup FAR: just the header, the data must be written by the frameWrite
	Packet = XHwIcap_Type1Write(XHI_FAR) | 1;
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 48, Packet);
	Packet = XHwIcap_Type1Write(XHI_CMD) | 1;
	Data = XHI_CMD_WCFG;
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 56, Packet);
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 60, Data);
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 64, XHI_NOOP_PACKET);
	TotalWords = (XHI_NUM_FRAME_BYTES / 4) << 1;
	Packet = XHwIcap_Type1Write(XHI_FDRI) | TotalWords;
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 68, Packet);
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 236, ALL_ZEROS);
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 240, ALL_ZEROS);
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 244, ALL_ZEROS);
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 248, ALL_ZEROS);
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 252, ALL_ZEROS);
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 256, ALL_ZEROS);
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 260, ALL_ZEROS);
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 264, ALL_ZEROS);
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 268, ALL_ZEROS);
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 272, ALL_ZEROS);
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 276, ALL_ZEROS);
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 280, ALL_ZEROS);
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 284, ALL_ZEROS);
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 288, ALL_ZEROS);
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 292, ALL_ZEROS);
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 296, ALL_ZEROS);
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 300, ALL_ZEROS);
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 304, ALL_ZEROS);
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 308, ALL_ZEROS);
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 312, ALL_ZEROS);
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 316, ALL_ZEROS);
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 320, ALL_ZEROS);
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 324, ALL_ZEROS);
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 328, ALL_ZEROS);
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 332, ALL_ZEROS);
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 336, ALL_ZEROS);
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 340, ALL_ZEROS);
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 344, ALL_ZEROS);
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 348, ALL_ZEROS);
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 352, ALL_ZEROS);
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 356, ALL_ZEROS);
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 360, ALL_ZEROS);
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 364, ALL_ZEROS);
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 368, ALL_ZEROS);
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 372, ALL_ZEROS);
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 376, ALL_ZEROS);
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 380, ALL_ZEROS);
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 384, ALL_ZEROS);
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 388, ALL_ZEROS);
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 392, ALL_ZEROS);
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 396, ALL_ZEROS);
	Packet = XHwIcap_Type1Write(XHI_CRC) | 1;
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 400, Packet);
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 404, XHI_DISABLED_AUTO_CRC);
	Packet = XHwIcap_Type1Write(XHI_FAR) | 1;
	Data = XHwIcap_SetupFarV4(0, 0, 3, 33, 0);
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 408, Packet);
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 412, Data);
	Packet = XHwIcap_Type1Write(XHI_CRC) | 1;
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 416, Packet);
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 420, XHI_DISABLED_AUTO_CRC);
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 424, (XHwIcap_Type1Write(XHI_CMD) | 1));
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 428, XHI_CMD_DESYNCH);
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 432, XHI_DUMMY_PACKET);
	SEDPR_CNTLR_mWriteMemory(XPAR_SEDPR_CNTLR_0_MEM0_BASEADDR + 436, XHI_DUMMY_PACKET);
}
#endif
