--- uboot-pristine/cpu/arm926ejs/WPCM450/misc.c	2009-05-14 10:29:19.000000000 -0700
+++ uboot-changed/cpu/arm926ejs/WPCM450/misc.c	2009-05-14 10:30:26.000000000 -0700
@@ -4,16 +4,25 @@
 #include "macros.h"
 #include "soc_hw.h"
 
+#define	SMC_CTL	(WPCM_SHM_VA_BASE + 0x01)
 void
 soc_init(void)
 {
 
 	/* Do any WPCM450 SOC Initialization here */
 	// joe --- begin 
+	// set GPIO93 and GPIO94 instead of KBCRST
+	SET_BIT(*(volatile unsigned long *)GCR_MFSEL1, 17);
+
 	// hw require this bit to 0 to select GPIO81 because hw desgin this pin as GPIO
 	// and must not affect ICH9
 	CLEAR_BIT(*(volatile unsigned long *)GCR_MFSEL2, 17);	
+
+	// clear the HOSTWAIT to let host access the bus
+	if ((*(volatile unsigned char *)SMC_CTL) & 0x80)
+		SET_BIT(*(volatile unsigned char *)SMC_CTL, 7);	
 	// joe --- end
+
 	/* Change CPU freq to max if possible */
 
 	return;
