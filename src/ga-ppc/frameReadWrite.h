#ifndef FRAMEREADWRITE_H_
#define FRAMEREADWRITE_H_

#define READ_FRAME_SIZE 20

#if SEDPR == 0
int XHwIcap_DeviceReadFrame(XHwIcap *InstancePtr, long Top, long Block, long HClkRow, long MajorFrame, long MinorFrame, u32 *FrameBuffer);
int XHwIcap_DeviceWriteFrame(XHwIcap *InstancePtr, long Top, long Block, long HClkRow, long MajorFrame, long MinorFrame, u32 *FrameData);
#else
int XHwIcap_DeviceWriteFrame(long Top, long Block, long HClkRow, long MajorFrame, long MinorFrame, u32 *FrameData);
void BitstreamInit();
#endif

#endif /*FRAMEREADWRITE_H_*/
