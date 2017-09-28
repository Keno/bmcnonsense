--- u-boot-1.1.4-cpu/cpu/arm926ejs/WPCM450/hermon_mac.h	1969-12-31 17:00:00.000000000 -0700
+++ u-boot-1.1.4-wpcm/cpu/arm926ejs/WPCM450/hermon_mac.h	2008-04-15 19:38:09.000000000 -0500
@@ -0,0 +1,313 @@
+#ifndef _HERMON_MAC_H
+#define _HERMON_MAC_H
+
+#define RXDES_NUM   		16
+#define TXDES_NUM			8
+#define RX_BUF_SIZE			1536
+#define TX_BUF_SIZE			1536
+#define SETUP_FRAME_SIZE		192
+
+typedef struct
+{
+	u32 SL;		// old was Status
+	u32 Buffer; 	// old was Length
+	u32 Reserved ;	// old was Buffer1
+	u32 next;	// old was Buffer2
+} RECV_DESC;
+
+typedef struct
+{
+	u32 mode;	// old was Status
+	u32 Buffer;	// old was Length
+	u32 SL;		// old was Buffer1
+	u32 next;	// old was Buffer2
+} XMIT_DESC;
+
+
+/*---------------------------------------------------------------------------------
+ * 							Private Information of driver
+ *---------------------------------------------------------------------------------*/
+typedef struct mac100_local
+{
+	unsigned long 	hw_addr;				// IO Base Addr
+	int 			devnum;					// Instance Number
+
+	volatile RECV_DESC 	*rx_descs;			// receive ring base address
+	volatile RECV_DESC	*rx_descs_dma;		// receive ring base physical address
+	volatile char		*rx_buf;			// receive buffer cpu address
+	volatile char		*rx_buf_dma;		// receive buffer physical address
+	int					rx_idx;				// receive descriptor
+
+	volatile XMIT_DESC 	*tx_descs;
+	volatile XMIT_DESC	*tx_descs_dma;
+	volatile char		*tx_buf;
+	volatile char		*tx_buf_dma;
+	int					tx_idx;
+
+	volatile XMIT_DESC 	*setup_descs;
+	volatile XMIT_DESC	*setup_descs_dma;
+	volatile char		*setup_frame;
+	volatile char		*setup_frame_dma;
+
+} ETH_PRIV_DATA;
+
+//CAM Registers
+#define  CAMCMR			(0)    //CAM Command Regiser
+#define  CAMEN			(0x4)//CAM ennable regiser
+#define  CAM0M			(0x8)//CAM1 Most significant Word register
+#define  CAM0L			(0xc)//CAM1 Least Significant Word Register
+#define  CAM_ENTRY_SIZE	0x8     		//CAM  entry size
+#define  CAM_ENTRIES	0x16    		//CAM  entries
+#define CAM0_L		0xC
+#define CAM0_M		0x8
+
+//MAC Regiseters
+#define MIEN			(0xac) //MAC Interrupt Enable Register
+#define MCMDR			(0x90) //MAC Command Regiser
+#define MIID			(0x94) //MII Management Data Register
+#define MIIDA			(0x98) //MII Management Data Control and Address Register
+#define MPCNT			(0xb8) //Missed Packet Counter Register
+
+//DMA Registers
+#define TXDLSA			(0x88) //Transmit Descriptor Link List Start Address Regiser 
+#define RXDLSA			(0x8c) //Receive Descriptor LInk List Start Addresss Register
+#define DMARFC			(0xa8) //DMA Receive Frame Control Register
+#define TSDR			(0xa0) //Transmit Start Demand Register
+#define RSDR			(0xa4) //Recevie Start Demand Register
+#define FFTCR			(0x9c) //FIFO Threshold Adjustment Register
+
+//EMC Status Register
+#define MISTA			(0xb0) //MAC Interrupter Status Register
+#define MGSTA			(0xb4) //MAC General Status Register
+#define MRPC			(0xbc)  //MAC Receive Pauese counter register
+#define MRPCC			(0xc0) //MAC Receive Pauese Current Count Regiser
+#define MREPC			(0xc4)  //MAC Remote pause count retister
+
+//DMA Registers
+#define DMARFS			(0xc8) //DMA Receive Frame Status Register
+#define CTXDSA			(0xcc) //Current Transmit Descriptor Start Addresss Register
+#define CTXBSA			(0xd0) //Current Transmit Buffer Start Address Regiser
+#define CRXDSA			(0xd4) //Current Receive Descriptor start Address regiser
+#define CRXBSA			(0xd8) //Current Receive Buffer Start Address Regiser
+
+//Debug Mode Receive Finite State Machine Registers
+#define RXFSM			(0x200)
+#define TXFSM			(0x204)
+#define FSM0			(0x208)
+#define FSM1			(0x20c)
+
+//More registers
+#define DMMIR           (0x0214)
+#define BISTR           (0x0300)
+
+// Phy Registers
+#define PHY_DSC_REG	0x10
+
+
+//Phy Status Register
+#define PHY_LINK_STATUS_VALID  (1 << 2)
+#define PHY_REMOTE_FAULT   (1 << 4)
+#define PHY_AN_COMPLETE    (1 << 5)
+
+
+// Bit defs for Phy Registers
+#define LINK_STATUS_VALID	( 1 << 0)
+#define SPEED_STATUS_10		( 1 << 1)
+#define DUPLEX_STATUS_FULL	( 1 << 2)
+#define PHYBUSY  0x00020000  // Busy Bit
+#define MDCCR1   0x00F00000  // MDC clock rating
+#define PHYWR    0x00010000  // Write Operation
+
+// Bit defs for registers
+#define MCMDR_RXON	(1 << 0)
+#define MCMDR_TXON	(1 << 8)
+#define MCMDR_SPCRC	(1 << 5)
+#define MCMDR_EnMDC	(1 << 19)
+#define MCMDR_ACP	(1 << 3)
+#define MCMDR_OPMOD	(1 << 20)
+#define MCMDR_FDUP	(1 << 18)
+
+#define MISTA_RDU	(1 << 10)
+#define MISTA_RxBErr	(1 << 11)
+
+#define CAMCMR_ECMP	(1 << 4)
+#define CAMCMR_ABP	(1 << 2)
+#define CAMCMR_AMP	(1 << 1)
+
+
+// Bit defs for rx and tx descriptors
+#define RDESC0_DMA_OWN	0x80000000 // 0 = CPU
+#define RDESC0_RXGD	(1 << 20)
+
+// Tx Frame Descriptor's Control bits
+#define MACTxIntEn    0x04
+#define CRCMode       0x02
+#define NoCRCMode     0x00
+#define PaddingMode   0x01
+#define NoPaddingMode 0x00
+
+
+/*---------------------------------------------------------------------------------*/
+/*------------------- Defines for Recv and Xmit Descriptors -----------------------*/
+/*---------------------------------------------------------------------------------*/
+
+/* receive descriptor 0 */
+#define RDESC0_OWN			0x80000000	// Own
+#define RDESC0_FF			0x40000000	// Filtering Fail
+
+#define RDESC0_FL_MSK		0x3FFF0000	// Frame length mask
+#define RDESC0_FL_GET(x)	(((x) & RDESC0_FL_MSK) >> 16)
+#define RDESC0_FL_PUT(x)	(((x) << 16) & RDESC0_FL_MSK)
+
+#define RDESC0_ES			0x00008000	// Error summary
+#define RDESC0_DE			0x00004000  // Descriptor error
+#define RDESC0_RF			0x00000800	// runt frame
+#define RDESC0_MF			0x00000400	// multicast frame
+#define RDESC0_FD			0x00000200	// first descriptor
+#define RDESC0_LS			0x00000100	// last descriptor
+#define RDESC0_TL			0x00000080	// frame too long
+#define RDESC0_CS			0x00000040	// collision seen
+#define RDESC0_FT			0x00000020	// frame type
+#define RDESC0_RE			0x00000008	// Report MII error
+#define RDESC0_DB			0x00000004	// dribbling bit
+#define RDESC0_CE			0x00000002	// crc error
+#define RDESC0_FIFOERR		0x00000001	// Legal length
+#define	RDESC0_FT_GET(x)	(((x) & RDESC0_FT) >> 5)
+
+/* receive descriptor 1 */
+#define RDESC1_RER			0x02000000	// recv end of ring
+#define RDESC1_RCH			0x01000000	// second address chained
+
+#define RDESC1_RBS2_MSK		0x003FF800	// RBS2 buffer 2 size
+#define RDESC1_RBS1_MSK		0x000007FF	// RBS1 buffer 1 size
+
+#define RDESC1_RBS1_VAL(x)	((x) & RDESC1_RBS1_MSK)	// multiple of 4
+#define RDESC1_RBS2_VAL(x)	(((x) << 11) & RDESC1_RBS2_MSK)
+
+/* xmit descriptor 0 */
+#define TDESC0_OWN			0x80000000	// own */
+#define TDESC0_ES			0x00008000	// error summary
+#define TDESC0_LO			0x00000800	// loss of carrier
+#define TDESC0_NC			0x00000400	// NC No carrier
+#define TDESC0_LC			0x00000200	// late collision
+#define TDESC0_EC			0x00000100	// excessive collision
+
+#define TDESC0_CC_MSK		0x00000078	// collision count
+#define TDESC0_CC_GET(x)    (((x) & TDESC0_CC_MSK) >> 3)
+
+#define TDESC0_UF			0x00000002	// underflow error
+#define TDESC0_DE			0x00000001	// deffered
+
+/* xmit descriptor 1 */
+#define TDESC1_IC			0x80000000	// interrupt on completion
+#define TDESC1_LS			0x40000000	// last segment
+#define TDESC1_FS			0x20000000	// first segment
+#define TDESC1_FT1			0x10000000	// filtering type
+#define TDESC1_SET			0x08000000	// setup packet
+#define TDESC1_AC			0x04000000	// add crc disable
+#define TDESC1_TER			0x02000000	// xmit end of ring
+#define TDESC1_TCH			0x01000000	// second address chained
+#define TDESC1_DPD			0x00800000	// disabled padding
+#define TDESC1_FT0			0x00400000	// filtering type
+
+#define TDESC1_TBS2_MSK		0x003FF800	// TBS2 buffer 2 size
+#define TDESC1_TBS1_MSK		0x000007FF	// TBS2 buffer 1 size
+
+#define TDESC1_PERFECT		0x00000000
+#define TDESC1_FT0			0x00400000
+#define TDESC1_FT1			0x10000000
+#define TDESC1_HASH			TDESC1_FT0
+#define TDESC1_INVERSE		TDESC1_FT1
+#define TDESC1_HASH_ONLY	(TDESC1_FT0|TDESC1_FT1)
+//#endif
+/*---------------------------------------------------------------------------------*/
+/*-------------------------------- Setup Frame  -----------------------------------*/
+/*---------------------------------------------------------------------------------*/
+#define FLTR_FRM_SIZE			0xC0		// filter frm size 192 bytes
+#define FLTR_FRM_SIZE_ULONGS	(FLTR_FRM_SIZE / sizeof (u32))
+#define FLTR_FRM_ADRS_NUM		0x10		// filter frm holds 16 addrs
+#define FLTR_FRM_ADRS_SIZE		0x06		// size of each phys addrs
+#define FLTR_FRM_DEF_ADRS		0xFFFFFFFF	// enet broad cast address
+
+#define FLTR_HASH_FRM_SIZE			0x80		// Hash filter frm size 128 bytes
+#define FLTR_HASH_FRM_SIZE_ULONGS	(FLTR_HASH_FRM_SIZE / sizeof (u32))
+
+/*---------------------------------------------------------------------------------*/
+/*------------------------------- MII/PHY defines ---------------------------------*/
+/*---------------------------------------------------------------------------------*/
+#define	MAC_MAX_PHY			32	// max number of PHY devices
+#define	MAC_MAX_LINK_TOUT	16	// max link timeout (in secs)
+#define MII_PREAMBLE		((u32) 0xFFFFFFFF)
+
+/* MII frame header format */
+#define MII_SOF				0x4	// start of frame
+#define MII_RD				0x2	// op-code: Read
+#define	MII_WR				0x1	// op-code: Write
+
+/* MII PHY registers */
+#define MII_PHY_CR			0x00	// Control Register
+#define MII_PHY_SR			0x01	// Status Register
+#define MII_PHY_ID0			0x02	// Identifier Register 0
+#define MII_PHY_ID1			0x03	// Identifier Register 1
+#define MII_PHY_ANA			0x04	// Auto Negot'n Advertisement
+#define MII_PHY_ANLPA		0x05	// Auto Negot'n Link Partner Ability
+#define MII_PHY_ANE			0x06	// Auto Negot'n Expansion
+#define MII_PHY_ANP			0x07	// Auto Negot'n Next Page TX
+/* SW ISSUE */
+#define	MII_PHY_LBR         0x11	// Loopback Register
+#define	MII_PHY_TEST        0x19	// Test Register
+
+/* MII_PHY control register */
+#define	MII_PHY_CR_RESET	0x8000	/* reset */
+#define	MII_PHY_CR_LOOP		0x4000	/* loopback enable */
+#define	MII_PHY_CR_100M		0x2000	/* speed 100Mbps */
+#define	MII_PHY_CR_10M		0x0000	/* speed 10Mbps */
+#define	MII_PHY_CR_AUTO		0x1000	/* auto speed enable */
+#define	MII_PHY_CR_OFF		0x0800	/* powerdown mode */
+#define	MII_PHY_CR_RAN		0x0200	/* restart auto negotiate */
+#define	MII_PHY_CR_FDX		0x0100	/* full duplex mode */
+#define	MII_PHY_CR_HDX		0x0000	/* Half duplex mode */
+
+/* MII PHY status register */
+#define MII_PHY_SR_100T4	0x8000	/* 100BASE-T4 capable */
+#define MII_PHY_SR_100TX_FD	0x4000	/* 100BASE-TX Full Duplex capable */
+#define MII_PHY_SR_100TX_HD	0x2000	/* 100BASE-TX Half Duplex capable */
+#define MII_PHY_SR_10TFD	0x1000	/* 10BASE-T Full Duplex capable */
+#define MII_PHY_SR_10THD	0x0800	/* 10BASE-T Half Duplex capable */
+#define MII_PHY_SR_MF		0x0040  /* Management Frams preamble suppressed */
+#define MII_PHY_SR_ASS		0x0020	/* Auto Speed Selection Complete*/
+#define MII_PHY_SR_RFD		0x0010	/* Remote Fault Detected */
+#define MII_PHY_SR_AN		0x0008	/* Auto Negotiation capable */
+#define MII_PHY_SR_LNK		0x0004	/* Link Status */
+#define MII_PHY_SR_JABD		0x0002	/* Jabber Detect */
+#define MII_PHY_SR_XC		0x0001	/* Extended Capabilities */
+
+/* MII PHY Auto Negotiation Advertisement Register */
+#define MII_PHY_ANA_TAF		0x03e0	/* Technology Ability Field */
+/* SW ISSUE */
+#define MII_PHY_ANA_T4AM	0x0200	/* T4 Technology Ability Mask */
+#define MII_PHY_ANA_100F	0x0100
+#define MII_PHY_ANA_100H	0x0080
+#define MII_PHY_ANA_10F		0x0040
+#define MII_PHY_ANA_10H		0x0020
+#define MII_PHY_ANA_FDAM	(MII_PHY_ANA_100F|MII_PHY_ANA_10F)	/* Full Duplex Technology Ability Mask */
+#define MII_PHY_ANA_HDAM	(MII_PHY_ANA_100H|MII_PHY_ANA_10H)	/* Half Duplex Technology Ability Mask */
+#define MII_PHY_ANA_100M	(MII_PHY_ANA_100F|MII_PHY_ANA_100H)	/* 100Mb Technology Ability Mask */
+#define MII_PHY_ANA_10M		(MII_PHY_ANA_10F|MII_PHY_ANA_10H)	/* 10Mb Technology Ability Mask */
+#define MII_PHY_ANA_ALLAM	(MII_PHY_ANA_T4AM|MII_PHY_ANA_100M|MII_PHY_ANA_10M)
+#define MII_PHY_ANA_CSMA	0x0001	/* CSMA-CD Capable */
+
+/* MII PHY Auto Negotiation Remote End Register */
+#define MII_PHY_ANLPA_NP	0x8000	/* Next Page (Enable) */
+#define MII_PHY_ANLPA_ACK	0x4000	/* Remote Acknowledge */
+#define MII_PHY_ANLPA_RF	0x2000	/* Remote Fault */
+#define MII_PHY_ANLPA_TAF	0x03e0	/* Technology Ability Field */
+#define MII_PHY_ANLPA_T4AM	0x0200	/* T4 Technology Ability Mask */
+#define MII_PHY_ANLPA_TXAM	0x0180	/* TX Technology Ability Mask */
+#define MII_PHY_ANLPA_FDAM	0x0140	/* Full Duplex Technology Ability Mask */
+#define MII_PHY_ANLPA_HDAM	0x02a0	/* Half Duplex Technology Ability Mask */
+#define MII_PHY_ANLPA_100M	0x0380	/* 100Mb Technology Ability Mask */
+#define MII_PHY_ANLPA_10M	0x0060	/* 10Mb Technology Ability Mask */
+#define MII_PHY_ANLPA_CSMA	0x0001	/* CSMA-CD Capable */
+
+#endif
