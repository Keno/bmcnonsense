--- linux/include/asm-arm/soc-wpcm450/hwdef.h	1969-12-31 19:00:00.000000000 -0500
+++ linux.mb/include/asm-arm/soc-wpcm450/hwdef.h	2007-07-06 17:33:28.000000000 -0400
@@ -0,0 +1,205 @@
+/*
+ *  This file contains the WPCM450  SOC Register definitions
+ *
+ *  Copyright (C) 2005 American Megatrends Inc
+ *
+ * This program is free software; you can redistribute it and/or modify
+ * it under the terms of the GNU General Public License as published by
+ * the Free Software Foundation; either version 2 of the License, or
+ * (at your option) any later version.
+ *
+ * This program is distributed in the hope that it will be useful,
+ * but WITHOUT ANY WARRANTY; without even the implied warranty of
+ * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
+ * GNU General Public License for more details.
+ *
+ * You should have received a copy of the GNU General Public License
+ * along with this program; if not, write to the Free Software
+ * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
+ */
+
+#ifndef     _HWDEF_WPCM450_H_
+#define     _HWDEF_WPCM450_H_
+
+
+
+/*--------------------------------------------------------
+ *  Timer definitions
+ * ------------------------------------------------------
+ */
+#define TIMER_CEN			1 << 30
+#define TIMER_IE			1 << 29
+#define TIMER_PERIODICAL		1 << 27
+
+/* --------------------------------------------------------
+ *  Interrupt Controllers
+ * --------------------------------------------------------
+ */
+
+
+/*----------------------------------------------------------------
+ *   Serial  Controllers
+ *----------------------------------------------------------------
+ */
+#define WPCM_UART_CLOCK      UART_CLOCK	/* defined in board config */
+#define WPCM_BAUD_115200    (WPCM_UART_CLOCK /1843200)
+#define WPCM_BAUD_57600	    (WPCM_UART_CLOCK /921600)
+#define WPCM_BAUD_38400	    (WPCM_UART_CLOCK /614400)
+#define WPCM_BAUD_19200	    (WPCM_UART_CLOCK /307200)
+#define WPCM_BAUD_14400	    (WPCM_UART_CLOCK /230400)
+#define WPCM_BAUD_9600	    (WPCM_UART_CLOCK /153600)
+
+
+/*----------------------------------------------------------------
+ * Global Control Register                                          
+ *----------------------------------------------------------------
+ */
+/* GCR PWON register fields */
+#define PWRON_VGA_BIOS_BIT    12
+#define PWRON_FPROG_EN_BIT    11
+#define PWRON_CPU_CORE_CLK_S   3
+#define PWRON_CPU_CORE_CLK_P   8
+#define PWRON_BIOSEN_BIT       7
+#define PWRON_RAM_SIZE_S       2
+#define PWRON_RAM_SIZE_P       2
+#define PWRON_BIOS_FL_SIZE_S   2
+#define PWRON_BIOS_FL_SIZE_P   0
+
+
+/* GCR MFSEL1 register fields */
+#define MF_MBEN_BIT     26
+#define MF_GSPISEL_BIT  24
+#define MF_CLKOSEL_BIT  21
+#define MF_DVOSEL_S      3
+#define MF_DVOSEL_P     18
+#define MF_R2MDSEL_BIT  16
+#define MF_R2ERRSEL_BIT 15
+#define MF_RMII2SEL_BIT 14 
+#define MF_R1MDSEL_BIT  13
+#define MF_R1ERRSEL_BIT 12
+#define MF_HSP2SEL_BIT  11
+#define MF_HSP1SEL_BIT  10
+#define MF_BSPSEL_BIT    9
+#define MF_SCS3SEL_BIT   5
+#define MF_SCS2SEL_BIT   4
+#define MF_SCS1SEL_BIT   3
+
+/* GCR MFSEL2 register fields */
+#define MF_PWM3SEL_BIT  19
+
+/* GCR INTCR registers fields */
+#define MF_VGAIOEN_BIT  6
+
+/* GCR ESTR register fields */
+#define ESTR_P1_BIT     0
+
+/*----------------------------------------------------------------
+ * Clock Bits                                          
+ *----------------------------------------------------------------
+*/
+#define CLK_SMB1_BIT    31
+#define CLK_SMB0_BIT    30
+#define CLK_ADC_BIT     27
+#define CLK_WDT_BIT     26
+#define CLK_MFT1_BIT    25
+#define CLK_MFT0_BIT    24
+#define CLK_TIMER4_BIT  23
+#define CLK_TIMER3_BIT  22
+#define CLK_TIMER2_BIT  21
+#define CLK_TIMER1_BIT  20
+#define CLK_TIMER0_BIT  19
+#define CLK_PWM_BIT     18
+#define CLK_HUART_BIT   17
+#define CLK_SMB5_BIT    16
+#define CLK_SMB4_BIT    15
+#define CLK_SMB3_BIT    14
+#define CLK_SMB2_BIT    13
+#define CLK_UART1_BIT   12
+#define CLK_UART0_BIT   11
+#define CLK_AES_BIT     10
+#define CLK_USB1_BIT     9
+#define CLK_USB0_BIT     8
+#define CLK_EMC1_BIT     7
+#define CLK_EMC0_BIT     6
+#define CLK_RNG_BIT      5
+#define CLK_SHM_BIT      4
+#define CLK_GDMA_BIT     3
+#define CLK_KCS_BIT      2
+#define CLK_XBUS_BIT     1
+#define CLK_FIU_BIT      0
+
+/* CLK_SEL register bits */
+
+#define CLK_HUART_SRC_BIT   10
+#define CLK_UART_SRC_P       8
+#define CLK_UART_SRC_S       2
+#define CLK_USB1CKSEL_P      6
+#define CLK_USB1CKSEL_S      2
+#define CLK_PIX_SRC_P        4
+#define CLK_PIX_SRC_S        2
+#define CLK_CPU_OUT_P        2
+#define CLK_CPU_OUT_S        2
+#define CLK_CPU_CLK_P        0
+#define CLK_CPU_CLK_S        2
+
+
+#define CLK_ADCCKDIV_P      28
+#define CLK_ADCCKDIV_S       2
+#define CLK_APBCKDIV_P      26
+#define CLK_APBCKDIV_S       2
+#define CLK_AHBCKDIV_P      24
+#define CLK_AHBCKDIV_S       2
+#define CLK_UARTDIV_P       16
+#define CLK_UARTDIV_S        4
+#define CLK_AHB3CKDIV_P      8
+#define CLK_AHB3CKDIV_S      2
+#define CLK_MCCKDIV_P        0
+#define CLK_MCCKDIV_S        3
+
+
+/* CLK_PLLCON0 CLK_PLLCON1 fields */
+#define CLK_FBDV_S           9
+#define CLK_FBDV_P           16    
+#define CLK_PRST_BIT         13
+#define CLK_PWDEN_BIT        12
+#define CLK_OTDV_S           3
+#define CLK_OTDV_P           8 
+#define CLK_INDV_S           6
+#define CLK_INDV_P           0
+
+
+
+#define CLK_RESET_CTRL_BIT    3
+#define CLK_MIDLE_ENABLE_BIT  2
+#define CLK_PDOWN_ENABLE_BIT  1
+#define CLK_IDLE_ENABLE_BIT   0
+
+#define CLK_IRQWAKEUPPOL_P   16
+#define CLK_IRQWAKEUPPOL_S   16
+#define CLK_IRQWAKEUPEN_P     0
+#define CLK_IRQWAKEUPEN_S    16
+#define CLK_IRQWAKEFLAG_P     0
+#define CLK_IRQWAKEFLAG_S    16
+
+/* CLK_IPSRST fields */
+#define CLK_IPSRST_SMB1_BIT    31
+#define CLK_IPSRST_SMB0_BIT    30
+#define CLK_IPSRST_ADC_BIT     27
+#define CLK_IPSRST_TIMER_BIT   19
+#define CLK_IPSRST_PWM_BIT     18
+#define CLK_IPSRST_SMB5_BIT    16
+#define CLK_IPSRST_SMB4_BIT    15
+#define CLK_IPSRST_SMB3_BIT    14
+#define CLK_IPSRST_SMB2_BIT    13
+#define CLK_IPSRST_MC_BIT      12
+#define CLK_IPSRST_UART_BIT    11
+#define CLK_IPSRST_AES_BIT     10
+#define CLK_IPSRST_PECI_BIT     9
+#define CLK_IPSRST_USB2_BIT     8
+#define CLK_IPSRST_EMC1_BIT     7
+#define CLK_IPSRST_EMC0_BIT     6
+#define CLK_IPSRST_USB1_BIT     5
+#define CLK_IPSRST_GDMA_BIT     3
+
+
+#endif
