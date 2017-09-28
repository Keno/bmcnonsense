--- uboot/net/net.c	2007-05-23 14:34:01.000000000 -0400
+++ uboot.fix/net/net.c	2007-05-23 14:26:41.000000000 -0400
@@ -575,6 +575,7 @@
 			return NetBootFileXferSize;
 
 		case NETLOOP_FAIL:
+			eth_halt();		/* AMI : Bug Fix */
 			return (-1);
 		}
 	}
