--- uboot-pristine/cpu/arm926ejs/WPCM450/hermon_mii_sg007.c	1969-12-31 16:00:00.000000000 -0800
+++ uboot-changed/cpu/arm926ejs/WPCM450/hermon_mii_sg007.c	2009-05-14 10:30:26.000000000 -0700
@@ -0,0 +1,336 @@
+#include <common.h>
+#include <asm/processor.h>
+#include <miiphy.h>
+#include "soc_hw.h"
+#include "hermon_mac.h"
+
+#define inl(addr) 		(*((volatile u32 *)(addr)))
+#define outl(value, addr)  	(*((volatile u32 *)(addr)) = value)
+
+// to store Phy Addr - Init to default values (in case not able to detect in scan)
+static unsigned char configPhyAddr[MAX_WPCM_ETH] = {CONFIG_PHY_ADDR_WPCM_ETH_1, CONFIG_PHY_ADDR_WPCM_ETH_1};
+
+// static [SMC] marked
+int
+GetDevNum(char *devname)
+{
+	int num = -1;
+
+	/* Assumption: single digits 0 to 9 only and occurs at last char */
+	while (*devname)
+	{
+		num= *devname;
+		devname++;
+	}
+	if ((num < '0') || ( num  > '9'))
+		return -1;
+
+	/* Convert char to integer */
+	num-='0';
+
+	return num;
+}
+
+/* 
+ * GetConfigPhyAddr
+ * Used to get the phy addr based on the device name
+ *
+ * return phyAddr of wpcm_eth0 / wpcm_eth1
+ */
+unsigned char GetConfigPhyAddr(char *devname)
+{
+   int num = -1;
+   if (devname == NULL)
+   {  // return wpcm_eth1 config phy addr as default.
+      return CONFIG_PHY_ADDR_WPCM_ETH_1;
+   }
+   num = GetDevNum(devname);
+   if (num == -1 || num > MAX_WPCM_ETH)
+   {  // return wpcm_eth1 config phy addr as default.
+      return CONFIG_PHY_ADDR_WPCM_ETH_1;
+   }      
+   // return the desired eth phy address.
+   return configPhyAddr[num];   
+}
+
+static
+u32
+getmacaddr(char *devname)
+{
+	int num;
+
+	if (devname == NULL)
+		return 0;
+	num = GetDevNum(devname);
+	if (num == -1)
+		return 0;
+	switch (num)
+	{
+		case 1:
+			return WPCM_MAC2_BASE;
+		case 0:
+			return WPCM_MAC1_BASE;
+		default:
+			break;
+	}
+	return 0;
+}
+
+int
+wpcm_miiphy_read (char *devname, unsigned char addr, unsigned char reg,  unsigned short *value)
+{
+
+	u32 macaddr;
+	u32 temp ;
+	volatile u32 loop=1000*100 ;
+
+	macaddr = getmacaddr(devname);
+	if (macaddr == 0)
+		return -1;
+
+// SET GPIO TO ENABLE RMII 1 and 2  INTERFACES
+	*(volatile unsigned long *)GCR_MFSEL1 |= 0x0001F000;
+
+	temp = inl(macaddr + MCMDR); // make MDC is running
+	outl(temp | MCMDR_EnMDC,macaddr + MCMDR); 
+
+	outl((reg | PHYBUSY | MDCCR1 | (addr << 8)),macaddr + MIIDA);
+	while (inl(macaddr + MIIDA) & PHYBUSY) {
+		loop-- ;
+		if(loop==0) {
+			printf("Phy Read Timeout\n");
+			*value = 0 ;
+			return -1 ;
+		}
+	}
+	*value = inl(macaddr + MIID);
+//	printf("wpcm_miiphy_read called Reg = %x Data = %x\n",reg,*value);
+
+	return 0;
+}
+
+int
+wpcm_miiphy_write (char *devname, unsigned char addr, unsigned char reg, unsigned short value)
+{
+//	printf("wpcm_miiphy_write called\n");
+	u32 macaddr;
+	volatile u32 loop=1000*100 ;
+	u32 temp ;
+
+	macaddr = getmacaddr(devname);
+	if (macaddr == 0)
+		return -1;
+
+	// SET GPIO TO ENABLE RMII 1 and 2  INTERFACES
+	*(volatile unsigned long *)GCR_MFSEL1 |= 0x0001F000;
+
+	temp = inl(macaddr + MCMDR); // make MDC is running
+	outl(temp | MCMDR_EnMDC,macaddr + MCMDR); 
+
+	outl((u32)value,macaddr + MIID);
+	outl((reg|(addr << 8)|PHYBUSY|PHYWR|MDCCR1),macaddr + MIIDA);
+	while (inl(macaddr + MIIDA) & PHYBUSY) {
+		loop-- ;
+		if(loop==0) {
+			printf("Phy Write Timeout\n");
+			return -1 ;
+		}
+	}
+
+	return 0;
+
+}
+
+int
+GetLinkStatus(char *devname)
+{
+	u16 value =0;
+	unsigned char phyAddr = GetConfigPhyAddr(devname);
+	wpcm_miiphy_read(devname, phyAddr, PHY_BMSR, &value);  
+	wpcm_miiphy_read(devname, phyAddr, PHY_BMSR, &value);	
+	
+//printf("uboot LinkStatus=%x\n", value);
+	if((value & PHY_REMOTE_FAULT)!=0)
+	{
+		  printf(" ERROR: PHY Remote Fault\n");
+		  return 0;
+	}
+	
+#if 0
+	if ((value & (PHY_AN_COMPLETE | PHY_LINK_STATUS_VALID)) == 
+							(PHY_AN_COMPLETE | PHY_LINK_STATUS_VALID))
+	{
+		return 1;
+	}
+#endif
+	if ((value & PHY_LINK_STATUS_VALID) == PHY_LINK_STATUS_VALID)
+	{
+		return 1;
+	}
+
+	return 0;
+
+}
+
+int 
+ResetAutoNeg(char *devname)
+{
+	u16 value = 0;
+	u32 count =0;
+	unsigned char phyAddr = GetConfigPhyAddr(devname);
+
+	wpcm_miiphy_read(devname, phyAddr, PHY_BMCR, &value);
+	wpcm_miiphy_write(devname, phyAddr, PHY_BMCR, (value| PHY_BMCR_AUTON | PHY_BMCR_RST_NEG) );
+ 	
+	count = 100000;
+ 	while (count) 	/* wait for auto-negotiation complete */
+   	{
+		wpcm_miiphy_read(devname, phyAddr, PHY_BMSR, &value);
+
+    	if((value & PHY_REMOTE_FAULT)!=0)
+    	{
+			  printf(" ERROR: PHY Remote Fault\n");
+    		  return -1;
+    	}
+
+    	if ((value & (PHY_AN_COMPLETE | PHY_LINK_STATUS_VALID)) == 
+								(PHY_AN_COMPLETE | PHY_LINK_STATUS_VALID))
+    	{
+            break;
+      	}
+		count--;
+    	if(count==0)
+    	{
+    		printf("AutoNegotiation failed \n");
+      	 	return -1;
+    	}	 
+   	}
+	return 0;
+}
+
+
+int
+GetPhySpeed(char *devname)
+{
+	u16 value=0;
+	unsigned char phyAddr = GetConfigPhyAddr(devname);
+
+	wpcm_miiphy_read (devname, phyAddr, PHY_BMCR, &value);
+	
+	// Is auto negotiation on
+	if ((value & PHY_BMCR_AUTON) != 0) 
+	{
+		wpcm_miiphy_read (devname, phyAddr, PHY_ANLPAR, &value);
+		
+		if ((value & PHY_ANLPAR_100) != 0) 
+		{
+			return (_100BASET);
+		} else 
+		{
+			return (_10BASET);
+		}
+	}
+	else if (value & PHY_BMCR_100MB) 
+	{
+		return (_100BASET);
+	} else 
+	{
+		return (_10BASET);
+	}
+	
+}
+
+int
+GetPhyDuplex(char *devname)
+{
+	u16 value=0;
+	unsigned char phyAddr = GetConfigPhyAddr(devname);
+
+	wpcm_miiphy_read (devname, phyAddr, PHY_BMCR, &value);
+		
+	// Is auto negotiation on
+	if ((value & PHY_BMCR_AUTON) != 0) 
+	{
+		wpcm_miiphy_read (devname, phyAddr, PHY_ANLPAR, &value);
+
+		if ((value & (PHY_ANLPAR_10FD | PHY_ANLPAR_TXFD)) != 0) 
+		{
+			return (FULL);
+		} else 	
+		{
+			return (HALF);
+		}
+	}
+    else if (value & PHY_BMCR_DPLX) 
+	{
+		return (FULL);
+	} else 	
+	{
+		return (HALF);
+	}
+
+}
+
+/* 
+ * DetectPhyAddr
+ * Used to detect the phy address based on the device name
+ *
+ * scans & stores the phy address of wpcm_eth0 & wpcm_eth1
+ */
+void DetectPhyAddr(char *devname)
+{
+   int num = -1, count = 0, count1=1;
+   unsigned short value =0;
+//   unsigned short value1=0;
+   
+	if (devname == NULL)
+	{
+      return;
+   }
+   num = GetDevNum(devname);
+   if (num == -1 || num > MAX_WPCM_ETH)
+   {
+      return;
+   }
+
+	// reset the phy
+	wpcm_miiphy_write(devname, count1, MII_PHY_CR, MII_PHY_CR_RESET);
+
+	// delay a while
+	for(count = 0; count < 32; ++count)
+	{ 		
+		if (wpcm_miiphy_read(devname, count, MII_PHY_CR, &value) != -1)
+		{    
+			/*if ( (value != 0xFFFF) && (value & 0x3000) )
+			{
+				configPhyAddr[num] = count;
+				printf("PHY for device %s detected @ Addr 0x%x \n", devname, count);
+				// Reset Auto Negotiation 
+				ResetAutoNeg(devname);   
+				return;
+			}	*/		
+      	} 
+      
+   	}
+
+#if 0	// hw did the straping already
+	// disable Auto MDIX
+	if (wpcm_miiphy_read(devname, count1, MII_PHY_TEST, &value) != -1)  {  // National PHY register=19
+		value1 = (value & 0x7FE0) | 0x0001;		
+		wpcm_miiphy_write(devname, count1, MII_PHY_TEST, value1);
+	}
+
+	wpcm_miiphy_read(devname, count1, MII_PHY_TEST, &value);
+	printf("[DetectPhyAddr] Phy_19 value= 0x%x\n ", value);
+#endif
+
+	// For SGI, they need hard code PHY=1, 100MB/s, FULL duplex and disable MDIC
+	wpcm_miiphy_write(devname, count1, MII_PHY_CR, 0x2100);
+	
+	// Phy Addr is 1 for x8dtt		
+	configPhyAddr[num] = 1;
+
+   //printf("PHY Scan failed for device %s will be using default 0x%x\n", devname, configPhyAddr[num]);
+   return;
+}
+
