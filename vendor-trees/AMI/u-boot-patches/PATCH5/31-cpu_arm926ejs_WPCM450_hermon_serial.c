--- uboot-pristine/cpu/arm926ejs/WPCM450/hermon_serial.c	2009-05-14 10:29:19.000000000 -0700
+++ uboot-changed/cpu/arm926ejs/WPCM450/hermon_serial.c	2009-05-14 10:30:26.000000000 -0700
@@ -21,11 +21,14 @@
   /* Select 48M clock source for UART in case that real chip */
   SET_FIELD(*(volatile unsigned long *)CLK_SEL, CLK_UART_SRC, CLK_48M_SRC_SEL);
   
+  /* Serial Interface pins 1 selected. For X8DTT-IBX & X8DTL only */
+  SET_BIT(*(volatile unsigned long *)GCR_MFSEL1, MF_HSP1SEL_BIT);
+
   /* Serial Interface pins 2 selected. */
   SET_BIT(*(volatile unsigned long *)GCR_MFSEL1, MF_HSP2SEL_BIT);
 
   /* Serial Port Mode 2 selected. (Core Take-ove) */
-  SET_FIELD( *(volatile unsigned long *)GCR_SPSWC, SPSWC_SPMOD, SPSWC_SPMOD_3);	
+  SET_FIELD( *(volatile unsigned long *)GCR_SPSWC, SPSWC_SPMOD, SPSWC_SPMOD_3);	 
 
   return;
 }
