--- uboot/vendors/ami/r2c/r2c.h	1969-12-31 19:00:00.000000000 -0500
+++ uboot.console/vendors/ami/r2c/r2c.h	2006-10-04 11:41:08.126459770 -0400
@@ -0,0 +1,426 @@
+#ifndef _AMI_R2C_H_
+#define _AMI_R2C_H_
+
+# define R2C_TIMEOUT 	5		/* In Seconds */
+# define R2C_RETRYCOUNT	3		
+# define R2C_UDP_PORT		6572
+# define R2C_TERMINAL_PORT	(R2C_UDP_PORT+1)
+# define R2C_DEVICE_PORT	(R2C_UDP_PORT+2)
+
+typedef struct
+{
+	unsigned long 	SeqNo;
+	unsigned short 	PktType;
+	unsigned short	PktLen;
+	unsigned long	Crc32;
+} __attribute__((packed)) R2C_Header;
+
+
+typedef struct
+{
+	R2C_Header Header;
+	unsigned short DeviceUDPPort;
+	unsigned char  UniqueId[6];				/* Fill the MAC Address */
+} __attribute__((packed)) R2C_InitiatePkt;
+
+typedef struct 
+{
+	R2C_Header Header;
+	unsigned short ConsoleUDPPort;
+	unsigned char UniqueId[6];
+	unsigned char IpVer;
+	unsigned char ConsoleIP[6];
+} __attribute__((packed)) R2C_AckInitiatePkt;
+
+
+typedef struct 
+{
+	R2C_Header Header;
+	unsigned short Unused;	
+	unsigned char UniqueId[6];
+} __attribute__((packed)) R2C_NakInitiatePkt;
+
+
+typedef struct
+{
+	R2C_Header Header;
+	unsigned char IpVer;
+	unsigned char ConsoleIP[6];
+	unsigned char DeviceMacAddr[6];
+} __attribute__((packed)) R2C_AddrReqPkt;
+
+typedef struct
+{
+	R2C_Header Header;
+	unsigned char DeviceMacAddr[6];
+} __attribute__((packed)) R2C_NakAddrReqPkt;
+	
+typedef struct
+{
+	R2C_Header Header;
+	unsigned char IpVer;
+	unsigned char DeviceIP[6];
+	unsigned char SubnetMask[6];
+	unsigned char Gateway[6];
+	unsigned char DeviceMacAddr[6];
+} __attribute__((packed)) R2C_AckAddrReqPkt;
+
+
+typedef struct
+{
+	R2C_Header Header;
+}  __attribute__((packed)) R2C_HeaderOnlyPkt;
+
+typedef R2C_HeaderOnlyPkt R2C_SessionEstablishPkt;
+typedef R2C_HeaderOnlyPkt R2C_AckSessionEstablishPkt;
+typedef R2C_HeaderOnlyPkt R2C_NakSessionEstablishPkt;
+
+typedef R2C_HeaderOnlyPkt R2C_SessionTerminatePkt;
+typedef R2C_HeaderOnlyPkt R2C_AckSessionTerminatePkt;
+typedef R2C_HeaderOnlyPkt R2C_NakSessionTerminatePkt;
+
+/* R2C Packet Types */
+
+#define R2C_INITIATE					0x0101
+#define R2C_ACK_INITIATE				0x0102
+#define R2C_NAK_INITIATE				0x0103
+
+#define R2C_ADDR_REQUEST				0x0201
+#define R2C_ACK_ADDR_REQUEST			0x0202
+#define R2C_NAK_ADDR_REQUEST			0x0203
+
+#define R2C_SESSION_ESTABLISH			0x0301
+#define R2C_ACK_SESSION_ESTABLISH		0x0302
+#define R2C_NAK_SESSION_ESTABLISH		0x0303
+
+#define R2C_SESSION_TERMINATE			0x0401
+#define R2C_ACK_SESSION_TERMINATE		0x0402
+#define R2C_NAK_SESSION_TERMINATE		0x0403
+
+
+/* R2C State Machine Phases */
+enum r2c_phase
+{ 
+	R2C_PHASE_INITIALIZATION,			/* On Start						*/	
+	R2C_PHASE_ADDR_ASSIGNMENT,			/* On R2C_ACK_INITIATE 			*/
+	R2C_PHASE_SESSION_ESTABLISHMENT,	/* On R2C_ACK_ADDR_REQUEST 		*/
+	R2C_PHASE_SESSION,					/* On R2C_ACK_SESSION_ESTABLISH */
+	R2C_PHASE_SESSION_TERMINATION,		/* On R2C_STOP_SESSION Command 	*/
+};
+
+typedef enum r2c_phase R2C_PHASE;			
+
+/* Session Commands */
+#define R2C_GET_FLASH_INFO		0x0001
+#define R2C_READ_FLASH			0x0002
+#define R2C_WRITE_FLASH			0x0003
+#define R2C_ERASE_FLASH			0x0004
+#define R2C_GET_FMH_INFO		0x0005
+#define R2C_RESERVED_0006		0x0006
+#define R2C_RESERVED_0007		0x0007
+#define R2C_RESERVED_0008		0x0008
+#define R2C_ERASE_COPY_FLASH	0x0009
+#define R2C_PROTECT_FLASH		0x000A
+
+#define R2C_READ_MEMORY			0x0010
+#define R2C_WRITE_MEMORY		0x0011
+#define R2C_COPY_MEMORY			0x0012
+#define R2C_COMPARE_MEMORY		0x0013
+#define R2C_CLEAR_MEMORY		0x0014
+#define R2C_ALLOC_MEMORY		0x0015
+#define R2C_FREE_MEMORY			0x0016
+
+
+#define R2C_SET_MAC_ADDRESS		0x0020
+#define R2C_EXECUTE_PROGRAM		0x0021
+#define R2C_STOP_SESSION		0x0022
+#define R2C_RESET_DEVICE		0x0023
+#define R2C_REMOTE_TERMINAL		0x0024
+#define R2C_GET_VAR				0x0025
+#define R2C_SET_VAR				0x0026
+#define R2C_SET_VAR_NO_SAVE			0x0027
+
+
+/* Session Command Packets */
+typedef R2C_HeaderOnlyPkt R2C_GetFlashInfo;
+typedef R2C_HeaderOnlyPkt R2C_GetFMHInfo;
+
+typedef struct
+{
+	R2C_Header Header;
+	unsigned long  FlashSize;
+	unsigned long  FlashAddr;
+	unsigned long  EraseSize;
+	unsigned short ProductID;
+	unsigned char  FlashWidth;
+	unsigned char  FMHCompliance;
+	unsigned short  EraseBlockCount;
+/*	unsigned char  EraseBlockSize[EraseBlockCount]; */ /* Variable Length */
+} __attribute__((packed)) R2C_GetFlashInfoResponse;
+
+
+typedef struct
+{
+	R2C_Header Header;
+	unsigned long BootBlockSize;
+	unsigned long BootBlockOffset;
+	unsigned short ModuleCount;
+/*  unsigned long ModuleOffset[ModuleCount]; */ /* Variable Length */
+} __attribute__((packed)) R2C_GetFMHInfoResponse;
+
+
+typedef struct 
+{
+	R2C_Header Header;
+	unsigned long Address;
+	unsigned char Width;
+	unsigned short Size;			/* <= 256 */
+} __attribute__((packed)) R2C_ReadFlash;
+
+typedef struct
+{
+	R2C_Header Header;
+	unsigned char Data[256];
+} __attribute__((packed)) R2C_ReadFlashResponse;
+
+typedef struct 
+{
+	R2C_Header Header;
+	unsigned long Address;
+	unsigned char Width;
+	unsigned char Data[256];
+} __attribute__((packed)) R2C_WriteFlash;
+
+typedef struct
+{
+	R2C_Header Header;
+	unsigned short SizeWritten;	
+} __attribute__((packed)) R2C_WriteFlashResponse;
+
+typedef R2C_ReadFlash R2C_ReadMemory;
+typedef R2C_ReadFlashResponse R2C_ReadMemoryResponse;
+typedef R2C_WriteFlash R2C_WriteMemory;
+typedef R2C_WriteFlashResponse R2C_WriteMemoryResponse;
+
+typedef struct 
+{
+	R2C_Header Header;
+	unsigned long BlockNumber;
+} __attribute__((packed)) R2C_EraseFlash;
+
+typedef struct
+{
+	R2C_Header Header;
+	unsigned char Status;
+} __attribute__((packed)) R2C_EraseFlashResponse;
+
+
+typedef struct 
+{
+	R2C_Header Header;
+	unsigned long SrcMemory;
+	unsigned long DestFlash;
+	unsigned long Size;
+} __attribute__((packed)) R2C_EraseCopyFlash;
+
+typedef struct
+{
+	R2C_Header Header;
+	unsigned long SizeCopied;	 
+} __attribute__((packed)) R2C_EraseCopyFlashResponse;
+
+
+typedef struct 
+{
+	R2C_Header Header;
+	unsigned long BlockNumber;
+	unsigned char Switch;	/* ON/OFF */
+} __attribute__((packed)) R2C_ProtectFlash;
+
+typedef struct
+{
+	R2C_Header Header;
+	unsigned char Status;
+} __attribute__((packed)) R2C_ProtectFlashResponse;
+	
+typedef struct 
+{
+	R2C_Header Header;
+	unsigned long Src;
+	unsigned long Dest;
+	unsigned long Size;
+} __attribute__((packed)) R2C_CopyMemory;
+
+typedef struct
+{
+	R2C_Header Header;
+	unsigned long SizeCopied;	 
+} __attribute__((packed)) R2C_CopyMemoryResponse;
+
+typedef struct 
+{
+	R2C_Header Header;
+	unsigned long Addr1;
+	unsigned long Addr2;
+	unsigned long Size;
+} __attribute__((packed)) R2C_CompareMemory;
+
+typedef struct
+{
+	R2C_Header Header;
+	unsigned long Offset;	 /* Gives Failed Offset or Size if success */
+} __attribute__((packed)) R2C_CompareMemoryResponse;
+
+typedef struct 
+{
+	R2C_Header Header;
+	unsigned long Addr;
+	unsigned long Size;
+} __attribute__((packed)) R2C_ClearMemory;
+
+typedef struct
+{
+	R2C_Header Header;
+	unsigned long SizeCleared;	 
+} __attribute__((packed)) R2C_ClearMemoryResponse;
+
+
+typedef struct 
+{
+	R2C_Header Header;
+	unsigned long Size;
+} __attribute__((packed)) R2C_AllocateMemory;
+
+typedef struct
+{
+	R2C_Header Header;
+	unsigned long Addr; 
+} __attribute__((packed)) R2C_AllocateMemoryResponse;
+
+
+typedef struct 
+{
+	R2C_Header Header;
+	unsigned long Addr;
+} __attribute__((packed)) R2C_FreeMemory;
+
+typedef struct
+{
+	R2C_Header Header;
+	unsigned char Status;  /* 1 = Success and 0 = Failed */ 
+} __attribute__((packed)) R2C_FreeMemoryResponse;
+
+typedef struct
+{
+	R2C_Header Header;
+	unsigned char MACAddr[6];
+}  __attribute__((packed)) R2C_SetMacAddress;
+	
+typedef struct
+{
+	R2C_Header Header;
+	unsigned char Status;
+}  __attribute__((packed)) R2C_SetMacAddressResponse;
+
+typedef struct
+{
+	R2C_Header Header;
+	unsigned long ExecAddr;	
+	unsigned char ImageType;
+}  __attribute__((packed)) R2C_ExecuteProgram;
+
+#define IMAGE_BINARY		0x00
+#define IMAGE_ELF			0x01
+#define IMAGE_COFF			0x02
+
+typedef struct
+{
+	R2C_Header Header;
+	unsigned char Status;
+}  __attribute__((packed)) R2C_ExecuteProgramResponse;
+
+typedef struct
+{
+	R2C_Header Header;
+}  __attribute__((packed)) R2C_StopSession;
+
+typedef struct
+{
+	R2C_Header Header;
+	unsigned char 	Status;
+}  __attribute__((packed)) R2C_StopSessionResponse;
+
+
+typedef struct
+{
+	R2C_Header Header;
+	unsigned short WaitTime;
+}  __attribute__((packed)) R2C_ResetDevice;
+
+
+typedef struct
+{
+	R2C_Header Header;
+	unsigned char 	Status;
+}  __attribute__((packed)) R2C_ResetDeviceResponse;
+
+typedef struct 
+{
+	R2C_Header Header;
+	unsigned char Switch;		/* 0 = OFF and 1 = ON */
+}	__attribute__((packed)) R2C_RemoteTerminal;
+
+typedef struct
+{
+	R2C_Header Header;
+	unsigned char Status;
+} __attribute__((packed)) R2C_RemoteTerminalResponse;
+		
+
+#define KEY_SIZE	64
+#define VALUE_SIZE	64
+typedef struct 
+{
+	R2C_Header Header;
+	unsigned char Key[KEY_SIZE+1];
+} __attribute__((packed)) R2C_GetVar;
+
+typedef struct
+{
+	R2C_Header Header;
+	unsigned char Status;  /* 1 = Success and 0 = Failed */ 
+	unsigned char Value[VALUE_SIZE+1];
+} __attribute__((packed)) R2C_GetVarResponse;
+
+typedef struct 
+{
+	R2C_Header Header;
+	unsigned char Key[KEY_SIZE+1];
+	unsigned char Value[VALUE_SIZE+1];
+} __attribute__((packed)) R2C_SetVar;
+
+typedef struct
+{
+	R2C_Header Header;
+	unsigned char Status;  /* 1 = Success and 0 = Failed */ 
+} __attribute__((packed)) R2C_SetVarResponse;
+
+/* Header CRC32 Calculation */
+unsigned long Calculate_R2C_Crc32(unsigned char *pkt,int len);
+
+#include <asm/byteorder.h>
+#define host_to_le16(x)	__cpu_to_le16((x))
+#define host_to_le32(x)	__cpu_to_le32((x))
+#define le16_to_host(x)	__le16_to_cpu((x))
+#define le32_to_host(x)	__le32_to_cpu((x))
+
+/*
+#define host_to_le16(x) (x)	
+#define host_to_le32(x)	(x)
+#define le16_to_host(x)	(x)
+#define le32_to_host(x)	(x)
+*/
+
+
+
+#endif
