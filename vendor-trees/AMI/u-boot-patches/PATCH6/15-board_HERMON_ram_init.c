--- uboot-pristine/board/HERMON/ram_init.c	2009-05-14 10:29:19.000000000 -0700
+++ uboot-changed/board/HERMON/ram_init.c	2009-05-14 10:30:26.000000000 -0700
@@ -498,10 +498,10 @@
 			while(1) ;
 		}
 
-		//MC_CFG5 = 0x190202  ; 	// MC_CFG5
+		//MC_CFG5 = 0x180202  ; 	// MC_CFG5
 			temp = 0x02 ;
 			temp |= 0x02 << 8 ;
-			temp |= 0x19 << 16 ;
+			temp |= 0x18 << 16 ;   // [Farida] orig. 0x19
 			*(volatile unsigned long *)MC_CFG5 = temp ;
 		*(volatile unsigned long *)MC_CFG6 = 0x0 ;	// MC_CFG6
 // NOT USED!!    0x1,	// CFG7
