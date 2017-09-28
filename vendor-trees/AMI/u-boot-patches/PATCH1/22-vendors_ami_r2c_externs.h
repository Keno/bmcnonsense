--- uboot/vendors/ami/r2c/externs.h	1969-12-31 19:00:00.000000000 -0500
+++ uboot.console/vendors/ami/r2c/externs.h	2006-10-04 11:41:08.120459730 -0400
@@ -0,0 +1,52 @@
+#ifndef AMI_EXTERNS_H
+#define AMI_EXTERNS_H
+
+/*From uboot common*/
+extern int do_reset (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]);
+extern int do_bootelf (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[] );
+extern int valid_elf_image (unsigned long addr);
+
+extern void RestoreTerminal(void);
+extern void AddtoInputQueue(uchar *pkt,int len);
+
+extern int ProcessSetMacAddress(uchar *pkt,int len);
+extern int ProcessRemoteTerminal(uchar *pkt,int len);
+extern int ProcessStopSession(uchar *pkt,int len);
+extern int ProcessExecuteProgram(uchar *pkt,int len);
+extern int ProcessResetDevice(uchar *pkt,int len);
+extern int ProcessSetVar(uchar *pkt,int len);
+extern int ProcessGetVar(uchar *pkt,int len);
+
+extern int ProcessReadMemory(uchar *pkt,int len); 	/* Also ReadFlash 	*/
+extern int ProcessWriteMemory(uchar *pkt,int len); 	/* Also WriteFlash 	*/
+extern int ProcessCopyMemory(uchar *pkt,int len);	/* Also CopyFlash 	*/
+extern int ProcessCompareMemory(uchar *pkt,int len);/* Also CompFlash	*/
+extern int ProcessClearMemory(uchar *pkt,int len);  /* Also ClearFlash	*/
+
+extern int ProcessAllocateMemory(uchar *pkt,int len);
+extern int ProcessFreeMemory(uchar *pkt,int len);
+		
+int ProcessGetFMHInfo(uchar *pkt,int len);
+int ProcessGetFlashInfo(uchar *pkt,int len);
+int ProcessEraseFlash(uchar *pkt,int len);
+int ProcessProtectFlash(uchar *pkt,int len);
+int ProcessEraseCopyFlash(uchar *pkt,int len);
+
+extern void EnterR2CSession(void);
+extern int ProcessSessionCommands(unsigned char *pkt,int len);
+
+extern int ProcessSetVarNoSave(uchar *pkt,int len);
+
+int IsFlash(unsigned long Dest);
+int Write2MemByte(unsigned char *Dest, unsigned char Data);
+int Write2MemWord(unsigned short *Dest, unsigned short Data);
+int Write2MemDword(unsigned long *Dest, unsigned long Data);
+
+extern IPaddr_t ConsoleIP;
+extern uchar ConsoleEther[6];
+extern uchar RemoteTerminal;
+extern uchar AllocMemory;
+extern unsigned short ConsoleUDPPort;
+extern uchar CallFromNetLoop;
+
+#endif
