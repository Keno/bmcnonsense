--- u-boot-1.1.4-cpu/cpu/arm926ejs/start.S	2007-06-27 14:52:10.000000000 -0400
+++ u-boot-1.1.4-wpcm/cpu/arm926ejs/start.S	2007-06-27 18:09:36.000000000 -0400
@@ -51,6 +51,9 @@
 
 .globl _start
 _start:
+#if defined(CONFIG_WPCM450)
+	.word 0xAA550450
+#endif
 	b	reset
 	ldr	pc, _undefined_instruction
 	ldr	pc, _software_interrupt
@@ -143,6 +146,14 @@
 	bl	cpu_init_crit
 #endif
 
+#if defined(CONFIG_WPCM450)
+/* Reset the processor mode, because lowlevel_init mucked with it */
+	mrs	r0,cpsr
+	bic	r0,r0,#0x1f
+	orr	r0,r0,#0xd3
+	msr	cpsr,r0
+#endif
+
 #ifndef CONFIG_SKIP_RELOCATE_UBOOT
 relocate:				/* relocate U-Boot to RAM	    */
 	adr	r0, _start		/* r0 <- current position of code   */
@@ -393,7 +404,7 @@
 
 #endif
 
-# ifdef CONFIG_INTEGRATOR
+# if defined(CONFIG_INTEGRATOR) || defined(CONFIG_WPCM450)
 
 	/* Satisfied by Integrator routine (AP or CP) */
 
