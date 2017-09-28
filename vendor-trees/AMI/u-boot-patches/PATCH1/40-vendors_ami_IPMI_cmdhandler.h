--- uboot/vendors/ami/IPMI/cmdhandler.h	1969-12-31 19:00:00.000000000 -0500
+++ uboot.new/vendors/ami/IPMI/cmdhandler.h	2007-11-20 16:30:24.000000000 -0500
@@ -0,0 +1,930 @@
+/*****************************************************************
+ *****************************************************************
+ ***                                                            **
+ ***    (C)Copyright 2002-2003, American Megatrends Inc.        **
+ ***                                                            **
+ ***            All Rights Reserved.                            **
+ ***                                                            **
+ ***        6145-F, Northbelt Parkway, Norcross,                **
+ ***                                                            **
+ ***        Georgia - 30071, USA. Phone-(770)-246-8600.         **
+ ***                                                            **
+ *****************************************************************
+ *****************************************************************
+ *****************************************************************
+ * $Header: $
+ *
+ * $Revision: $
+ *
+ * $Date: $
+ *
+ ******************************************************************
+ ******************************************************************
+ * 
+ * cmdhandler.h
+ * command handler data structures & macros
+ *
+ *  Author: Rama Rao Bisa <ramab@ami.com>
+ *			Modified by Anurag Bhatia for SP porting
+ ******************************************************************/
+
+#ifndef _CMD_HANDLER_H_
+#define _CMD_HANDLER_H_
+
+#define PACKED __attribute__ ((packed))
+/*** Macro Definitions ***/
+
+#define IP_ADDR_LEN			4
+#define MAX_FILE_NAME_LEN		100
+/**
+ * @def MAX_IPMI_MSG_LEN
+ * @brief Maximum IPMI Message Length
+ **/
+#define MAX_IPMI_MSG_LEN		(64 * 1024)
+
+#define DATA_PACKET_SIZE        (32*1024)       
+
+#define TOTAL_PACKETS_XMIT_IN_A_BLOCK   (CFG_PROJ_ERASE_BLOCK_SIZE/DATA_PACKET_SIZE)
+
+#define MAX_FIRMWARE_SECTORS		40
+
+/*----------------------------------------------
+ * Processor specfic type definetion
+ *----------------------------------------------*/
+
+ typedef char  INT8S;
+ typedef short INT16S;
+ typedef long  INT32S;
+ typedef unsigned char  INT8U;
+ typedef unsigned short INT16U;
+ typedef unsigned long INT32U;
+ typedef unsigned long DWORD;
+/*  Command Codes */
+/**
+ * @def CMD_GET_DEV_ID
+ * @brief Command code for Get Device ID Command
+ **/
+#define CMD_GET_DEV_ID				0x01
+#define CMD_GET_CH_AUTH_CAPS			0x38
+#define CMD_GET_SESSION_CHALLENGE		0x39
+#define CMD_ACTIVATE_SESSION			0x3A
+#define CMD_SET_SESSION_PRIV_LEVEL		0x3B
+#define CMD_CLOSE_SESSION			0x3C
+
+#define CMD_GET_IMC_STATUS			0x02
+#define CMD_SET_TFTP_INFO			0x03
+#define CMD_AMI_UPGRADE_BLOCK			0x11
+#define CMD_AMI_INIT_FLASH			0x12
+#define CMD_AMI_EXIT_FLASH			0x13
+#define CMD_AMI_RESET_CARD			0x14
+#define CMD_AMI_START_FW_UPDATE			0x15
+#define CMD_AMI_UPDATE_UBOOT			0x16
+#define CMD_AMI_GET_STATUS			0x17
+#define CMD_AMI_ENTER_FW_UPDATE_MODE		0x18
+
+
+#define CMD_AMI_YAFU_COMMON_NAK (0X00FF)
+#define CMD_AMI_YAFU_GET_FLASH_INFO  (0x0001)
+#define CMD_AMI_YAFU_GET_FMH_INFO (0x0003)
+#define CMD_AMI_YAFU_GET_STATUS (0x0004)
+#define CMD_AMI_YAFU_ACTIVATE_FLASH (0x0010)
+#define CMD_AMI_YAFU_PROTECT_FLASH (0x0025)
+#define CMD_AMI_YAFU_ERASE_COPY_FLASH (0x0026)
+#define CMD_AMI_YAFU_WRITE_MEMORY (0x0031)
+#define CMD_AMI_YAFU_GET_BOOT_CONFIG (0x0040)
+#define CMD_AMI_YAFU_SET_BOOT_CONFIG (0x0041)
+#define CMD_AMI_YAFU_GET_BOOT_VARS (0x0042)
+#define CMD_AMI_YAFU_DEACTIVATE_FLASH_MODE (0x0050)
+#define CMD_AMI_YAFU_RESET_DEVICE (0x0051)
+
+
+
+/* Command Handlers */
+/**
+ * @def CMD_GET_DEV_ID
+ * @brief Get Device ID Command Handler
+ **/
+#define GET_DEV_ID						GetDevID
+#define GET_IMC_STATUS					GetIMCStatus
+#define SET_TFTP_INFO					SetTFTPInfo
+#define GET_CH_AUTH_CAPS				GetChAuthCaps
+#define	GET_SESSION_CHALLENGE			GetSessionChallenge
+#define ACTIVATE_SESSION				ActivateSession
+#define SET_SESSION_PRIV_LEVEL			SetSessionPrivLevel
+#define CLOSE_SESSION					CloseSession
+#define AMI_INIT_FLASH					AmiInitFlash
+#define AMI_UPGRADE_BLOCK				AMIUpgradeBlock
+#define AMI_EXIT_FLASH					AMIExitUploadImage
+#define AMI_RESET_CARD					AMIResetCard
+#define AMI_GET_STATUS					AMIGetStatus
+#define AMI_UPDATE_UBOOT				AMIUpdateUboot
+#define AMI_START_FW_UPDATE				AMIStartFirmwareUpdate
+#define AMI_ENTER_FW_UPDATE_MODE		AMIEnterFirmwareUpdateMode
+
+#define AMI_YAFU_GET_FLASH_INFO        		AMIYAFUGetFlashInfo   
+#define AMI_YAFU_GET_FMH_INFO            		AMIYAFUGetFMHInfo
+#define AMI_YAFU_GET_STATUS                		AMIYAFUGetStatus
+#define AMI_YAFU_ACTIVATE_FLASH         		AMIYAFUActivateFlashMode
+#define AMI_YAFU_PROTECT_FLASH           		AMIYAFUProtectFlash
+#define AMI_YAFU_ERASE_COPY_FLASH     		AMIYAFUEraseCopyFlash
+#define AMI_YAFU_WRITE_MEMORY            		AMIYAFUWriteMemory
+#define AMI_YAFU_GET_BOOT_CONFIG      		AMIYAFUGetBootConfig        
+#define AMI_YAFU_SET_BOOT_CONFIG      		AMIYAFUSetBootConfig        
+#define AMI_YAFU_GET_BOOT_VARS          		AMIYAFUGetBootVars          
+#define AMI_YAFU_DEACTIVATE_FLASH_MODE   AMIYAFUDeactivateFlash
+#define AMI_YAFU_RESET_DEVICE   			AMIYAFUResetDevice
+
+
+#define IMAGE_UPLOAD_LOCATION					(0x8A00000)
+#define TEMP_SECTOR_BUFFER						(0x9A00000)
+#define FMH_MODULE_CHECKSUM_START_OFFSET		(0x32 + CFG_PROJ_USED_FLASH_SIZE - CFG_PROJ_ERASE_BLOCK_SIZE)
+#define FMH_MODULE_CHCKSUM_END_OFFSET			(0x35 + CFG_PROJ_USED_FLASH_SIZE - CFG_PROJ_ERASE_BLOCK_SIZE)
+#define FMH_FMH_HEADER_CHECKSUM_OFFSET			(0x17 + CFG_PROJ_USED_FLASH_SIZE - CFG_PROJ_ERASE_BLOCK_SIZE)
+#define CONFIG_SECTION_NAME						"params"
+
+
+#pragma pack(1)
+
+typedef struct
+{
+	unsigned char NetFnLUN;
+	unsigned char Cmd;
+	unsigned long ReqLen;
+	unsigned long ResLen;
+	unsigned char ReqData [MAX_IPMI_MSG_LEN];
+	unsigned char ResData [MAX_IPMI_MSG_LEN];
+
+} PACKED IPMICmdData_T;
+
+
+typedef struct 
+{
+    unsigned char   CompletionCode;
+    unsigned char   DeviceID;
+    unsigned char   DevRevision;
+    unsigned char   FirmwareRevision1;
+    unsigned char   FirmwareRevision2;
+    unsigned char   IPMIVersion;
+    unsigned char   DevSupport;
+    unsigned char   MfgID[3];
+    unsigned short  ProdID;
+    unsigned long  AuxFirmwareRevision;
+} PACKED GetDevIDRes_T;
+
+typedef struct
+{
+	unsigned int	ServerIPAddr;
+	unsigned char	ImageFileName [MAX_FILE_NAME_LEN];	
+
+} PACKED SetTFTPInfo_T;
+
+typedef struct
+{
+	unsigned char	CompletionCode;
+	unsigned char	StatusByte1;
+	unsigned char	StatusByte2;
+	unsigned char	StatusByte3;
+	unsigned char	StatusByte4;
+
+} PACKED GetIMCStatus_T;
+
+/* GetChAuthCapReq_T */
+typedef struct 
+{
+    unsigned char   ChannelNum;
+    unsigned char   PrivLevel;
+
+} PACKED GetChAuthCapReq_T;
+
+/* GetChAuthCapRes_T */
+typedef struct 
+{
+    uchar   CompletionCode;
+    uchar   ChannelNum;
+    uchar   AuthType;
+    uchar   PerMsgUserAuthLoginStatus;
+    uchar   Reserved;
+    uchar   OEMID[3];
+    uchar   OEMAuxData;
+
+} PACKED GetChAuthCapRes_T;
+
+
+/* GetSesChallengeReq_T */
+typedef struct 
+{
+    uchar   AuthType;
+    uchar   UserName[16];
+
+} PACKED GetSesChallengeReq_T;
+
+/* GetSesChallengeRes_T */
+typedef struct 
+{
+    uchar   CompletionCode;
+    unsigned int  TempSessionID;
+    uchar   ChallengeString[16];
+
+} PACKED GetSesChallengeRes_T;
+
+/* ActivateSesReq_T */
+typedef struct 
+{
+    uchar   AuthType;
+    uchar   Privilege;
+    uchar   ChallengeString[16];
+    unsigned int  OutboundSeq;
+
+} PACKED ActivateSesReq_T;
+
+/* ActivateSesRes_T */
+typedef struct 
+{
+    uchar   CompletionCode;
+    uchar   AuthType;
+    unsigned int  SessionID;
+    unsigned int  InboundSeq;
+    uchar   Privilege;
+
+} PACKED ActivateSesRes_T;
+
+/* SetSesPrivLevelRes_T */
+typedef struct 
+{
+    uchar   CompletionCode;
+    uchar   Privilege;
+
+} PACKED SetSesPrivLevelRes_T;
+
+/* GetSesInfoReq_T */
+typedef struct 
+{
+    uchar   SessionIndex;
+    uchar   SessionHandleOrID [4];
+
+} GetSesInfoReq_T;
+
+/* LANSesInfoRes_T */
+typedef struct 
+{
+    uchar    IPAddress[4];
+    uchar    MACAddress[6];
+    ushort   PortNumber;
+
+} PACKED LANSesInfoRes_T;
+
+/* SerialSesInfoRes_T */
+typedef struct 
+{
+    uchar    SessionActivityType;
+    uchar    DestinationSelector;
+    uchar    IPAddress[4];
+    ushort   PortNumber;
+
+} PACKED SerialSesInfoRes_T;
+
+/* Union SessionInfoRes_T */
+typedef union 
+{
+    LANSesInfoRes_T     LANSesInfo;
+    SerialSesInfoRes_T  SerialSesInfo;
+
+} PACKED SessionInfoRes_T;
+
+/* ActiveSesInfo_T */
+typedef struct 
+{
+    uchar       UserID;
+    uchar       Privilege;
+    uchar       ChannelNum;
+
+} PACKED ActiveSesInfo_T;
+
+/* GetSesInfoRes_T */
+typedef struct 
+{
+    uchar               CompletionCode;
+    uchar               SessionHandle;
+    uchar               NumPossibleActiveSession;
+    uchar               NumActiveSession;
+    ActiveSesInfo_T     ActiveSesinfo;
+    SessionInfoRes_T    SesInfo;
+
+} PACKED GetSesInfoRes_T;
+
+/* CloseSesReq_T */
+typedef struct 
+{
+    unsigned int  SessionID;
+
+} PACKED CloseSesReq_T;
+
+typedef struct
+{
+	ulong				FlashStartAddr;
+	ulong				FlashSize;
+	ulong				UsedFlashStartAddr;
+	ulong				UsedFlashSize;
+	ulong				EnvStartAddr;
+	ulong				EnvSize;
+	ulong				EraseBlockSize;
+	uchar				CurrentImageMajor;
+	uchar				CurrentImageMinor;
+	uchar				CurrentImageName [16];
+	uchar				RetainConfig;
+	uchar				UpdateUboot;
+
+} PACKED FLASH_PARAMS;
+
+typedef struct
+{
+	FLASH_PARAMS		Params;
+
+} PACKED AMIInitFlashReq_T;
+
+typedef struct
+{
+	uchar				CompletionCode;
+	FLASH_PARAMS		Params;
+
+} PACKED AMIInitFlashRes_T;
+
+typedef struct  BlockInfo
+{
+    unsigned long BlockStartAdd;
+    unsigned char BlockData[DATA_PACKET_SIZE];
+    unsigned long CRC32;
+    unsigned char EndOfData;
+} BI_t;
+
+/**
+ * @struct AMIUpgradeBlockReq_T
+ * @brief AMI Upgrade Block command request structure
+ **/
+typedef struct
+{
+    /* Block information */
+    BI_t   blkInfo;
+} PACKED AMIUpgradeBlockReq_T;
+
+/**
+ * @struct AMIUpgradeBlockRes_T
+ * @brief AMI Upgrade Block command response structure
+ **/
+typedef struct
+{
+    unsigned char CompletionCode;
+
+} PACKED AMIUpgradeBlockRes_T;
+
+/* Exit Flash */
+typedef struct
+{
+    FLASH_PARAMS Params;
+
+} PACKED AMIExitFlashReq_T;
+
+typedef struct
+{
+    unsigned char CompletionCode;
+    //FLASH_PARAMS Params;
+
+} PACKED AMIExitFlashRes_T;
+
+typedef struct
+{
+    unsigned char CompletionCode;
+
+} PACKED AMIResetCardRes_T;
+
+typedef struct
+{
+    FLASH_PARAMS Params;
+
+} PACKED AMIStartFirmwareUpdateReq_T;
+
+typedef struct
+{
+    unsigned char CompletionCode;
+
+} PACKED AMIStartFirmwareUpdateRes_T;
+
+typedef struct
+{
+    unsigned char CompletionCode;
+
+} PACKED AMIEnterFirmwareUpdateModeRes_T;
+
+
+typedef struct _StructuredFlashProgress
+{
+    char SubAction[60]; //currently doing what step of the action- erasing,writing etc
+    char Progress[30]; //progress indicator..can be strings like so much percent done etc.
+    int  State; //set by modules to indicate that everything is complete for this module
+} PACKED STRUCTURED_FLASH_PROGRESS;
+
+typedef struct
+{
+    unsigned char 				CompletionCode;
+	STRUCTURED_FLASH_PROGRESS	FlashProgess;	
+} PACKED AMIGetStatusRes_T;
+
+
+/**
+ * @struct YafuHeader
+ * @brief Flash info structure
+ */
+ typedef struct
+{
+    INT32U  Seqnum;
+    INT16U  YafuCmd;
+    INT16U  Datalen;
+    INT32U  CRC32chksum;
+} PACKED YafuHeader;
+
+/**
+ * @struct AMIYAFUNotAck
+ * @brief Flash info structure
+ */
+typedef struct
+{
+   YafuHeader NotAck;
+   INT16U       ErrorCode;
+}PACKED AMIYAFUNotAck;
+
+/**
+ * @struct ALT_FMH
+ * @brief Flash info structure
+ */
+
+typedef struct
+{
+    INT16U EndSignature;
+    INT8U   HeaderChkSum;
+    INT8U   Reserved;
+    INT32U LinkAddress;
+    INT8U   Signature[8]; 	
+} PACKED ALT_FMHead;
+
+
+
+/**
+ * @struct FMH
+ * @brief Flash info structure
+ */
+
+	typedef struct
+	{
+		INT8U		FmhSignature[8];
+		union
+		{
+			INT16U	FwVersion;
+			INT8U	FwMinorVer;
+			INT8U	FwMajorVer;
+		} PACKED Fmh_Version;
+		INT16U		FmhSize;
+		INT32U		AllocatedSize;
+		INT32U		FmhLocation;
+		INT8U		FmhReserved[3];
+		INT8U		HeaderChecksum;
+		INT8U		ModuleName[8];
+		union
+		{
+			INT16U	ModVersion;
+			INT8U	ModMinorVer;
+			INT8U	ModMajorVer;
+		}PACKED Module_Version;
+		INT16U		ModuleType;
+		INT32U		ModuleLocation;
+		INT32U		ModuleSize;
+		INT16U		ModuleFlags;
+		INT32U		ModuleLoadAddress;
+		INT32U		ModuleChecksum;
+		INT8U		ModuleReserved[8];
+		INT16U		EndSignature;
+	}PACKED  FlashMH;
+
+/**
+ * @struct AMIYAFUGetFlashInfoReq_T
+ * @brief Flash info structure
+ */
+ 
+typedef struct
+{
+    YafuHeader FlashInfoReq;
+
+} PACKED AMIYAFUGetFlashInfoReq_T;
+
+
+typedef struct
+{
+
+   INT32U   FlashSize;
+    INT32U  FlashAddress;
+    INT32U  FlashEraseBlkSize;
+    INT16U  FlashProductID;
+    INT8U    FlashWidth;	
+    INT8U    FMHCompliance;
+    INT16U  Reserved;
+    INT16U  NoEraseBlks;	 
+	
+}PACKED FlashDetails;
+
+
+/**
+ * @struct AMIYAFUGetFlashInfoRes_T
+ * @brief Flash info structure
+ */
+typedef struct
+{
+    INT8U           CompletionCode;
+    YafuHeader   FlashInfoRes;
+    FlashDetails  FlashInfo;
+
+} PACKED AMIYAFUGetFlashInfoRes_T;	
+
+
+
+
+/**
+ * @struct AMIYAFUGetFMHInfoRes_T
+ * @brief Flash info structure
+ */
+typedef struct
+{
+     YafuHeader FMHReq;
+
+} PACKED AMIYAFUGetFMHInfoReq_T;
+
+/**
+ * @struct AMIYAFUGetFMHInfoRes_T
+ * @brief Flash info structure
+ */
+typedef struct
+{
+    INT8U         CompletionCode;
+    YafuHeader FMHRes;
+    INT16U       Reserved;
+    INT16U       NumFMH;
+   
+} PACKED AMIYAFUGetFMHInfoRes_T;
+
+/**
+ * @struct AMIYAFUGetStatusReq_T
+ * @brief Flash info structure
+ */
+typedef struct
+{
+     YafuHeader GetStatusReq;
+
+} PACKED AMIYAFUGetStatusReq_T;
+
+
+/**
+ * @struct AMIYAFUGetStatusRes_T
+ * @brief Flash info structure
+ */
+typedef struct
+{
+    INT8U         CompletionCode;
+    YafuHeader GetStatusRes;
+    INT16U       LastStatusCode;
+    INT16U       YAFUState;
+    INT16U         Mode;
+    INT16U       Reserved;
+    INT8U         Message[65];
+
+} PACKED AMIYAFUGetStatusRes_T;
+
+/**
+ * @struct AMIYAFUActivateFlashModeReq_T
+ * @brief Flash info structure
+ */
+typedef struct
+{
+    YafuHeader ActivateflashReq;
+    INT16U       Mode;
+
+} PACKED AMIYAFUActivateFlashModeReq_T;
+
+/**
+ * @struct AMIYAFUGetStatusRes_T
+ * @brief Flash info structure
+ */
+typedef struct
+{
+    INT8U         CompletionCode;
+    YafuHeader ActivateflashRes;
+    INT8U         Delay;
+
+}PACKED AMIYAFUActivateFlashModeRes_T;
+
+/**
+ * @struct AMIYAFUProtectFlashReq_T
+ * @brief Flash info structure
+ */
+typedef struct
+{
+    YafuHeader ProtectFlashReq;
+    INT32U Blknum;
+    INT8U   Protect;
+
+} PACKED AMIYAFUProtectFlashReq_T;
+
+/**
+ * @struct AMIYAFUProtectFlashRes_T
+ * @brief Flash info structure
+ */
+typedef struct
+{
+    INT8U   CompletionCode;
+    YafuHeader ProtectFlashRes;
+    INT8U   Status;
+
+}PACKED AMIYAFUProtectFlashRes_T;
+
+/**
+ * @struct AMIYAFUEraseCopyFlashReq_T
+ * @brief Flash info structure
+ */
+typedef struct
+{
+    YafuHeader EraseCpyFlashReq;
+    INT32U Memoffset;
+    INT32U Flashoffset;
+    INT32U Sizetocopy;
+
+} PACKED AMIYAFUEraseCopyFlashReq_T;
+
+/**
+ * @struct AMIYAFUEraseCopyFlashRes_T
+ * @brief Flash info structure
+ */
+typedef struct
+{
+    INT8U   CompletionCode;
+    YafuHeader EraseCpyFlashRes;
+    INT32U Sizecopied;
+
+}PACKED AMIYAFUEraseCopyFlashRes_T;
+
+
+/**
+ * @struct AMIYAFUWriteMemoryReq_T
+ * @brief Flash info structure
+ */
+typedef struct
+{
+    YafuHeader WriteMemReq;
+    INT32U Memoffset;
+    INT8U   WriteWidth;
+    //INT8U   Buffer[0x4000];
+
+} PACKED AMIYAFUWriteMemoryReq_T;
+
+/**
+ * @struct AMIYAFUWriteMemoryRes_T
+ * @brief Flash info structure
+ */
+typedef struct
+{
+    INT8U   CompletionCode;
+    YafuHeader WriteMemRes;
+    INT16U SizeWritten;
+
+}PACKED AMIYAFUWriteMemoryRes_T;
+
+/**
+ * @struct AMIYAFUGetBootConfigReq_T
+ * @brief Flash info structure
+ */
+typedef struct
+{
+    YafuHeader GetBootReq;
+    INT8U         VarName[65];  	
+             	
+} PACKED AMIYAFUGetBootConfigReq_T;
+
+/**
+ * @struct AMIYAFUGetBootConfigRes_T
+ * @brief Flash info structure
+ */
+typedef struct
+{
+    INT8U         CompletionCode; 
+    YafuHeader GetBootRes;
+    INT8U         Status;	
+   	
+}PACKED AMIYAFUGetBootConfigRes_T;
+
+/**
+ * @struct AMIYAFUSetBootConfigReq_T
+ * @brief Flash info structure
+ */
+typedef struct
+{
+    YafuHeader SetBootReq;
+    INT8U         VarName[65];	
+                 	  
+} PACKED AMIYAFUSetBootConfigReq_T;
+
+/**
+ * @struct AMIYAFUSetBootConfigRes_T
+ * @brief Flash info structure
+ */
+typedef struct
+{
+    INT8U         CompletionCode; 
+    YafuHeader SetBootRes;
+    INT8U         Status;	
+    	
+}PACKED AMIYAFUSetBootConfigRes_T;
+
+/**
+ * @struct AMIYAFUGetBootVarsReq_T
+ * @brief Flash info structure
+ */
+typedef struct
+{
+    YafuHeader GetBootReq;
+                  	  
+} PACKED AMIYAFUGetBootVarsReq_T;
+
+/**
+ * @struct AMIYAFUGetBootVarsRes_T
+ * @brief Flash info structure
+ */
+typedef struct
+{
+    INT8U         CompletionCode; 
+    YafuHeader GetBootRes;
+    INT8U         VarCount;
+        	
+}PACKED AMIYAFUGetBootVarsRes_T;
+
+
+/**
+ * @struct AMIYAFUDeactivateFlashReq_T
+ * @brief Flash info structure
+ */
+typedef struct
+{
+    YafuHeader DeactivateFlashReq;
+
+} PACKED AMIYAFUDeactivateFlashReq_T;
+
+/**
+ * @struct AMIYAFUDeactivateFlashRes_T
+ * @brief Flash info structure
+ */
+typedef struct
+{
+    INT8U         CompletionCode;
+    YafuHeader DeactivateFlashRes;
+    INT8U         Status;
+	
+}PACKED AMIYAFUDeactivateFlashRes_T;
+
+/**
+ * @struct AMIYAFUResetDeviceReq_T
+ * @brief Flash info structure
+ */
+typedef struct
+{
+    YafuHeader ResetReq;
+    INT16U       WaitSec;
+
+} PACKED AMIYAFUResetDeviceReq_T;
+
+/**
+ * @struct AMIYAFUResetDeviceRes_T
+ * @brief Flash info structure
+ */
+typedef struct
+{
+    INT8U         CompletionCode;
+    YafuHeader ResetRes;
+    INT8U         Status;
+
+
+}PACKED AMIYAFUResetDeviceRes_T;
+#pragma pack()
+
+#define NO_ADDRESS						0xFFFFFFFF
+
+#define YAFU_CC_NORMAL						0x00
+#define YAFU_CC_INVALID_DATLEN			0xcc
+#define YAFU_CC_ALLOC_ERR				       0x02
+#define YAFU_CC_DEV_OPEN_ERR				0x03
+#define YAFU_CC_SEEK_ERR				       0x04
+#define YAFU_CC_READ_ERR				       0x05
+#define YAFU_CC_WRITE_ERR			              0x06
+#define YAFU_CC_MEM_ERASE_ERR				0x07
+#define YAFU_CC_IN_DEACTIVATE                       0x08
+#define YAFU_OFFSET_NOT_IN_ERASE_BOUNDARY     0x09
+#define YAFU_SIZE_NOT_IN_ERASE_BOUNDARY  0x10
+#define YAFU_FLASH_ERASE_FAILURE                 0x11
+#define YAFU_INVALID_CHKSUM                           0x12
+#define YAFU_PROTECT_ERR                                 0x14
+/**
+ * @def DEVICE_ID
+ * @brief Device ID
+ **/
+#define DEVICE_ID                       0x20
+/**
+ * @def SDR_SUPPORT
+ * @brief SDR support bit flag 
+ **/
+#define SDR_SUPPORT                     0x80    
+/**
+ * @def DEV_REVISION                    
+ * @brief Device Revision 
+ **/
+#define DEV_REVISION                    0x01    
+#define IPMI_DEV_REVISION               DEV_REVISION
+/**
+ * @def DEV_AVAILABLE                   
+ * @brief Device availability bit flag
+ **/
+#define DEV_AVAILABLE                   0x80
+/**
+ * @def IPMI_VERSION
+ * @brief IPMI version
+ **/
+#define IPMI_VERSION                    0x02
+/**
+ * @def CHASSIS_DEVICE_SUPPORT          
+ * @brief Chassis Device support bit flag
+ **/
+#define CHASSIS_DEVICE_SUPPORT          0x80    
+/**
+ * @def BRIDGE_COMMAND_SUPPORT          
+ * @brief Bridge Device support bit flag
+ **/
+#define BRIDGE_COMMAND_SUPPORT          0x40    
+/**
+ * @def IPMB_EVENT_GENERATOR_SUPPORT    
+ * @brief IPMB Event generator support bit flag 
+ **/
+#define IPMB_EVENT_GENERATOR_SUPPORT    0x20    
+/**
+ * @def IPMB_EVENT_RECEIVER_SUPPORT     
+ * @brief IPMB Event receiver support bit flag 
+ **/
+#define IPMB_EVENT_RECEIVER_SUPPORT     0x10    
+/**
+ * @def FRU_INVENTORY_DEVICE_SUPPORT    
+ * @brief FRU Device support bit flag 
+ **/
+#define FRU_INVENTORY_DEVICE_SUPPORT    0x08    
+/**
+ * @def SEL_DEVICE_SUPPORT              
+ * @brief SEL Device support bit flag 
+ **/
+#define SEL_DEVICE_SUPPORT              0x04    
+/**
+ * @def SDR_REPOSITORY_DEVICE_SUPPORT   
+ * @brief SDR Device support bit flag
+ **/
+#define SDR_REPOSITORY_DEVICE_SUPPORT   0x02    
+/**
+ * @def SENSOR_DEVICE_SUPPORT           
+ * @brief Sensor Device support bit flag 
+ **/
+#define SENSOR_DEVICE_SUPPORT           0x01    
+
+#define DEV_SUPPORT                     CHASSIS_DEVICE_SUPPORT |          \
+                                        IPMB_EVENT_GENERATOR_SUPPORT | IPMB_EVENT_RECEIVER_SUPPORT |\
+                                        FRU_INVENTORY_DEVICE_SUPPORT | SEL_DEVICE_SUPPORT |         \
+                                        SDR_REPOSITORY_DEVICE_SUPPORT | SENSOR_DEVICE_SUPPORT 
+/**
+ * @def MFG_ID_AMI                      
+ * @brief IANA number for AMI 
+ **/
+#define MFG_ID_AMI                      {0x00,0x00,0x00}    
+/**
+ * @def PROD_ID_AMI_PM                  
+ * @brief AMI Product ID {'E','M'} 
+ **/
+#define PROD_ID_AMI_PM                  0x0000
+
+#define 	CPLD_BASE_ADDR				0x90200000
+#define     CPLD_FW_UPDATE_REG_ADDR		0xB000007E
+#define 	CPLD_CONFIG_REG_INIT_VALUE	0x10000000
+#define 	CPLD_TIMING_REG_INIT_VALUE  0xFF1700DF
+
+
+extern unsigned int gSessionSeqNum;
+extern unsigned int gSessionID;
+extern int gStartFirmwareUpdate;
+extern int gReset;
+extern unsigned char *pNewFirmwareImage;
+extern char  *argv [];
+extern STRUCTURED_FLASH_PROGRESS	gAMIFlashStatus;
+extern unsigned int  gUpdateUboot;
+extern int gfirmwareUpgradeProcessStarted;
+extern int gStartFirmwareUpdation;
+extern INT32U gEraseBlock,gSizeToErase;
+extern void ProcessIPMICmd (IPMICmdData_T* pIPMICmd);
+
+#endif /* _CMD_HANDLER_H_ */
