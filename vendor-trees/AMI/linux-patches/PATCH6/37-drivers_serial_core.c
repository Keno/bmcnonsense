--- linux/drivers/serial/serial_core.c	2008-01-24 16:58:37.000000000 -0600
+++ linux.good/drivers/serial/serial_core.c	2008-06-03 18:11:33.000000000 -0500
@@ -400,6 +400,9 @@
 {
 	unsigned int quot;
 
+#ifdef CONFIG_WPCM450_SERIAL	
+	quot = (port->uartclk  / (16 * baud))-2;
+#else
 	/*
 	 * Old custom speed handling.
 	 */
@@ -407,6 +410,7 @@
 		quot = port->custom_divisor;
 	else
 		quot = (port->uartclk + (8 * baud)) / (16 * baud);
+#endif
 
 	return quot;
 }
