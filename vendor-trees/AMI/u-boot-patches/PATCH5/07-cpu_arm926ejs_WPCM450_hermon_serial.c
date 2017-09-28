--- u-boot-1.1.4-cpu/cpu/arm926ejs/WPCM450/hermon_serial.c	1969-12-31 19:00:00.000000000 -0500
+++ u-boot-1.1.4-wpcm/cpu/arm926ejs/WPCM450/hermon_serial.c	2007-06-28 01:58:59.000000000 -0400
@@ -0,0 +1,293 @@
+#include "soc_hw.h"
+#include "macros.h"
+#include "clocks.h"
+#include "hermon_serial.h"
+
+#define serial_inw(addr) 			(*((volatile UINT32 *)(addr)))
+#define serial_outw(addr, value)  	(*((volatile UINT32 *)(addr)) = value)
+
+
+
+static
+void 
+WPCM_global_uart_init(void)
+{
+  /* Muxing for UART0  */
+  SET_BIT(*(volatile unsigned long *)GCR_MFSEL1, MF_BSPSEL_BIT);
+  CLEAR_BIT(*(volatile unsigned long *)GCR_MFSEL1, MF_HSP1SEL_BIT);
+  CLEAR_BIT(*(volatile unsigned long *)GCR_MFSEL1, MF_HSP2SEL_BIT);
+
+    
+  /* Select 48M clock source for UART in case that real chip */
+  SET_FIELD(*(volatile unsigned long *)CLK_SEL, CLK_UART_SRC, CLK_48M_SRC_SEL);
+  
+  /* Serial Interface pins 2 selected. */
+  SET_BIT(*(volatile unsigned long *)GCR_MFSEL1, MF_HSP2SEL_BIT);
+
+  /* Serial Port Mode 2 selected. (Core Take-ove) */
+  SET_FIELD( *(volatile unsigned long *)GCR_SPSWC, SPSWC_SPMOD, SPSWC_SPMOD_3);	
+
+  return;
+}
+
+
+
+
+
+
+
+void
+WPCM_SerialInit(UINT32 port, UINT32 baudrate, UINT32 parity,UINT32 num,UINT32 len)
+{
+	UINT32 lcr;
+
+	WPCM_global_uart_init();
+    serial_outw(port+SERIAL_IER,0);
+
+    lcr = serial_inw(port + SERIAL_LCR) & ~SERIAL_LCR_DLAB;
+	/* Set DLAB=1 */
+    serial_outw(port + SERIAL_LCR,SERIAL_LCR_DLAB);
+    /* Set baud rate */
+    serial_outw(port + SERIAL_DLM, ((baudrate & 0xf00) >> 8));
+    serial_outw(port + SERIAL_DLL, (baudrate & 0xff));
+
+	//clear orignal parity setting
+	lcr &= 0xc0;
+
+	switch (parity)
+	{
+		case PARITY_NONE:
+			//do nothing
+    		break;
+    	case PARITY_ODD:
+		    lcr|=SERIAL_LCR_ODD;
+   		 	break;
+    	case PARITY_EVEN:
+    		lcr|=SERIAL_LCR_EVEN;
+    		break;
+    	case PARITY_MARK:
+    		lcr|=(SERIAL_LCR_STICKPARITY|SERIAL_LCR_ODD);
+    		break;
+    	case PARITY_SPACE:
+    		lcr|=(SERIAL_LCR_STICKPARITY|SERIAL_LCR_EVEN);
+    		break;
+
+    	default:
+    		break;
+    }
+
+    if(num==2)
+		lcr|=SERIAL_LCR_STOP;
+
+	len-=5;
+
+	lcr|=len;
+
+    serial_outw(port+SERIAL_LCR,lcr);
+}
+
+
+void
+WPCM_SetSerialLoopback(UINT32 port, UINT32 onoff)
+{
+	UINT32 temp;
+
+	temp=serial_inw(port+SERIAL_MCR);
+	if(onoff==ON)
+		temp|=SERIAL_MCR_LPBK;
+	else
+		temp&=~(SERIAL_MCR_LPBK);
+
+	serial_outw(port+SERIAL_MCR,temp);
+}
+
+void
+WPCM_SetSerialFifoCtrl(UINT32 port, UINT32 level, UINT32 resettx, UINT32 resetrx)
+{
+	UINT8 fcr = 0;
+
+ 	fcr |= SERIAL_FCR_FE;
+
+ 	switch(level)
+ 	{
+ 		case 4:
+ 			fcr|=0x40;
+ 			break;
+ 		case 8:
+ 			fcr|=0x80;
+ 			break;
+ 		case 14:
+ 			fcr|=0xc0;
+ 			break;
+ 		default:
+ 			break;
+ 	}
+
+	if(resettx)
+		fcr|=SERIAL_FCR_TXFR;
+
+	if(resetrx)
+		fcr|=SERIAL_FCR_RXFR;
+
+	serial_outw(port+SERIAL_FCR,fcr);
+}
+
+
+void
+WPCM_DisableSerialFifo(UINT32 port)
+{
+	serial_outw(port+SERIAL_FCR,0);
+}
+
+
+void
+WPCM_SetSerialInt(UINT32 port, UINT32 IntMask)
+{
+	serial_outw(port + SERIAL_IER, IntMask);
+}
+
+
+char
+WPCM_GetSerialChar(UINT32 port)
+{
+    char Ch;
+	UINT32 status;
+
+   	do
+	{
+	 	status=serial_inw(port+SERIAL_LSR);
+	}
+	while (!((status & SERIAL_LSR_DR)==SERIAL_LSR_DR));	// wait until Rx ready
+    Ch = serial_inw(port + SERIAL_RBR);
+    return (Ch);
+}
+
+void
+WPCM_PutSerialChar(UINT32 port, char Ch)
+{
+  	UINT32 status;
+
+    do
+	{
+	 	status=serial_inw(port+SERIAL_LSR);
+	}while (!((status & SERIAL_LSR_THRE)==SERIAL_LSR_THRE));	// wait until Tx ready
+    serial_outw(port + SERIAL_THR,Ch);
+}
+
+void
+WPCM_PutSerialStr(UINT32 port, char *Str)
+{
+  	char *cp;
+
+ 	for(cp = Str; *cp != 0; cp++)
+	{
+   		WPCM_PutSerialChar(port, *cp);
+    	if(*cp == '\n')
+	   		WPCM_PutSerialChar(port, '\r');
+	}
+}
+
+
+void
+WPCM_EnableSerialInt(UINT32 port, UINT32 mode)
+{
+	UINT32 data;
+
+	data = serial_inw(port + SERIAL_IER);
+	serial_outw(port + SERIAL_IER, data | mode);
+}
+
+
+void
+WPCM_DisableSerialInt(UINT32 port, UINT32 mode)
+{
+	UINT32 data;
+
+	data = serial_inw(port + SERIAL_IER);
+	mode = data & (~mode);
+	serial_outw(port + SERIAL_IER, mode);
+}
+
+UINT32
+WPCM_SerialIntIdentification(UINT32 port)
+{
+	return serial_inw(port + SERIAL_IIR);
+}
+
+void
+WPCM_SetSerialLineBreak(UINT32 port)
+{
+	UINT32 data;
+
+	data = serial_inw(port + SERIAL_LCR);
+	serial_outw(port + SERIAL_LCR, data | SERIAL_LCR_SETBREAK);
+}
+
+void
+WPCM_SetSerialLoopBack(UINT32 port,UINT32 onoff)
+{
+	UINT32 temp;
+
+	temp = serial_inw(port+SERIAL_MCR);
+	if(onoff == ON)
+		temp |= SERIAL_MCR_LPBK;
+	else
+		temp &= ~(SERIAL_MCR_LPBK);
+
+	serial_outw(port+SERIAL_MCR,temp);
+}
+
+void
+WPCM_SerialRequestToSend(UINT32 port)
+{
+	UINT32 data;
+
+	data = serial_inw(port + SERIAL_MCR);
+	serial_outw(port + SERIAL_MCR, data | SERIAL_MCR_RTS);
+}
+
+void
+WPCM_SerialStopToSend(UINT32 port)
+{
+	UINT32 data;
+
+	data = serial_inw(port + SERIAL_MCR);
+	data &= ~(SERIAL_MCR_RTS);
+	serial_outw(port + SERIAL_MCR, data);
+}
+
+void
+WPCM_SerialDataTerminalReady(UINT32 port)
+{
+	UINT32 data;
+
+	data = serial_inw(port + SERIAL_MCR);
+	serial_outw(port + SERIAL_MCR, data | SERIAL_MCR_DTR);
+}
+
+void
+WPCM_SerialDataTerminalNotReady(UINT32 port)
+{
+	UINT32 data;
+
+	data = serial_inw(port + SERIAL_MCR);
+	data &= ~(SERIAL_MCR_DTR);
+	serial_outw(port + SERIAL_MCR, data);
+}
+
+UINT32
+WPCM_ReadSerialLineStatus(UINT32 port)
+{
+	return serial_inw(port + SERIAL_LSR);
+}
+
+UINT32
+WPCM_ReadSerialModemStatus(UINT32 port)
+{
+	return serial_inw(port + SERIAL_MSR);
+}
+
+int WPCM_TestSerialForChar(UINT32 port)
+{
+ 	return ((serial_inw( port+ SERIAL_LSR ) & SERIAL_LSR_DR ) == SERIAL_LSR_DR);
+}
