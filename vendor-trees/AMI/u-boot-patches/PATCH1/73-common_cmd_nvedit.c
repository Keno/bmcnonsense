--- uboot/common/cmd_nvedit.c	2007-11-19 09:37:58.000000000 -0500
+++ uboot.setenv/common/cmd_nvedit.c	2007-11-19 09:40:12.000000000 -0500
@@ -240,19 +240,22 @@
 					baudrate);
 				return 1;
 			}
-			printf ("## Switch baudrate to %d bps and press ENTER ...\n",
-				baudrate);
-			udelay(50000);
-			gd->baudrate = baudrate;
-#ifdef CONFIG_PPC
-			gd->bd->bi_baudrate = baudrate;
-#endif
-
-			serial_setbrg ();
-			udelay(50000);
-			for (;;) {
-				if (getc() == '\r')
-				      break;
+			if (gd->baudrate != baudrate)
+			{
+				printf ("## Switch baudrate to %d bps and press ENTER ...\n",
+					baudrate);
+				udelay(50000);
+				gd->baudrate = baudrate;
+	#ifdef CONFIG_PPC
+				gd->bd->bi_baudrate = baudrate;
+	#endif
+
+				serial_setbrg ();
+				udelay(50000);
+				for (;;) {
+					if (getc() == '\r')
+					      break;
+				}
 			}
 		}
 
