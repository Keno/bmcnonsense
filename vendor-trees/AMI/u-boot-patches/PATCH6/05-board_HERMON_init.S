--- u-boot-1.1.4-wpcm/board/HERMON/init.S	1969-12-31 19:00:00.000000000 -0500
+++ u-boot-1.1.4-good/board/HERMON/init.S	2007-06-28 00:38:36.000000000 -0400
@@ -0,0 +1,51 @@
+/***************************************************************************
+#
+# Copyright (c) American Megatrends Inc 2007
+# All rights reserved
+#
+#       Project  : Hermon
+#       Filename : init.s
+#
+# Modified for U-Boot by American Megatrends Inc.
+# Set TAB Spaces to 4 for proper view of the source
+#      
+#****************************************************************************
+*/
+
+Mode_SYS32		=	0x1f
+SYSTEMMODE		=       Mode_SYS32
+	.global hermon_init
+	.align   
+
+/*
+#************************************************************************
+#              Called from lowlevelinit.s to initialize DDR
+#************************************************************************
+*/
+hermon_init:
+		mov		r11, lr			   	   @ save link register so we can return
+
+@ UBoot disables interrupts before calling this
+@ Need IRQ enabled to handle clock switching interrupt
+        ldr     r2,= SYSTEMMODE        @User32Mode
+        MSR     CPSR, r2
+
+		ldr	 	r13,=0x00001FFC        @temp stack in local memory
+		ldr		r0, =0x00000000        @ save r12 (ip) temporarily in local ram
+		str		r12, [r0]
+
+@ Do CPU Clock Switching
+		bl WBL_ClockInit
+
+@ Do DDR Initializtaion
+		bl WBL_RamInit
+
+		ldr	r0, =0x00000000   	@ restore r12 (ip) from local ram
+		ldr	r12, [r0]
+
+		mov	lr, r11
+		mov	pc, lr
+	
+	    b    .
+    .ltorg 
+	.end     
