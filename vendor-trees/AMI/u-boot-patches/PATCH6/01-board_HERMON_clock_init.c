--- uboot-old/board/HERMON/clock_init.c	1969-12-31 17:00:00.000000000 -0700
+++ uboot-new/board/HERMON/clock_init.c	2008-08-02 18:44:27.000000000 -0500
@@ -0,0 +1,278 @@
+/******************************************************************************
+ *
+ * Copyright (c) 2003 Windond Electronics Corp.
+ * All rights reserved.
+ *
+ * $Workfile: wbl_info.c $
+ *
+ * $Author: achechik $
+ ******************************************************************************/
+
+#include "wpcm450_hw.h"
+#include "macros.h"
+#include "clocks.h"
+
+
+
+
+
+/************************************************************************/
+/*  GLOBAL VARIABLES                                                    */
+/************************************************************************/
+/*
+const UINT32 CLK_DIV_REG_Val[] = {
+  CLK_180MHZ_DIV_REG_CFG,
+  CLK_133MHZ_DIV_REG_CFG,
+  CLK_250MHZ_DIV_REG_CFG,
+  CLK_160MHZ_DIV_REG_CFG,
+  CLK_220MHZ_DIV_REG_CFG,
+  CLK_125MHZ_DIV_REG_CFG,
+  CLK_200MHZ_DIV_REG_CFG
+};
+
+const UINT32 CLK_PLLCON_REG_Val[] = {
+  CLK_180MHZ_PLLCON_REG_CFG,
+  CLK_133MHZ_PLLCON_REG_CFG,
+  CLK_250MHZ_PLLCON_REG_CFG,
+  CLK_160MHZ_PLLCON_REG_CFG,
+  CLK_220MHZ_PLLCON_REG_CFG,
+  CLK_125MHZ_PLLCON_REG_CFG,
+  CLK_200MHZ_PLLCON_REG_CFG
+};
+*/
+
+
+/************************************************************************/
+/* EXTERNAL VARIABLES                                                   */
+/************************************************************************/
+
+
+
+/*****************************************************************************
+ *  Name : WBL_ClockInit
+ *
+ *  Description:
+ *
+ *  Params :
+ *       ClkScheme_t cfg_scheme
+ *
+ *  Return : none
+ *
+ *****************************************************************************/
+void WBL_ClockInit (void)
+{
+  
+//#ifdef __TARGET_CPU_ARM926EJ_S  
+
+  UINT32 pwron_cfg = *(volatile unsigned long *)GCR_PWRON;
+  UINT32 idx1;
+  UINT32 idx2;
+  volatile UINT32 temp ;
+//	UINT32 PLLCON180 = 0x002C2005;
+
+  /************************************************************************/
+  /* If watchdog reset was done. Don't do any clock init                  */
+  /************************************************************************/
+  if(IS_BIT_SET(*(volatile unsigned long*)TIMER_WTCR, WTCR_RESET_FLAG_BIT))
+  {
+    return;
+  }
+
+  /************************************************************************/
+  /*  Till PLL's not configured all clock's sources will from             */
+  /*  RMII reference clock. It's default value(0x0000022A). But will done */
+  /*  for more .......                                                    */
+  /************************************************************************/
+
+
+  /************************************************************************/
+  /*  Read STRAP clock configuration                                      */
+  /************************************************************************/
+  idx1 = GET_FIELD(pwron_cfg, PWRON_CPU_CORE_CLK);
+
+  if(idx1 != CLK_BYPASS_MODE)
+  {
+    idx1 -= 1; /* Clock configuration arrays not contain BY_PASS mode configuration values  */
+
+    SET_BIT(*(volatile unsigned long*)CLK_PLLCON0,CLK_PRST_BIT);
+      
+// Cannot use constant array when running in pc relative mode      
+//    CLK_PLLCON0 = CLK_PLLCON_REG_Val[idx1];
+// Must force the compiler to use immediate addressing
+    	if (idx1 == 0) {
+		// 180
+		temp = 0x05 ;
+		temp |= 0x20 << 8;
+		temp |= 0x2c << 16 ;
+    		*(volatile unsigned long*)CLK_PLLCON0 = temp;
+    		//CLK_PLLCON0 = 0x002C2005;
+	}
+    	if (idx1 == 1) {
+		// 133
+		temp = 0x08 ;
+		temp |= 0x21 << 8 ;
+		temp |= 0x63 << 16 ;
+    		*(volatile unsigned long*)CLK_PLLCON0 = temp;
+    		//CLK_PLLCON0 = 0x00632108;
+	}
+    	if (idx1 == 2) {
+		// 250
+		temp = 0x0B ;
+		temp |= 0x20 << 8 ;
+		temp |= 0x7C << 16 ;
+    		*(volatile unsigned long*)CLK_PLLCON0 = temp;
+    		//CLK_PLLCON0 = 0x007C200B;
+	}
+    	if (idx1 == 3) {
+		// 160
+		temp = 0x05 ;
+		temp |= 0x20 << 8 ;
+		temp |= 0x27 << 16 ;
+    		*(volatile unsigned long*)CLK_PLLCON0 = temp ;
+    		//CLK_PLLCON0 = 0x00272005 ;
+	}
+    	if (idx1 == 4) {
+		// 220
+		temp = 0x05 ;
+		temp |= 0x20 << 8 ;
+
+		// For All chips less than A2, set the CPU to 216 MHZ (PLL0 to 432 MHZ). 
+		// workaround for silicon bug, which will be fixed in A2.
+		if((*(UINT8*)(GCR_PDID + 0x03)) <= 8 )  
+		{
+			temp |= 0x35 << 16 ;		 
+		}else
+		{
+			temp |= 0x36 << 16 ;		 
+		}
+    		*(volatile unsigned long*)CLK_PLLCON0 =  temp;
+    		//CLK_PLLCON0 =  0x00362005;
+	}
+    	if (idx1 == 5) {
+		// 125
+		temp = 0x0B ;
+		temp |= 0x21 << 8 ;
+		temp |= 0x7C << 16 ;
+    		*(volatile unsigned long*)CLK_PLLCON0 =  temp;
+    		//CLK_PLLCON0 =  0x007C210B;
+	}
+    	if (idx1 == 6) {
+		// 200
+		temp = 0x05 ;
+		temp |= 0x20 << 8 ;
+		temp |= 0x31 << 16 ;
+    		*(volatile unsigned long*)CLK_PLLCON0 = temp;
+    		//CLK_PLLCON0 = 0x00312005;
+	}
+      
+    /* Wait Loop */
+    CLK_DELAY_10_MICRO_SEC(idx2);
+        
+    CLEAR_BIT(*(volatile unsigned long*)CLK_PLLCON0,CLK_PRST_BIT);
+  
+    /* Wait Loop */
+    CLK_DELAY_500_MICRO_SEC(idx2);
+
+    /************************************************************************/
+    /* Set clock divider accordingly setted PLL configuration               */
+    /************************************************************************/
+// Can't use const array in pc relative mode
+//    CLK_DIV     = CLK_DIV_REG_Val[idx1];
+	if (idx1 == 0) {
+		// 180
+			temp = 0x00 ;
+			temp |= 0x03 << 8 ;
+			temp |= 0x01 << 16 ;
+			temp |= 0x05 << 24 ;
+		    *(volatile unsigned long*)CLK_DIV = temp;
+		    //CLK_DIV = 0x05010300;
+	}
+	if (idx1 == 1) {
+			temp = 0 ;
+			temp |= 0x02 << 8 ;
+			temp |= 0x01 << 16 ;
+			temp |= 0x05 << 24 ;
+		    *(volatile unsigned long*)CLK_DIV = temp;
+		    //CLK_DIV =  0x05010200;
+	}
+	if (idx1 == 2) {
+		// 250
+			temp = 0x00 ;
+			temp |= 0x03 << 8 ;
+			temp |= 0x01 << 16 ;
+			temp |= 0x05 << 24 ;
+		    *(volatile unsigned long*)CLK_DIV = temp;
+		    //CLK_DIV =  0x05010300;
+	}
+	if (idx1 == 3) {
+		// 160
+			temp = 0x00 ;
+			temp |= 0x02 << 8 ;
+			temp |= 0x01 << 16 ;
+			temp |= 0x05 << 24 ;
+		    *(volatile unsigned long*)CLK_DIV = temp;
+		    //CLK_DIV =  0x05010200;
+	}
+	if (idx1 == 4) {
+		// 220
+			temp = 0x00 ;
+			temp |= 0x03 << 8 ;
+			temp |= 0x01 << 16 ;
+			temp |= 0x05 << 24 ;
+		    *(volatile unsigned long*)CLK_DIV = temp;
+		    //CLK_DIV = 0x05010300;
+	}
+	if (idx1 == 5) {
+		// 125
+			temp = 0x00 ;
+			temp |= 0x03 << 8 ;
+			temp |= 0x01 << 16 ;
+			temp |= 0x04 << 24 ;
+		    *(volatile unsigned long*)CLK_DIV = temp;
+		    //CLK_DIV =  0x04010300;
+	}
+	if (idx1 == 6) {
+		// 200
+			temp = 0x00 ;
+			temp |= 0x03 << 8 ;
+			temp |= 0x01 << 16 ;
+			temp |= 0x05 << 24 ;
+		    *(volatile unsigned long*)CLK_DIV = temp;
+		    //CLK_DIV = 0x05010300;
+	}
+
+
+    /* Wait Loop 200 cycles */
+    CLK_DELAY_10_MICRO_SEC(idx2);
+        
+    /************************************************************************/
+    /* Set clock sources selection to PLLCON0                               */
+    /************************************************************************/
+    SET_FIELD(*(volatile unsigned long*)CLK_SEL,CLK_CPU_CLK, CLK_PLL0_SRC_SEL );
+    SET_FIELD(*(volatile unsigned long*)CLK_SEL,CLK_CPU_OUT, CLK_PLL0_SRC_SEL );
+    SET_FIELD(*(volatile unsigned long*)CLK_SEL,CLK_PIX_SRC, CLK_PIX_PLL_GFX_SEL );
+  }
+  else
+  {
+    /************************************************************************/
+    /* By pass mode                                                         */
+    /* Bypass mode: Core clock from reference clock PWM3/GPIO83 pin as      */
+    /* input and pixel clock from GPI96. Reference clock 24 MHz and the SPI */
+    /* clock is 3 MHz by default register values.                           */
+    /************************************************************************/
+      
+    SET_BIT(*(volatile unsigned long *)GCR_MFSEL2, MF_PWM3SEL_BIT);
+    SET_FIELD(*(volatile unsigned long*)CLK_SEL,CLK_CPU_CLK, CLK_PWM3_GPIO83_SRC_SEL);
+    CLEAR_BIT(*(volatile unsigned long *)GCR_MFSEL1, MF_CLKOSEL_BIT);
+    SET_FIELD(*(volatile unsigned long*)CLK_SEL,CLK_PIX_SRC, CLK_PIX_GPI96_SEL );
+  }
+
+  SET_FIELD(*(volatile unsigned long*)CLK_SEL,CLK_USB1CKSEL, CLK_PLL1_SRC_SEL );
+  
+//#endif /* __TARGET_CPU_ARM926EJ_S  */
+  
+}
+
+
+
+
