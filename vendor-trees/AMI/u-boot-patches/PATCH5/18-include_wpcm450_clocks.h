--- u-boot/include/wpcm450/clocks.h	1969-12-31 17:00:00.000000000 -0700
+++ u-boot-new/include/wpcm450/clocks.h	2008-06-04 17:49:34.000000000 -0500
@@ -0,0 +1,103 @@
+/******************************************************************************
+ *
+ * Copyright (c) 2003 Windond Electronics Corp.
+ * All rights reserved.
+ *
+ * $Workfile: clock_init.h $
+ *
+ * $Author: achechik $
+ ******************************************************************************/
+
+
+#ifndef _CLOCK_INIT_H_
+#define _CLOCK_INIT_H_
+
+/************************************************************************/
+/*  MACROS & DEFINITIONS                                                */
+/************************************************************************/
+
+
+/*
+* Supported Clock Schemes
+*---------------------------------------------------------------------------------------------------------------
+*  Scheme     | CPU       | DDR-II Clock,  | AHB1 and AHB2  |  AHB3                |  APB              | USB
+*                         | AHB4, AHB5     |                |                      |                   |
+*---------------------------------------------------------------------------------------------------------------
+*  Preferred  |  220 MHz  |    220 MHz     |   110 MHz      | 55 or 27.5 MHz       |   55 MHz          | 48 MHz
+*  High SPI   |           |                |                |                      |                   |
+*  Clock      |  200 MHz  |    200 MHz     |   100 MHz      | 50 MHz               |   50 MHz          | 48 MHz
+*  Fallback 1 |  167 MHz  |    167 MHz     |   83.3 MHz     | 41.7 MHz             |   41.7 MHz        | 48 MHz
+*  Fallback 2 |  133.3 MHz|    133.3 MHz   |   66.7 MHz     | 66.7 MHz or 33.3 MHz | 66.7 or 33.3 MHz  | 48 MHz
+*  Minimal    |  125 MHz  |    125 MHz     |   125  MHz     | 15.6 MHz             | 15.6 MHz          | 48 MHz
+*---------------------------------------------------------------------------------------------------------------
+*
+*  Supported PLL configuration values
+*-----------------------------------------------------------------------------------------------------------------
+*  PLL Output Frequency | 500 MHz    | 440 MHz    | 400 MHz    | 360 MHz    | 320 MHz    | 266.7 MHz  | 250 MHz
+*-----------------------------------------------------------------------------------------------------------------
+*  PLLCON Register      | 0x007C200B | 0x00362005 | 0x00312005 | 0x002C2005 | 0x00272005 | 0x00632108 | 0x007C210B 
+*-----------------------------------------------------------------------------------------------------------------
+*  Used for:            | 250 MHz    | 220 MHz    | 200 MHz    | 180 MHz    | 160 MHz    | 133.3 MHz  | 125 MHz 
+*                       | CPU clock  | CPU clock  | CPU clock  | CPU clock  | CPU clock  | CPU clock  | CPU clock
+*-----------------------------------------------------------------------------------------------------------------
+*/
+#define CLK_125MHZ_DIV_REG_CFG     0x04010300
+#define CLK_133MHZ_DIV_REG_CFG     0x05010200
+#define CLK_160MHZ_DIV_REG_CFG     0x05010200
+#define CLK_180MHZ_DIV_REG_CFG     0x05010300
+#define CLK_200MHZ_DIV_REG_CFG     0x05010300
+#define CLK_220MHZ_DIV_REG_CFG     0x05010300
+#define CLK_250MHZ_DIV_REG_CFG     0x05010300
+#define CLK_BYPASS_DIV_REG_CFG     0x05010300
+
+#define CLK_125MHZ_PLLCON_REG_CFG  0x007C210B
+#define CLK_133MHZ_PLLCON_REG_CFG  0x00632108
+#define CLK_160MHZ_PLLCON_REG_CFG  0x00272005
+#define CLK_180MHZ_PLLCON_REG_CFG  0x002C2005
+#define CLK_200MHZ_PLLCON_REG_CFG  0x00312005
+#define CLK_220MHZ_PLLCON_REG_CFG  0x00362005
+#define CLK_250MHZ_PLLCON_REG_CFG  0x007C200B
+
+/* Clock Source selection */
+#define CLK_48M_SRC_SEL            0x02
+#define CLK_PLL0_SRC_SEL           0x00
+#define CLK_PLL1_SRC_SEL           0x01
+#define CLK_PWM3_GPIO83_SRC_SEL    0x03
+
+/* Host UART clock selection */
+#define CLK_HUART_48M_SRC_SEL		0x0
+#define CLK_HUART_24M_SRC_SEL		0x1
+
+
+/************************************************************************/
+/* Pixel Clock Source Select Bits                                       */
+/************************************************************************/
+#define CLK_PIX_PLL_GFX_SEL        0x00
+#define CLK_PIX_GPI96_SEL          0x01
+#define CLK_PIX_48M_SEL            0x02
+
+#define CLK_DELAY_10_MICRO_SEC(cnt)  {for(cnt=0;cnt<20;){cnt++;}}
+#define CLK_DELAY_500_MICRO_SEC(cnt)  {for(cnt=0;cnt<1000;){cnt++;}}
+
+/************************************************************************/
+/* TYPES DEFINITIONS                                                    */
+/************************************************************************/
+
+typedef enum {
+  CLK_BYPASS_MODE = 0x0,          /* Bypass mode: Core clock from reference clock and pixel clock from PWM3/GPIO83 pin as input. */
+  /* Reference clock divided by 2 / Reference divided by 2 / Reference divided by 16             */
+  CLK_180MHz_MODE = 0x1,          /*  180 / 90 / 11.25 MHz.   */
+  CLK_133MHz_MODE = 0x2,          /* 133.3 / 66.7 / 16.7 MHz. */
+  CLK_250MHz_MODE = 0x3,          /* 250 / 125 / 15.6 MHz.    */
+  CLK_160MHz_MODE = 0x4,          /* 160 / 80 / 20 MHz.       */
+  CLK_220MHz_MODE = 0x5,          /* 220 / 110 / 13.75 MHz.   */
+  CLK_125MHz_MODE = 0x6,          /* 125 / 125 / 15.6 MHz.    */
+  CLK_200MHz_MODE = 0x7           /* 200 / 100 / 12.5 MHz.    */
+}PowerOnClkFreqTypes_t; 
+
+
+#endif 
+
+
+
+
