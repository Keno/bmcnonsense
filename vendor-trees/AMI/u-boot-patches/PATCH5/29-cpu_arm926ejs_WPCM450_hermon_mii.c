--- uboot-pristine/cpu/arm926ejs/WPCM450/hermon_mii.c	2009-05-14 10:29:19.000000000 -0700
+++ uboot-changed/cpu/arm926ejs/WPCM450/hermon_mii.c	2009-05-14 10:30:26.000000000 -0700
@@ -145,6 +145,7 @@
 GetLinkStatus(char *devname)
 {
 	u16 value =0;
+
 	unsigned char phyAddr = GetConfigPhyAddr(devname);
 	wpcm_miiphy_read(devname, phyAddr, PHY_BMSR, &value);  
 	wpcm_miiphy_read(devname, phyAddr, PHY_BMSR, &value);	
@@ -158,8 +159,11 @@
 	if ((value & (PHY_AN_COMPLETE | PHY_LINK_STATUS_VALID)) == 
 							(PHY_AN_COMPLETE | PHY_LINK_STATUS_VALID))
 	{
+		printf("Links status ok, value=%x phyaddr=%x\n", value, phyAddr);
 		return 1;
 	}
+	printf("Links status not ok, value=%x phyaddr=%x\n", value, phyAddr);
+
 	return 0;
 
 }
@@ -168,21 +172,29 @@
 ResetAutoNeg(char *devname)
 {
 	u16 value = 0;
-	u32 count =0;
+	u32 count =0;
+	u32 retry=0;
 	unsigned char phyAddr = GetConfigPhyAddr(devname);
 
 	wpcm_miiphy_read(devname, phyAddr, PHY_BMCR, &value);
 	wpcm_miiphy_write(devname, phyAddr, PHY_BMCR, (value| PHY_BMCR_AUTON | PHY_BMCR_RST_NEG) );
  	
-	count = 100000;
+	count = 100000;
+	retry = 5;
  	while (count) 	/* wait for auto-negotiation complete */
    	{
 		wpcm_miiphy_read(devname, phyAddr, PHY_BMSR, &value);
 
     	if((value & PHY_REMOTE_FAULT)!=0)
     	{
-			  printf(" ERROR: PHY Remote Fault\n");
-    		  return -1;
+			wpcm_miiphy_read(devname, phyAddr, PHY_BMCR, &value);
+			wpcm_miiphy_write(devname, phyAddr, PHY_BMCR, (value| PHY_BMCR_AUTON | PHY_BMCR_RST_NEG) );
+ 	
+			printf(" ERROR: PHY Remote Fault\n");
+			retry--;
+			if (retry==0)
+    			return -1;
+			continue;
     	}
 
     	if ((value & (PHY_AN_COMPLETE | PHY_LINK_STATUS_VALID)) == 
@@ -272,7 +284,9 @@
 void DetectPhyAddr(char *devname)
 {
    int num = -1, count = 0;
-   unsigned short value =0;
+   unsigned short value =0;
+   unsigned char retry;
+
    if (devname == NULL)
    {
       return;
@@ -284,17 +298,21 @@
    }
 
    for(count = 0; count < 32; ++count)
-   {
+   {
  		wpcm_miiphy_write(devname, count, MII_PHY_CR, MII_PHY_CR_RESET);
 		if (wpcm_miiphy_read(devname, count, MII_PHY_CR, &value) != -1)
       {    
 		   if ( (value != 0xFFFF) && (value & 0x3000) )
          {
             configPhyAddr[num] = count;
-            printf("PHY for device %s detected @ Addr 0x%x \n", devname, count);
-			// Reset Auto Negotiation 
-			ResetAutoNeg(devname);   // joe unmarked
-            return;
+            printf("PHY for device %s detected @ Addr 0x%x \n", devname, count);
+
+			for (retry=0; retry<2; retry++)
+			{
+				// Reset Auto Negotiation 
+				if (ResetAutoNeg(devname)==0)	   // joe unmarked
+					return;
+			}
          }
 			
       }
