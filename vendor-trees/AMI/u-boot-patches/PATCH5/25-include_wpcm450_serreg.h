--- u-boot-1.1.4-cpu/include/wpcm450/serreg.h	1969-12-31 19:00:00.000000000 -0500
+++ u-boot-1.1.4-wpcm/include/wpcm450/serreg.h	2007-06-28 00:32:27.000000000 -0400
@@ -0,0 +1,95 @@
+#ifndef WPCM450_SER_REG_H
+#define WPCM450_SER_REG_H
+
+#define ENABLE	1
+#define	DISABLE	0
+
+#define PARITY_NONE		0
+#define PARITY_ODD		1
+#define PARITY_EVEN		2
+#define PARITY_MARK		3
+#define PARITY_SPACE	4
+
+
+#define ON		1
+#define	OFF		0
+
+/* -------------------------------------------------------------------------------
+ * WPCM450  UART definitions
+ * -------------------------------------------------------------------------------
+ */
+#define SERIAL_THR                     0x00	 		/*  Transmitter Holding Register(Write).*/
+#define SERIAL_RBR                     0x00	 		/*  Receive Buffer register (Read).*/
+#define SERIAL_IER                     0x04	 		/*  Interrupt Enable register.*/
+#define SERIAL_IIR                     0x08	 		/*  Interrupt Identification register(Read).*/
+#define SERIAL_FCR                     0x08	 		/*  FIFO control register(Write).*/
+#define SERIAL_LCR                     0x0C	 		/*  Line Control register.*/
+#define SERIAL_MCR                     0x10	 		/*  Modem Control Register.*/
+#define SERIAL_LSR                     0x14	 		/*  Line status register(Read) .*/
+#define SERIAL_MSR                     0x18	 		/*  Modem Status register (Read).*/
+#define SERIAL_DLL                     0x0      	/*  Divisor Register LSB */
+#define SERIAL_DLM                     0x4      	/*  Divisor Register MSB */
+
+/* IER Register */
+#define SERIAL_IER_DR                  0x1      	/* Data ready Enable */
+#define SERIAL_IER_TE                  0x2      	/* THR Empty Enable */
+#define SERIAL_IER_RLS                 0x4      	/* Receive Line Status Enable */
+#define SERIAL_IER_MS                  0x8      	/* Modem Staus Enable */
+
+/* IIR Register */
+#define SERIAL_IIR_NONE                0x1			/* No interrupt pending */
+#define SERIAL_IIR_RLS                 0x6			/* Receive Line Status */
+#define SERIAL_IIR_DR                  0x4			/* Receive Data Ready */
+#define SERIAL_IIR_TIMEOUT             0xc			/* Receive Time Out */
+#define SERIAL_IIR_TE                  0x2			/* THR Empty */
+#define SERIAL_IIR_MODEM               0x0			/* Modem Status */
+
+/* FCR Register */
+#define SERIAL_FCR_FE                  0x1 	 		/* FIFO Enable */
+#define SERIAL_FCR_RXFR                0x2 	 		/* Rx FIFO Reset */
+#define SERIAL_FCR_TXFR                0x4 	 		/* Tx FIFO Reset */
+
+/* LCR Register */
+#define SERIAL_LCR_LEN5                0x0
+#define SERIAL_LCR_LEN6                0x1
+#define SERIAL_LCR_LEN7                0x2
+#define SERIAL_LCR_LEN8                0x3
+
+#define SERIAL_LCR_STOP                0x4
+#define SERIAL_LCR_EVEN                0x18 	 	/* Even Parity */
+#define SERIAL_LCR_ODD                 0x8      	/* Odd Parity */
+#define SERIAL_LCR_PE                  0x8			/* Parity Enable */
+#define SERIAL_LCR_SETBREAK            0x40	 		/* Set Break condition */
+#define SERIAL_LCR_STICKPARITY         0x20	 		/* Stick Parity Enable */
+#define SERIAL_LCR_DLAB                0x80     	/* Divisor Latch Access Bit */
+
+/* LSR Register */
+#define SERIAL_LSR_DR                  0x1      	/* Data Ready */
+#define SERIAL_LSR_OE                  0x2      	/* Overrun Error */
+#define SERIAL_LSR_PE                  0x4      	/* Parity Error */
+#define SERIAL_LSR_FE                  0x8      	/* Framing Error */
+#define SERIAL_LSR_BI                  0x10     	/* Break Interrupt */
+#define SERIAL_LSR_THRE                0x20     	/* THR Empty */
+#define SERIAL_LSR_TE                  0x40     	/* Transmitte Empty */
+#define SERIAL_LSR_DE                  0x80     	/* FIFO Data Error */
+
+/* MCR Register */
+#define SERIAL_MCR_DTR                 0x1		/* Data Terminal Ready */
+#define SERIAL_MCR_RTS                 0x2		/* Request to Send */
+#define SERIAL_MCR_OUT1                0x4		/* output	1 */
+#define SERIAL_MCR_OUT2                0x8		/* output2 or global interrupt enable */
+#define SERIAL_MCR_LPBK                0x10	 	/* loopback mode */
+
+
+/* MSR Register */
+#define SERIAL_MSR_DELTACTS            0x1		/* Delta CTS */
+#define SERIAL_MSR_DELTADSR            0x2		/* Delta DSR */
+#define SERIAL_MSR_TERI                0x4		/* Trailing Edge RI */
+#define SERIAL_MSR_DELTACD             0x8		/* Delta CD */
+#define SERIAL_MSR_CTS                 0x10	 	/* Clear To Send */
+#define SERIAL_MSR_DSR                 0x20	 	/* Data Set Ready */
+#define SERIAL_MSR_RI                  0x40	 	/* Ring Indicator */
+#define SERIAL_MSR_DCD                 0x80	 	/* Data Carrier Detect */
+
+#endif
+
