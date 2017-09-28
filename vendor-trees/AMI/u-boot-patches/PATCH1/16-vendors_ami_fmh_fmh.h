--- uboot/vendors/ami/fmh/fmh.h	1969-12-31 19:00:00.000000000 -0500
+++ uboot.new/vendors/ami/fmh/fmh.h	2007-04-30 12:48:21.000000000 -0400
@@ -0,0 +1,130 @@
+#ifndef __AMI_FMH_H__
+#define __AMI_FMH_H__
+
+#define FMH_SIGNATURE		"$MODULE$"
+#define FMH_END_SIGNATURE 	0x55AA
+#define FMH_SIZE			64
+#define FMH_MAJOR			1
+#define FMH_MINOR		 	5	
+
+/*
+typedef struct
+{
+	unsigned short BootPathID:3;
+	unsigned short CopyToRAM:1;
+	unsigned short Execute:1;	
+	unsigned short Compress:3;
+	unsigned short ValidChecksum:1;
+	unsigned short Reserved:7;
+} __attribute__ ((packed)) MODULE_FLAGS;
+*/
+
+/* Module Information Structure */
+typedef struct
+{
+	unsigned char 	Module_Name[8];
+	unsigned char  	Module_Ver_Major;
+	unsigned char  	Module_Ver_Minor;
+	unsigned short	Module_Type;			/* Refer Module Type Values Below */
+	unsigned long	Module_Location;		/* Offset to Module from start */
+	unsigned long	Module_Size;
+	unsigned short	Module_Flags;			/* Refer Module Flags Values Below */
+	unsigned long	Module_Load_Address;
+	unsigned long 	Module_Checksum;		/* CRC 32 */
+	unsigned char	Module_Reserved[8];
+} __attribute__ ((packed)) MODULE_INFO;
+
+/* Flash Module Header */
+typedef struct
+{
+	unsigned char 	FMH_Signature[8];		/* "$MODULE$" */
+	unsigned char 	FMH_Ver_Major;			/* 1 */	
+	unsigned char 	FMH_Ver_Minor;			/* 5 */
+	unsigned short	FMH_Size;				/* For FHM Ver 1.5 this is 64 */
+	unsigned long 	FMH_AllocatedSize;
+	unsigned long	FMH_Location;
+	unsigned char	FMH_Reserved[3];
+	unsigned char	FMH_Header_Checksum;	/* Modulo 100 */
+	MODULE_INFO		Module_Info;
+	unsigned short 	FMH_End_Signature;		/* 0x55AA */	
+} __attribute__ ((packed)) FMH;
+
+/* Alternate FMH Location - At the end of first erase block */
+typedef struct
+{
+	unsigned short FMH_End_Signature;		/* 0x55AA */
+	unsigned char  FMH_Header_Checksum;		/* Modulo 100 */
+	unsigned char  FMH_Reserved;
+	unsigned long  FMH_Link_Address;		/* Actual FMH offset from erase blk start*/
+	unsigned char  FMH_Signature[8];		/* "$MODULE$" */
+} __attribute__ ((packed)) ALT_FMH;
+
+
+/*Values for FMH_Link_Address*/
+#define INVALID_FMH_OFFSET 0xFFFFFFFF
+
+
+/* Values for LSB of Module Type */
+#define MODULE_UNKNOWN		0x00	/* Unknown Module Type  */	/* All */
+#define MODULE_BOOTLOADER	0x01	/* Boot Loader 		*/	/* All */	
+#define MODULE_FMH_FIRMWARE		0x02  	/* Info about  firmware */	/* All */	
+#define MODULE_KERNEL		0x03	/* OS Kernel 		*/	/* All */	
+#define MODULE_FPGA		0x04	/* FPGA Microcode 	*/	/* All */		
+#define MODULE_ELF		0x05	/* ELF Executable Image */	/* All */
+#define MODULE_PIMAGE		0x06	/* U-Boot Linux Image 	*/	/* < 1.4  */
+#define MODULE_INITRD_CRAMFS    0x07  	/* Initrd image of cramfs*/	/* < 1.4  */
+#define MODULE_LINUX_KERNEL	0x08	/* Linux Kernel Image 	 */     /* >= 1.4 */
+#define MODULE_LINUX_ROOTFS	0x09	/* Linux Root FileSystem */  	/* >= 1.4 */
+#define MODULE_JFFS		0x10	/* JFFS File System 	 */	/* < 1.3  */
+#define MODULE_JFFS2		0x11	/* JFFS2 File System 	 */	/* < 1.3  */
+#define MODULE_JFFS_CONFIG	0x20					/* 1.3 Only */
+#define MODULE_JFFS2_CONFIG	0x21					/* 1.3 Only */
+#define MODULE_CONFIG		0x30	/* Configuration */ 		/* >= 1.4 */
+#define MODULE_WEB		0x40	/* Web pages 	 */		/* >= 1.4 */
+#define MODULE_PDK		0x50	/* PDK 	 */			/* >= 1.4 */
+
+/* Values for MSBof Module Type = Module Format */
+#define MODULE_FORMAT_BACKWARD		0x00	/* Set for Backward comaptible till 1.3 */
+#define MODULE_FORMAT_BINARY		0x01
+#define MODULE_FORMAT_FIRMWARE_INFO	0x02
+#define MODULE_FORMAT_UBOOT_WRAP	0x03
+#define MODULE_FORMAT_ELF		0x04
+#define MODULE_FORMAT_CRAMFS		0x10
+#define MODULE_FORMAT_JFFS		0x11
+#define MODULE_FORMAT_JFFS2		0x12
+#define MODULE_FORMAT_INITRD		0x80
+
+#define MODULE_FIRMWARE_1_4	((MODULE_FORMAT_FIRMWARE_INFO << 8) | (MODULE_FMH_FIRMWARE))
+
+/* Values for Module Flags */
+#define MODULE_FLAG_BOOTPATH_OS			0x0001
+#define MODULE_FLAG_BOOTPATH_DIAG		0x0002
+#define MODULE_FLAG_BOOTPATH_RECOVERY	0x0004
+#define MODULE_FLAG_COPY_TO_RAM			0x0008
+#define MODULE_FLAG_EXECUTE				0x0010
+#define MODULE_FLAG_COMPRESSION_MASK	0x00E0  /* Refer Compression Type values */
+#define MODULE_FLAG_COMPRESSION_LSHIFT	5
+#define MODULE_FLAG_VALID_CHECKSUM		0x0100	/* Validate Checksum if this is set */
+
+/* Values for Compression */
+#define MODULE_COMPRESSION_NONE			0x00
+#define MODULE_COMPRESSION_MINILZO_1X	0x01
+#define MODULE_COMPRESSION_GZIP			0x02
+
+#include <asm/byteorder.h>
+#define host_to_le16(x)	__cpu_to_le16((x))
+#define host_to_le32(x)	__cpu_to_le32((x))
+#define le16_to_host(x)	__le16_to_cpu((x))
+#define le32_to_host(x)	__le32_to_cpu((x))
+
+/* Function Prototypes */
+FMH* 	ScanforFMH(unsigned char *SectorAddr, unsigned long SectorSize);
+void	CreateFMH(FMH *fmh,unsigned long AllocatedSize, MODULE_INFO *mod);
+void 	CreateAlternateFMH(ALT_FMH *altfmh,unsigned long FMH_Offset); 
+
+/* CRC32 Related */
+unsigned long CalculateCRC32(unsigned char *Buffer, unsigned long Size);
+void BeginCRC32(unsigned long *crc32);
+void DoCRC32(unsigned long *crc32, unsigned char Data);
+void EndCRC32(unsigned long *crc32);
+#endif	
