--- linux-pristine/drivers/net/arm/wpcm450_mii.c	2009-05-14 14:10:28.000000000 -0700
+++ linux-changed/drivers/net/arm/wpcm450_mii.c	2009-05-14 13:41:09.000000000 -0700
@@ -253,10 +253,12 @@
 {
  	volatile int i = 1000;
  	int which=num;
-    volatile long loop=1000*100;
+        volatile long loop=1000*100;
+	unsigned long MyMDCCR=0x00f00000;	// HCLK/60  [Joe]
 
    	wpcm_WriteReg(MIID,PhyWrData,which);
-   	wpcm_WriteReg(MIIDA,PhyInAddr|PhyAddr|PHYBUSY|PHYWR|MDCCR,which);
+//   	wpcm_WriteReg(MIIDA,PhyInAddr|PhyAddr|PHYBUSY|PHYWR|MDCCR,which);
+    wpcm_WriteReg(MIIDA, PhyInAddr | PhyAddr | PHYBUSY | MyMDCCR,which);   //[Joe]
    	while(i--);
    	while((wpcm_ReadReg(MIIDA,which) &PHYBUSY))
    	{
@@ -279,8 +281,10 @@
  	int which=num;
  	volatile unsigned long loop=1000*100;
  	unsigned long PhyRdData ;
+	unsigned long MyMDCCR=0x00f00000;	// HCLK/60  [Joe]
 
-    wpcm_WriteReg(MIIDA, PhyInAddr | PhyAddr | PHYBUSY | MDCCR,which);
+//    wpcm_WriteReg(MIIDA, PhyInAddr | PhyAddr | PHYBUSY | MDCCR,which);
+    wpcm_WriteReg(MIIDA, PhyInAddr | PhyAddr | PHYBUSY | MyMDCCR,which);  //[Joe]
    	while(i--);
     while( (wpcm_ReadReg(MIIDA,which)& PHYBUSY) ) 
     {
