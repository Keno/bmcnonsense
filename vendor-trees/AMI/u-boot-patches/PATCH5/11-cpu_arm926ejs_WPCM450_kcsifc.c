--- u-boot-1.1.4-cpu/cpu/arm926ejs/WPCM450/kcsifc.c	1969-12-31 19:00:00.000000000 -0500
+++ u-boot-1.1.4-wpcm/cpu/arm926ejs/WPCM450/kcsifc.c	2007-11-26 19:10:38.000000000 -0500
@@ -0,0 +1,371 @@
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
+ * KCS.c
+ * KCS Functions.
+ *
+ *  Author: Rama Rao Bisa <ramab@ami.com>
+ *			Modified by Anurag Bhatia for SP porting		
+ ******************************************************************/
+
+#include <common.h>
+#include <config.h>
+
+#ifdef CFG_YAFU_SUPPORT
+
+//#include "hwmap.h"
+#include "kcs.h"
+#include "kcsregs.h"
+
+#define PACKED __attribute__ ((packed))
+
+/*** Local definitions ***/
+
+/* Different Phases of the KCS Module */
+#define KCS_PHASE_IDLE			    0x00
+#define KCS_PHASE_WRITE			    0x01
+#define KCS_PHASE_WRITE_END		    0x02
+#define KCS_PHASE_READ			    0x03
+#define KCS_PHASE_ABORT			    0x04
+#define KCS_PHASE_ERROR			    0x05
+#define ABORT_PHASE_ERROR1		    0x02
+#define ABORT_PHASE_ERROR2		    0x03
+
+/* Different KCS States */
+#define KCS_IDLE_STATE	            0x00
+#define KCS_READ_STATE	            0x40
+#define KCS_WRITE_STATE             0x80
+#define KCS_ERROR_STATE	            0xC0
+
+/* KCS Command Control Codes */
+#define KCS_GET_STATUS				0x60
+#define KCS_ABORT					0x60
+#define KCS_WRITE_START				0x61
+#define KCS_WRITE_END				0x62
+#define KCS_READ					0x68
+
+/* KCS Error Codes */
+#define KCS_NO_ERROR				0x00
+#define KCS_ABORTED_BY_COMMAND		0x01
+#define KCS_ILLEGAL_CONTROL_CODE	0x02
+#define KCS_LENGTH_ERROR			0x06
+#define KCS_UNSPECIFIED_ERROR		0xff
+
+
+
+/*** Local typedefs ***/
+/**
+ * @struct KCSBuf_T	
+ * @brief KCS Buffer.
+**/
+#pragma pack(1)
+typedef struct
+{
+	int			    KCSRcvPktIx;
+	unsigned long		    KCSSendPktIx;
+	unsigned long		    KCSSendPktLen;
+	unsigned char	KCSRcvPkt [MAX_KCS_PKT_LEN];
+	unsigned char	KCSSendPkt [MAX_KCS_PKT_LEN];
+	unsigned char   KCSPhase;	
+	unsigned char   AbortPhase;
+	unsigned char   KCSError;
+	unsigned char   KCSRcvPktReady;
+
+} PACKED KCSBuf_T;
+#pragma pack ()
+
+/*** Module variables ***/
+static KCSBuf_T					m_KCSBuf;
+static unsigned short			m_KCSPortNo;
+volatile static unsigned char   *lpc_base;
+
+/*** Prototype Declaration ***/
+static void OnKCSRcvByte 	(void);
+
+/*------------------------------------
+ * InitKCS
+ *------------------------------------*/
+int
+InitKCS (void)
+{
+//	unsigned char 	CorePresence;
+	KCSBuf_T		*pKCSBuf;
+
+	pKCSBuf = &m_KCSBuf;
+
+	pKCSBuf->KCSRcvPktIx    = 0;
+	pKCSBuf->KCSSendPktIx   = 0;
+	pKCSBuf->KCSSendPktLen  = 0;
+	pKCSBuf->KCSPhase	    = 0;
+	pKCSBuf->AbortPhase     = 0;
+	pKCSBuf->KCSError	    = 0;
+	pKCSBuf->KCSRcvPktReady = 0;
+
+	m_KCSPortNo				= SMS_KCS_BASE_ADDR;
+	lpc_base				= (unsigned char *) LPC_BASE_ADDR;
+
+
+	INIT_KCS_HARDWARE ();
+
+	return 0;
+}
+
+/**
+ * @brief RecvKCSRequest
+**/
+int
+RecvKCSRequest (unsigned char *pKCSPkt)
+{
+
+	KCSBuf_T*  pKCSBuf =  &m_KCSBuf;
+	
+	OnKCSRcvByte ();
+
+	if (pKCSBuf->KCSRcvPktReady)
+	{
+		pKCSBuf->KCSRcvPktReady = 0;
+		memcpy (pKCSPkt, pKCSBuf->KCSRcvPkt, pKCSBuf->KCSRcvPktIx);
+		return (pKCSBuf->KCSRcvPktIx);
+	}
+	return 0;
+}
+
+
+/**
+ * @brief KCS Byte-receive handler.
+**/
+static void
+OnKCSRcvByte (void)
+{
+	unsigned char   Status;
+	unsigned char   DummyByte;
+	KCSBuf_T*		pKCSBuf;
+
+	pKCSBuf = &m_KCSBuf;
+
+	/* Read the Present Status of KCS Port */
+	Status = 0;	
+
+	READ_KCS_STATUS_REG (Status);
+
+	if(!(IBF_BIT_SET(Status)))
+	{
+		return;
+	}
+	/* If write to command register */
+	if (IS_WRITE_TO_CMD_REG (Status))
+	{
+		unsigned char	Cmd;
+		
+		Cmd = 0;
+		
+		/* Set the status to WRITE_STATE */
+		SET_KCS_STATE (KCS_WRITE_STATE);
+		/* Read the command */
+		KCS_CMD_REG (Cmd);
+		
+		switch (Cmd)
+		{
+		case KCS_WRITE_START :
+
+			/* Set the Index to 0 */
+			pKCSBuf->KCSRcvPktIx = 0;
+			/* Set the phase to WRITE */
+			pKCSBuf->KCSPhase = KCS_PHASE_WRITE;
+			break;
+
+		case KCS_WRITE_END 	 :
+
+            /* Set the phase to write end */
+			pKCSBuf->KCSPhase = KCS_PHASE_WRITE_END;
+			break;
+
+		case KCS_ABORT       : 
+
+            /* Set the error code */
+			if (KCS_NO_ERROR == pKCSBuf->KCSError)
+			{
+				pKCSBuf->KCSError = KCS_ABORTED_BY_COMMAND; 
+			}
+			/* Set the phase to write end */
+			pKCSBuf->KCSPhase = KCS_PHASE_ABORT;
+			/* Set the abort phase to be error1 */
+			pKCSBuf->AbortPhase = ABORT_PHASE_ERROR1;
+			/* Send the dummy byte	*/
+			KCS_DATA_OUT_REG (0);
+			SET_OBF_STATUS();
+
+			break;
+
+		default :
+
+            /* Set the error code */
+			pKCSBuf->KCSError = KCS_ILLEGAL_CONTROL_CODE;
+			/* Invalid command code - Set an error state */
+			SET_KCS_STATE (KCS_ERROR_STATE);
+			/* Set the phase to error phase */
+			pKCSBuf->KCSPhase = KCS_PHASE_ERROR;
+			break;
+		}
+	}
+
+	/* Else last write was to data register */
+	else
+	{
+		switch (pKCSBuf->KCSPhase)
+		{
+		case KCS_PHASE_WRITE :
+
+            /* Set the state to write state */
+			SET_KCS_STATE (KCS_WRITE_STATE);
+			/* Read the BYTE from the data register */
+			KCS_DATA_IN_REG (pKCSBuf->KCSRcvPkt [pKCSBuf->KCSRcvPktIx]);
+			if (pKCSBuf->KCSRcvPktIx < MAX_KCS_PKT_LEN)
+			{
+				pKCSBuf->KCSRcvPktIx++;
+			}
+			break;
+
+		case KCS_PHASE_WRITE_END :
+
+            /* Set the state to READ_STATE */
+			SET_KCS_STATE (KCS_READ_STATE);
+			/* Read the BYTE from the data register */
+			KCS_DATA_IN_REG (pKCSBuf->KCSRcvPkt [pKCSBuf->KCSRcvPktIx]);
+			pKCSBuf->KCSRcvPktIx++;
+
+			/* Move to READ Phase */
+			pKCSBuf->KCSPhase = KCS_PHASE_READ;
+
+			/* Signal receive data ready */
+			pKCSBuf->KCSRcvPktReady = 1;
+
+			break;
+
+		case KCS_PHASE_READ		 :
+
+            /* If we have reached the end of the packet move to idle state */
+			if (pKCSBuf->KCSSendPktIx == pKCSBuf->KCSSendPktLen)
+			{
+				SET_KCS_STATE (KCS_IDLE_STATE);
+			}
+			/* Read the byte returned by the SMS */
+			{ 
+				unsigned char b = 0;
+				KCS_DATA_IN_REG (b); 
+				if (b != KCS_READ)
+				{
+					SET_KCS_STATE (KCS_ERROR_STATE);
+					KCS_DATA_OUT_REG (0);
+					SET_OBF_STATUS();
+					break;
+				}
+			}
+			/* If we are finished transmitting, send the dummy byte */
+			if (pKCSBuf->KCSSendPktIx == pKCSBuf->KCSSendPktLen) 
+			{
+				pKCSBuf->KCSPhase = KCS_PHASE_IDLE;
+				KCS_DATA_OUT_REG (0);
+				SET_OBF_STATUS();
+#if 0 // ANURAGB
+				/* Set Transmission Complete */
+                pKCSBuf->TxReady                = 1;
+#endif
+				break;
+			}
+			/* Transmit the next byte from the send buffer */
+			KCS_DATA_OUT_REG (pKCSBuf->KCSSendPkt [pKCSBuf->KCSSendPktIx]);
+			SET_OBF_STATUS();
+			pKCSBuf->KCSSendPktIx++;
+			break;
+			
+		case KCS_PHASE_ABORT 	 :
+
+            switch (pKCSBuf->AbortPhase) 
+			{
+			case ABORT_PHASE_ERROR1 :
+				/* Set the KCS State to READ_STATE */
+				SET_KCS_STATE (KCS_READ_STATE);
+				/* Read the Dummy byte	*/
+				KCS_DATA_IN_REG (DummyByte); 
+
+				/* Write the error code to Data out register */
+				KCS_DATA_OUT_REG (pKCSBuf->KCSError);
+
+				SET_OBF_STATUS();
+
+				/* Set the abort phase to be error2 */
+				pKCSBuf->AbortPhase = ABORT_PHASE_ERROR2;
+
+				break;
+				
+			case ABORT_PHASE_ERROR2 :
+
+                /**
+				 * The system software has read the error code. Go to idle
+				 * state.
+				**/
+				SET_KCS_STATE (KCS_IDLE_STATE);
+
+				/* Read the Dummy byte	*/
+				KCS_DATA_IN_REG (DummyByte); 
+
+				pKCSBuf->KCSPhase = KCS_PHASE_IDLE;
+				pKCSBuf->AbortPhase = 0;
+
+				/* Send the dummy byte	*/
+				KCS_DATA_OUT_REG (0);
+				SET_OBF_STATUS();
+			}
+			break;
+
+			default:
+				/* Read the Dummy byte	*/
+				KCS_DATA_IN_REG (DummyByte); 
+		}
+	}
+}
+
+
+/**
+ * @brief Send KCS Response
+**/
+void
+SendKCSResponse (unsigned char *Pkt, int Size)
+{
+	KCSBuf_T*	pKCSBuf = &m_KCSBuf;
+#ifdef DEBUG
+	printf ("Send KCS Resposne of 0x%x bytes with completion code 0x%x\n", Size, Pkt [2]);
+#endif
+	pKCSBuf->KCSSendPktIx   = 0;
+	pKCSBuf->KCSSendPktLen = Size;
+	memcpy (pKCSBuf->KCSSendPkt, Pkt, Size);
+	/* Send the first byte */
+	pKCSBuf->KCSSendPktIx++;
+	KCS_DATA_OUT_REG (pKCSBuf->KCSSendPkt [0]);
+	SET_OBF_STATUS();
+
+	return;
+}
+#endif
+
