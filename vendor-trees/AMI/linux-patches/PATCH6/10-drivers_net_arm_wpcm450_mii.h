--- linux/drivers/net/arm/wpcm450_mii.h	1969-12-31 17:00:00.000000000 -0700
+++ linux.good/drivers/net/arm/wpcm450_mii.h	2008-04-18 19:23:03.000000000 -0500
@@ -0,0 +1,55 @@
+/*
+ * Ethernet driver for winbond WPCM450 SOC
+*/
+
+#ifndef _WPCM450_MII_H_
+#define _WPCM450_MII_H_
+
+#define PHY_CNTL_REG    0x00
+#define PHY_STATUS_REG  0x01
+#define PHY_ID1_REG     0x02
+#define PHY_ID2_REG     0x03
+#define PHY_ANA_REG     0x04
+#define PHY_ANLPA_REG   0x05
+#define PHY_ANE_REG     0x06
+
+
+//PHY Control Register
+#define RESET_PHY       (1 << 15)
+#define ENABLE_LOOPBACK (1 << 14)
+#define DR_100MB        (1 << 13)
+#define ENABLE_AN       (1 << 12)
+#define PHY_MAC_ISOLATE (1 << 10)
+#define RESTART_AN      (1 << 9)
+#define PHY_FULLDUPLEX  (1 << 8)
+#define PHY_COL_TEST    (1 << 7)
+
+// PHY Status Register
+#define LINK_STATUS_VALID  (1 << 2)
+#define PHY_REMOTE_FAULT   (1 << 4)
+#define PHY_AN_COMPLETE    (1 << 5)
+
+// PHY Auto Negotiation values
+#define PHY_ANLPAR_TXFD		0x0100
+#define PHY_ANLPAR_10FD		0x0040
+#define PHY_ANLPAR_100		0x0380	    
+
+
+
+#if 0
+// PHY Auto-negotiation Advertisement Register
+
+#define MODE_DR100_FULL   3
+#define MODE_DR100_HALF   2
+#define MODE_DR10_FULL    1
+#define MODE_DR10_HALF    0
+
+
+#define DR100_TX_FULL   (1 << 8)
+#define DR100_TX_HALF   (1 << 7)
+#define DR10_TX_FULL    (1 << 6)
+#define DR10_TX_HALF    (1 << 5)
+#define IEEE_802_3_CSMA_CD   1
+
+#endif
+#endif 
