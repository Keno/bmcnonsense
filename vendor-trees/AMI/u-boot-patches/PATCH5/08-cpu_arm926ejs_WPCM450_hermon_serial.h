--- u-boot/cpu/arm926ejs/WPCM450/hermon_serial.h	1969-12-31 17:00:00.000000000 -0700
+++ u-boot-new/cpu/arm926ejs/WPCM450/hermon_serial.h	2008-06-04 17:53:00.000000000 -0500
@@ -0,0 +1,48 @@
+#ifndef HERMON_SERIAL_H
+#define HERMON_SERIAL_H
+
+/*  -------------------------------------------------------------------------------
+ *   API
+ *  -------------------------------------------------------------------------------
+ */
+#include "macros.h"
+
+#define FALSE	0
+#define TRUE	1
+
+/* SPSWC serial port mode selection */
+#define SPSWC_SPMOD_1				0x0   // Core snoop (default)     (HSP2 <-> SI2, UART1<-> SI2)
+#define SPSWC_SPMOD_2				0x1   // Core Take Over		     (UART1 <-> SI2)
+#define SPSWC_SPMOD_3				0x2   // Core Direct to Host SP2  (UART1 <-> HSP2)
+#define SPSWC_SPMOD_RESERVED			0x3   // Reserved
+
+extern void WPCM_SerialInit(UINT32 port, UINT32 baudrate, UINT32 parity,UINT32 num,UINT32 len);
+extern void WPCM_SetSerialFifoCtrl(UINT32 port, UINT32 level, UINT32 resettx, UINT32 resetrx);
+extern void WPCM_DisableSerialFifo(UINT32 port);
+extern void WPCM_SetSerialInt(UINT32 port, UINT32 IntMask);
+
+extern char WPCM_GetSerialChar(UINT32 port);
+extern void WPCM_PutSerialChar(UINT32 port, char Ch);
+extern void WPCM_PutSerialStr(UINT32 port, char *Str);
+
+extern void WPCM_EnableSerialInt(UINT32 port, UINT32 mode);
+extern void WPCM_DisableSerialInt(UINT32 port, UINT32 mode);
+
+extern void WPCM_SerialRequestToSend(UINT32 port);
+extern void WPCM_SerialStopToSend(UINT32 port);
+extern void WPCM_SerialDataTerminalReady(UINT32 port);
+extern void WPCM_SerialDataTerminalNotReady(UINT32 port);
+
+extern void WPCM_SetSerialLineBreak(UINT32 port);
+extern void WPCM_SetSerialLoopBack(UINT32 port,UINT32 onoff);
+extern UINT32 WPCM_SerialIntIdentification(UINT32 port);
+
+extern UINT32 WPCM_ReadSerialLineStatus(UINT32 port);
+extern UINT32 WPCM_ReadSerialModemStatus(UINT32 port);
+
+extern void WPCM_SetSerialMode(UINT32 port, UINT32 mode);
+extern void WPCM_EnableIRMode(UINT32 port, UINT32 TxEnable, UINT32 RxEnable);
+extern int  WPCM_TestSerialForChar(UINT32 port);
+
+
+#endif
