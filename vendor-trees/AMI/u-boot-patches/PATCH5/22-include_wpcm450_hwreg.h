--- u-boot/include/wpcm450/hwreg.h	1969-12-31 17:00:00.000000000 -0700
+++ u-boot-new/include/wpcm450/hwreg.h	2008-06-04 17:34:04.000000000 -0500
@@ -0,0 +1,129 @@
+/*
+ *  This file contains the WPCM450  SOC Register locations
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
+#ifndef     _HWREG_WPCM450_H
+#define     _HWREG_WPCM450_H
+
+
+
+/* -------------------------------------------------------------------
+ *  		System Global Control Registers
+ * -------------------------------------------------------------------
+ */
+#define GCR_PDID			(WPCM_GCR_VA_BASE + 0x00)
+#define GCR_PWRON  			(WPCM_GCR_VA_BASE + 0x04)
+#define GCR_MFSEL1			(WPCM_GCR_VA_BASE + 0x0C)
+#define GCR_MFSEL2			(WPCM_GCR_VA_BASE + 0x10)
+#define GCR_SPSWC			(WPCM_GCR_VA_BASE + 0x38)
+#define GCR_INTCR 			(WPCM_GCR_VA_BASE + 0x3C)
+#define GCR_INTCR2 			(WPCM_GCR_VA_BASE + 0x60)
+#define GCR_ETSR   			(WPCM_GCR_VA_BASE + 0x108)
+
+/* -------------------------------------------------------------------
+ *  					Timer Registers
+ * -------------------------------------------------------------------
+ */
+#define TIMER_TCSR0                (WPCM_TIMER_VA_BASE + 0x00)
+#define TIMER_TCSR1                (WPCM_TIMER_VA_BASE + 0x04)
+#define TIMER_TICR0                (WPCM_TIMER_VA_BASE + 0x08)
+#define TIMER_TICR1                (WPCM_TIMER_VA_BASE + 0x0C)
+#define TIMER_TDR0                 (WPCM_TIMER_VA_BASE + 0x10)
+#define TIMER_TDR1                 (WPCM_TIMER_VA_BASE + 0x14)
+#define TIMER_TISR                 (WPCM_TIMER_VA_BASE + 0x18)
+#define TIMER_WTCR                 (WPCM_TIMER_VA_BASE + 0x1C)
+#define TIMER_TCSR2                (WPCM_TIMER_VA_BASE + 0x20)
+#define TIMER_TCSR3                (WPCM_TIMER_VA_BASE + 0x24)
+#define TIMER_TICR2                (WPCM_TIMER_VA_BASE + 0x28)
+#define TIMER_TICR3                (WPCM_TIMER_VA_BASE + 0x2C)
+#define TIMER_TDR2                 (WPCM_TIMER_VA_BASE + 0x30)
+#define TIMER_TDR3                 (WPCM_TIMER_VA_BASE + 0x34)
+#define TIMER_TCSR4                (WPCM_TIMER_VA_BASE + 0x40)
+#define TIMER_TICR4                (WPCM_TIMER_VA_BASE + 0x48)
+#define TIMER_TDR4                 (WPCM_TIMER_VA_BASE + 0x50)
+
+
+/* -----------------------------------------------------------------
+ * 				 Interrupt Controller Register
+ * -----------------------------------------------------------------
+ */
+#define AIC_SCR(x)		(WPCM_AIC_VA_BASE + ((x)*4))
+#define AIC_GEN			(WPCM_AIC_VA_BASE + 0x84)
+#define AIC_GRSR		(WPCM_AIC_VA_BASE + 0x88)
+#define AIC_IRSR		(WPCM_AIC_VA_BASE + 0x100)
+#define AIC_IASR		(WPCM_AIC_VA_BASE + 0x104)
+#define AIC_ISR			(WPCM_AIC_VA_BASE + 0x108)
+#define AIC_IPER		(WPCM_AIC_VA_BASE + 0x10C)
+#define AIC_ISNR		(WPCM_AIC_VA_BASE + 0x110)
+#define AIC_IMR			(WPCM_AIC_VA_BASE + 0x114)
+#define AIC_OISR		(WPCM_AIC_VA_BASE + 0x118)
+#define AIC_MECR		(WPCM_AIC_VA_BASE + 0x120)
+#define AIC_MDCR		(WPCM_AIC_VA_BASE + 0x124)
+#define AIC_SSCR		(WPCM_AIC_VA_BASE + 0x128)
+#define AIC_SCCR		(WPCM_AIC_VA_BASE + 0x12C)
+#define AIC_EOSCR		(WPCM_AIC_VA_BASE + 0x130)
+
+
+/* -----------------------------------------------------------------
+ * 				 Clock Register
+ * -----------------------------------------------------------------
+ */
+#define CLK_EN   	  (WPCM_CLK_VA_BASE+0x0)     /* Clock Enable Register */
+#define CLK_SEL           (WPCM_CLK_VA_BASE+0x4)     /* Clock Select Register */
+#define CLK_DIV           (WPCM_CLK_VA_BASE+0x8)     /* Clock Divider Control Register */
+#define CLK_PLLCON0       (WPCM_CLK_VA_BASE+0xC)     /* PLL Control Register 0  */
+#define CLK_PLLCON1       (WPCM_CLK_VA_BASE+0x10)    /* PLL Control Register 1 */
+#define CLK_PMCON         (WPCM_CLK_VA_BASE+0x14)    /* Power Management Control Register */
+#define CLK_IRQWAKECON    (WPCM_CLK_VA_BASE+0x18)    /* IRQ Wake-Up Control Register */
+#define CLK_IRQWAKEFLAG   (WPCM_CLK_VA_BASE+0x1C)    /* IRQ Wake-Up Flag Register */
+#define CLK_IPSRST	  (WPCM_CLK_VA_BASE+0x20)    /* IP Software Reset Register */
+
+/* -----------------------------------------------------------------
+ * 				 Memory Control Registers
+ * -----------------------------------------------------------------
+ */
+#define MC_CFG0           (WPCM_MC_VA_BASE+0x0)      /* Memory control configuration register 0 )*/
+#define MC_CFG1           (WPCM_MC_VA_BASE+0x4)      /* Memory control configuration register 1 )*/
+#define MC_CFG2           (WPCM_MC_VA_BASE+0x8)      /* Memory control configuration register 2 )*/
+#define MC_CFG3           (WPCM_MC_VA_BASE+0xC)      /* Memory control configuration register 3 )*/
+#define MC_CFG4           (WPCM_MC_VA_BASE+0x10)     /* Memory control configuration register 4 )*/
+#define MC_CFG5           (WPCM_MC_VA_BASE+0x14)     /* Memory control configuration register 5 )*/
+#define MC_CFG6           (WPCM_MC_VA_BASE+0x18)     /* Memory control configuration register 5 )*/
+#define MC_CFG7           (WPCM_MC_VA_BASE+0x1C)     /* Memory control configuration register 7 )*/
+#define MC_P1_ARBT        (WPCM_MC_VA_BASE+0x24)     /* Port 1 arbitration timer value )*/
+#define MC_P1_CNT         (WPCM_MC_VA_BASE+0x20)     /* Port 1 arbitration control register )*/
+#define MC_P2_ARBT        (WPCM_MC_VA_BASE+0x2C)     /* Port 2 arbitration timer value )*/
+#define MC_P2_CNT         (WPCM_MC_VA_BASE+0x28)     /* Port 2 arbitration control register )*/
+#define MC_P3_ARBT        (WPCM_MC_VA_BASE+0x34)     /* Port 3 arbitration timer value )*/
+#define MC_P3_CNT         (WPCM_MC_VA_BASE+0x30)     /* Port 3 arbitration control register )*/
+#define MC_P4_ARBT        (WPCM_MC_VA_BASE+0x3C)     /* Port 4 arbitration timer value )*/
+#define MC_P4_CNT         (WPCM_MC_VA_BASE+0x38)     /* Port 4 arbitration control register )*/
+#define MC_P5_ARBT        (WPCM_MC_VA_BASE+0x44)     /* Port 4 arbitration timer value )*/
+#define MC_P5_CNT         (WPCM_MC_VA_BASE+0x40)     /* Port 4 arbitration control register )*/
+#define MC_P6_ARBT        (WPCM_MC_VA_BASE+0x4C)     /* Port 4 arbitration timer value )*/
+#define MC_P6_CNT         (WPCM_MC_VA_BASE+0x48)     /* Port 4 arbitration control register )*/
+#define MC_P1_INCRS       (WPCM_MC_VA_BASE+0x50)     /* Port 1 INCR size control )*/
+#define MC_P2_INCRS       (WPCM_MC_VA_BASE+0x54)     /* Port 2 INCR size control )*/
+#define MC_P3_INCRS       (WPCM_MC_VA_BASE+0x58)     /* Port 3 INCR size control )*/
+#define MC_P4_INCRS       (WPCM_MC_VA_BASE+0x5C)     /* Port 4 INCR size control ) */
+#define MC_DLL_0	  (WPCM_MC_VA_BASE+0x60)     /* DLL and ODT Configuration Register 0 */
+#define MC_DLL_1	  (WPCM_MC_VA_BASE+0x64)     /* PDLL and ODT Configuration Register 1 */
+
+
+#endif
