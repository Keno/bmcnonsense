--- uboot-old/board/HERMON/ram_init.c	1969-12-31 17:00:00.000000000 -0700
+++ uboot-new/board/HERMON/ram_init.c	2008-08-02 18:44:27.000000000 -0500
@@ -0,0 +1,570 @@
+/******************************************************************************
+ *
+ * Copyright (c) 2003 Windond Electronics Corp.
+ * All rights reserved.
+ *
+ * $Workfile: ram_init.c $
+ *
+ * $Author: achechik $
+ ******************************************************************************/
+#include "wpcm450_hw.h"
+#include "macros.h"
+#include "clocks.h"
+
+
+
+/************************************************************************/
+/* TYPES DEFINITIONS                                                    */
+/************************************************************************/
+
+typedef enum 
+{
+  MEM_SKIP_INIT_MODE = 0x0, /* Skip all init and jump to SPI flash directly with no check. */
+  MEM_128MB_MODE     = 0x1, /* 128 MB. */
+  MEM_64MB_MODE      = 0x2, /* 64 MB.  */
+  MEM_32MB_MODE      = 0x3  /* 32 MB.  */
+}PowerOnMemSizeTypes_t;
+
+/************************************************************************/
+
+#define WAIT_MC_RESET(cnt)  {for(cnt=0;cnt<10000;){cnt++;}}
+
+/************************************************************************/
+/* TYPES DEFINITIONS                                                    */
+/************************************************************************/
+typedef struct {
+  UINT32 mc_cfg0[7];
+  UINT32 mc_cfg1[7];
+  UINT32 mc_cfg2[7];
+  UINT16 mc_cfg3;
+  UINT8  mc_cfg4[7];
+  UINT32 mc_cfg5;
+  UINT32 mc_cfg6;
+  UINT32 mc_cfg7;
+  UINT32 mc_p1_arbt;
+  UINT32 mc_p1_cnt;
+  UINT32 mc_p2_arbt[7];
+  UINT32 mc_p2_cnt;
+  UINT32 mc_p3_arbt;
+  UINT32 mc_p3_cnt;
+  UINT32 mc_p4_arbt;
+  UINT32 mc_p4_cnt;
+  UINT32 mc_p5_arbt;
+  UINT32 mc_p5_cnt;
+  UINT32 mc_p6_arbt;
+  UINT32 mc_p6_cnt;
+  UINT32 mc_p1_incrs;
+  UINT32 mc_p2_incrs;
+  UINT32 mc_p3_incrs;
+  UINT32 mc_p4_incrs;
+  UINT32 mc_dll_0;
+  UINT32 mc_dll_1;
+}MemCtrlCfg_t;
+
+
+/************************************************************************/
+/* GLOBAL VARIABLES                                                     */
+/************************************************************************/
+
+/************************************************************************/
+/* AHB/STREAMING DDR-2 CONTROLLER Configuration                         */
+/************************************************************************/
+
+
+//const MemCtrlCfg_t MC_Cfg[3] =
+#if 0
+static MemCtrlCfg_t MC_Cfg[3] =
+{
+  /* MEM_128MB_MODE */
+  {
+    /* 180 MHz    133 MHz     250 MHz     160MHz      220 MHz     125MHz      200 MHz  */
+    {0x241B457C, 0x2312240E, 0x3624079E, 0x239AA4E0, 0x362406B4, 0x231223CE, 0x241B4618},
+    {0x22162323, 0x21162223, 0x22162424, 0x22162323, 0x22162424, 0x21162223, 0x22162323},
+    {0x40000643, 0x40000443, 0x40000843, 0x40000643, 0x40000843, 0x40000443, 0x40000643},
+    0x6,
+    {0xA,        0x7,        0xD,        0x8,        0xD,        0x7,        0xA       },
+    0x190202, 
+    0x0, 
+    0x1,
+    0x23,
+    0x5,
+    {0x2A,       0x1E,       0x30,       0x24,       0x30,       0x1E,       0x2A    },
+    0x1,
+    0x64,
+    0x0,
+    0x64,
+    0x0,
+    0x1B8,
+    0x5,
+    0xC,
+    0x0,
+    0x2,
+    0x2,
+    0x4,
+    0x4,
+    0x00010106,
+    0x00000106
+   },
+  /* MEM_64MB_MODE */
+  {
+    /* 180 MHz     133 MHz     250 MHz     160MHz      220 MHz    125MHz      200 MHz  */
+    {0x241AA57C, 0x2311C40E, 0x3623679E, 0x239A24E0, 0x362366B4, 0x2311C3CE, 0x241AA618},
+    {0x22122323, 0x21122223, 0x22122424, 0x22122323, 0x22122424, 0x21122223, 0x22122323},
+    {0x40000643, 0x40000443, 0x40000843, 0x40000643, 0x40000843, 0x40000443, 0x40000643},
+    0x6,
+    {0xA,        0x7,        0xD,        0x8,        0xD,        0x7,        0xA       },
+    0x550202,
+    0x0,
+    0x1,
+    0x23,
+    0x5,
+    {0x2A,       0x1E,       0x30,       0x24,       0x30,       0x1E,       0x2A      },
+    0x1,
+    0x64,
+    0x0,
+    0x64,
+    0x0,
+    0x1B8,
+    0x5,
+    0xC,
+    0x0,
+    0x2,
+    0x2,
+    0x4,
+    0x4,
+    0x00010106,
+    0x00000106
+  },
+  /* MEM_32MB_MODE */
+  {
+    /*180 MHz     133 MHz     250 MHz     160MHz      220 MHz     125MHz      200 MHz   */
+    {0x249B857C, 0x2312A40E, 0x3624E79E, 0x241B24E0, 0x352466B4, 0x231283CE, 0x249BE618},
+    {0x220E2324, 0x210E2224, 0x220E2424, 0x220E2324, 0x220E2424, 0x110E2224, 0x220E2324},
+    {0x40000643, 0x40000443, 0x40000843, 0x40000643, 0x40000843, 0x40000443, 0x40000643},
+    0x6,
+    {0x9,        0x7,        0xD,        0x8,        0xB,        0x7,        0xA       },
+    0x310202,
+    0x0,
+    0x1,
+    0x23,
+    0x5,
+    {0x30,       0x1E,       0x30,       0x24,       0x30,       0x1E,      0x30       },
+    0x1,
+    0x64,
+    0x0,
+    0x64,
+    0x0,
+    0x1B8,
+    0x5,
+    0xC,
+    0x0,
+    0x2,
+    0x2,
+    0x4,
+    0x4,
+    0x00010106,
+    0x00000106
+  }
+};
+#endif
+
+/************************************************************************/
+/* EXTERNAL VARIABLES                                                   */
+/************************************************************************/
+
+
+/************************************************************************/
+/* EXTERNAL FUNCTIONS                                                   */
+/************************************************************************/
+
+
+void WBL_RamInit(void)
+{
+     
+  
+  UINT32 idx1 = 0;
+  UINT32 idx2 = 0; 
+  UINT32 pwron_cfg = *(volatile unsigned long *)GCR_PWRON;
+  volatile UINT32 temp ;
+
+  /************************************************************************/
+  /* If watchdog reset was done. Don't do any memory controller init      */
+  /************************************************************************/
+  if(IS_BIT_SET(*(volatile unsigned long*)TIMER_WTCR, WTCR_RESET_FLAG_BIT)) 
+  	{
+		// Clear the timer reset flag & reset the timer 
+		*(volatile unsigned long * )TIMER_WTCR = 0x405;  
+		return;
+  	}
+
+  /************************************************************************/
+  /*  RAM Configuration Part                                              */
+  /************************************************************************/
+
+
+  /************************************************************************/
+  /*  idx1 is RAM size index                                              */
+  /************************************************************************/
+  idx1 = GET_FIELD(pwron_cfg, PWRON_RAM_SIZE);
+
+
+  if(idx1 == MEM_SKIP_INIT_MODE)
+  {
+    idx1 = MEM_128MB_MODE;
+  }
+  
+  if(idx1 == MEM_128MB_MODE)
+  {
+    SET_BIT(*(volatile unsigned long *)GCR_MFSEL1, MF_MBEN_BIT);
+  }
+
+  /************************************************************************/
+  /* Set the Memory Controller reset bit                                  */
+  /************************************************************************/ 
+
+  SET_BIT(*(volatile unsigned long *)CLK_IPSRST,CLK_IPSRST_MC_BIT); 
+
+  /* Wait Loop */
+  WAIT_MC_RESET(idx2);
+ 
+  /************************************************************************/
+  /* Release memory Controller from reset by                              */
+  /* clearing Memory Controller reset bit                                 */
+  /************************************************************************/
+  
+  CLEAR_BIT(*(volatile unsigned long *)CLK_IPSRST,CLK_IPSRST_MC_BIT);    
+  
+
+  /* Wait Loop */
+  WAIT_MC_RESET(idx2);
+
+    
+  /************************************************************************/
+  /*  idx2 is clock frequency index                                       */
+  /************************************************************************/
+  idx2 = GET_FIELD(pwron_cfg, PWRON_CPU_CORE_CLK);
+
+
+  
+  if((idx2 == CLK_BYPASS_MODE))
+  {
+    idx2 = CLK_250MHz_MODE; /* Configured MC to 250MHz mode */
+  }
+
+  idx1 -=1; /* MC_Cfg array not contain SKIP mode configuration values */
+
+  idx2 -=1; /* MC_CFG array not contain BY_PASS mode configuration values  */
+
+/*
+  *(volatile unsigned long *)MC_CFG0     = MC_Cfg[idx1].mc_cfg0[idx2];
+  *(volatile unsigned long *)MC_CFG1     = MC_Cfg[idx1].mc_cfg1[idx2];
+  *(volatile unsigned long *)MC_CFG2     = MC_Cfg[idx1].mc_cfg2[idx2];
+  *(volatile unsigned long *)MC_CFG3     = MC_Cfg[idx1].mc_cfg3;
+  *(volatile unsigned long *)MC_CFG4     = MC_Cfg[idx1].mc_cfg4[idx2];
+  *(volatile unsigned long *)MC_CFG5     = MC_Cfg[idx1].mc_cfg5;
+  *(volatile unsigned long *)MC_CFG6     = MC_Cfg[idx1].mc_cfg6;
+  // *(volatile unsigned long *)MC_CFG7     = MC_Cfg[idx1].mc_cfg7;
+  *(volatile unsigned long *)MC_P1_ARBT  = MC_Cfg[idx1].mc_p1_arbt;
+  *(volatile unsigned long *)MC_P1_CNT   = MC_Cfg[idx1].mc_p1_cnt;
+  *(volatile unsigned long *)MC_P2_ARBT  = MC_Cfg[idx1].mc_p2_arbt[idx2];
+  *(volatile unsigned long *)MC_P2_CNT   = MC_Cfg[idx1].mc_p2_cnt;
+  *(volatile unsigned long *)MC_P3_ARBT  = MC_Cfg[idx1].mc_p3_arbt;
+  *(volatile unsigned long *)MC_P3_CNT   = MC_Cfg[idx1].mc_p3_cnt;
+  *(volatile unsigned long *)MC_P4_ARBT  = MC_Cfg[idx1].mc_p4_arbt;
+  *(volatile unsigned long *)MC_P4_CNT   = MC_Cfg[idx1].mc_p4_cnt;
+  *(volatile unsigned long *)MC_P5_ARBT  = MC_Cfg[idx1].mc_p5_arbt;
+  *(volatile unsigned long *)MC_P5_CNT   = MC_Cfg[idx1].mc_p5_cnt;
+  *(volatile unsigned long *)MC_P6_ARBT  = MC_Cfg[idx1].mc_p6_arbt;
+  *(volatile unsigned long *)MC_P6_CNT   = MC_Cfg[idx1].mc_p6_cnt;
+  *(volatile unsigned long *)MC_P1_INCRS = MC_Cfg[idx1].mc_p1_incrs;
+  *(volatile unsigned long *)MC_P2_INCRS = MC_Cfg[idx1].mc_p2_incrs;
+  *(volatile unsigned long *)MC_P3_INCRS = MC_Cfg[idx1].mc_p3_incrs;
+  *(volatile unsigned long *)MC_P4_INCRS = MC_Cfg[idx1].mc_p4_incrs;
+  *(volatile unsigned long *)MC_DLL_0    = MC_Cfg[idx1].mc_dll_0;
+  *(volatile unsigned long *)MC_DLL_1    = MC_Cfg[idx1].mc_dll_1;
+*/  
+	if(idx1 == 0) {
+		  /* MEM_128MB_MODE */
+      /* 180 MHz    133 MHz     250 MHz     160MHz      220 MHz     125MHz      200 MHz  */
+//    {0x241B457C, 0x2312240E, 0x3624079E, 0x239AA4E0, 0x362406B4, 0x231223CE, 0x241B4618},
+		if(idx2 == 0) { // 180MHZ
+			//MC_CFG0 = 0x241B457C ;
+			temp = 0x7C ;
+			temp |= 0x45 << 8;
+			temp |= 0x1B << 16 ;
+			temp |= 0x24 << 24 ;
+			*(volatile unsigned long *)MC_CFG0 = temp ;
+		}
+		if(idx2 == 1) { // 133MHZ
+			//MC_CFG0 = 0x2312240E ;
+			temp = 0x0E ;
+			temp |= 0x24 << 8 ;
+			temp |= 0x12 << 16 ;
+			temp |= 0x23 << 24 ;
+			*(volatile unsigned long *)MC_CFG0 = temp ;
+		}
+		if(idx2 == 2) { // 250MHZ
+			//MC_CFG0 = 0x3624079E ;
+			temp = 0x9E ;
+			temp |= 0x07 << 8 ;
+			temp |= 0x24 << 16 ;
+			temp |= 0x36 << 24 ;
+			*(volatile unsigned long *)MC_CFG0 = temp ;
+		}
+		if(idx2 == 3) { // 160MHZ
+			//MC_CFG0 = 0x239AA4E0 ;
+			temp = 0xE0 ;
+			temp |= 0xA4 << 8 ;
+			temp |= 0x9A << 16 ;
+			temp |= 0x23 << 24 ;
+			*(volatile unsigned long *)MC_CFG0 = temp ;
+		}
+		if(idx2 == 4) { // 220MHZ
+			//MC_CFG0 = 0x362406B4 ;
+			temp = 0xB4 ;
+			temp |= 0x06 << 8 ;
+			temp |= 0x24 << 16 ;
+			temp |= 0x36 << 24 ;
+			*(volatile unsigned long *)MC_CFG0 = temp ;
+		}
+		if(idx2 == 5) { // 125MHZ
+			//MC_CFG0 =  0x231223CE ;
+			temp = 0xCE ;
+			temp |= 0x23 << 8 ;
+			temp |= 0x12 << 16 ;
+			temp |= 0x23 << 24 ;
+			*(volatile unsigned long *)MC_CFG0 = temp ;
+		}
+		if(idx2 == 6) { // 200MHZ
+			//MC_CFG0 = 0x241B4618 ;
+			temp = 0x18 ;
+			temp |= 0x46 << 8 ;
+			temp |= 0x1B << 16 ;
+			temp |= 0x24 << 24 ;
+			*(volatile unsigned long *)MC_CFG0 = temp ;
+		}
+		if(idx2 == 7) { // INVALID
+			while(1) ;
+		}
+
+//    {0x22162323, 0x21162223, 0x22162424, 0x22162323, 0x22162424, 0x21162223, 0x22162323},
+		if(idx2 == 0) { // 180MHZ
+			//MC_CFG1 = 0x22162323 ;
+			temp = 0x23 ;
+			temp |= 0x23 << 8 ;
+			temp |= 0x16 << 16 ;
+			temp |= 0x22 << 24 ;
+			*(volatile unsigned long *)MC_CFG1 = temp ;
+		}
+		if(idx2 == 1) { // 133MHZ
+			//MC_CFG1 = 0x21162223 ;
+			temp = 0x23 ;
+			temp |= 0x22 << 8 ;
+			temp |= 0x16 << 16 ;
+			temp |= 0x21 << 24 ;
+			*(volatile unsigned long *)MC_CFG1 = temp ;
+		}
+		if(idx2 == 2) { // 250MHZ
+			//MC_CFG1 =  0x22162424 ;
+			temp = 0x24 ;
+			temp |= 0x24 << 8 ;
+			temp |= 0x16 << 16 ;
+			temp |= 0x22 << 24 ;
+			*(volatile unsigned long *)MC_CFG1 = temp ;
+		}
+		if(idx2 == 3) { // 160MHZ
+			//MC_CFG1 = 0x22162323 ;
+			temp = 0x23 ;
+			temp |= 0x23 << 8 ;
+			temp |= 0x16 << 16 ;
+			temp |= 0x22 << 24 ;
+			*(volatile unsigned long *)MC_CFG1 = temp ;
+		}
+		if(idx2 == 4) { // 220MHZ
+			//MC_CFG1 = 0x22162424 ;
+			temp = 0x24 ;
+			temp |= 0x24 << 8 ;
+			temp |= 0x16 << 16 ;
+			temp |= 0x22 << 24 ;
+			*(volatile unsigned long *)MC_CFG1 = temp ;
+		}
+		if(idx2 == 5) { // 125MHZ
+			//MC_CFG1 =  0x21162223 ;
+			temp = 0x23 ;
+			temp |= 0x22 << 8 ;
+			temp |= 0x16 << 16 ;
+			temp |= 0x21 << 24 ;
+			*(volatile unsigned long *)MC_CFG1 = temp ;
+		}
+		if(idx2 == 6) { // 200MHZ
+			//MC_CFG1 = 0x22162323 ;
+			temp = 0x23 ;
+			temp |= 0x23 << 8 ;
+			temp |= 0x16 << 16 ;
+			temp |= 0x22 << 24 ;
+			*(volatile unsigned long *)MC_CFG1 = temp ;
+		}
+		if(idx2 == 7) { // INVALID
+			while(1) ;
+		}
+
+//    {0x40000643, 0x40000443, 0x40000843, 0x40000643, 0x40000843, 0x40000443, 0x40000643},
+		if(idx2 == 0) { // 180MHZ
+			//MC_CFG2 = 0x40000643 ;
+			temp = 0x43 ;
+			temp |= 0x06 << 8 ;
+			temp |= 0x00 << 16 ;
+			temp |= 0x40 << 24 ;
+			*(volatile unsigned long *)MC_CFG2 = temp ;
+		}
+		if(idx2 == 1) { // 133MHZ
+			//MC_CFG2 = 0x40000443 ;
+			temp = 0x43 ;
+			temp |= 0x04 << 8 ;
+			temp |= 0x00 << 16 ;
+			temp |= 0x40 << 24 ;
+			*(volatile unsigned long *)MC_CFG2 = temp ;
+		}
+		if(idx2 == 2) { // 250MHZ
+			//MC_CFG2 = 0x40000843 ;
+			temp = 0x43 ;
+			temp |= 0x08 << 8 ;
+			temp |= 0x00 << 16 ;
+			temp |= 0x40 << 24 ;
+			*(volatile unsigned long *)MC_CFG2 = temp ;
+		}
+		if(idx2 == 3) { // 160MHZ
+			//MC_CFG2 =  0x40000643 ;
+			temp = 0x43 ;
+			temp |= 0x06 << 8 ;
+			temp |= 0x00 << 16 ;
+			temp |= 0x40 << 24 ;
+			*(volatile unsigned long *)MC_CFG2 = temp ;
+		}
+		if(idx2 == 4) { // 220MHZ
+			//MC_CFG2 =  0x40000843 ;
+			temp = 0x43 ;
+			temp |= 0x08 << 8 ;
+			temp |= 0x00 << 16 ;
+			temp |= 0x40 << 24 ;
+			*(volatile unsigned long *)MC_CFG2 = temp ;
+		}
+		if(idx2 == 5) { // 125MHZ
+			//MC_CFG2 = 0x40000443 ;
+			temp = 0x43 ;
+			temp |= 0x04 << 8 ;
+			temp |= 0x00 << 16 ;
+			temp |= 0x40 << 24 ;
+			*(volatile unsigned long *)MC_CFG2 = temp ;
+		}
+		if(idx2 == 6) { // 200MHZ
+			//MC_CFG2 = 0x40000643 ;
+			temp = 0x43 ;
+			temp |= 0x06 << 8 ;
+			temp |= 0x00 << 16 ;
+			temp |= 0x40 << 24 ;
+			*(volatile unsigned long *)MC_CFG2 = temp ;
+		}
+		if(idx2 == 7) { // INVALID
+			while(1) ;
+		}
+
+//    0x6,
+		*(volatile unsigned long *)MC_CFG3 = 0x6 ;
+
+//    {0xA,        0x7,        0xD,        0x8,        0xD,        0x7,  0xA },
+		if(idx2 == 0) { // 180MHZ
+			*(volatile unsigned long *)MC_CFG4 = 0xA ;
+		}
+		if(idx2 == 1) { // 133MHZ
+			*(volatile unsigned long *)MC_CFG4 = 0x7 ;
+		}
+		if(idx2 == 2) { // 250MHZ
+			*(volatile unsigned long *)MC_CFG4 = 0xD ;
+		}
+		if(idx2 == 3) { // 160MHZ
+			*(volatile unsigned long *)MC_CFG4 = 0x8 ;
+		}
+		if(idx2 == 4) { // 220MHZ
+			*(volatile unsigned long *)MC_CFG4 = 0xD ;
+		}
+		if(idx2 == 5) { // 125MHZ
+			*(volatile unsigned long *)MC_CFG4 = 0x7 ;
+		}
+		if(idx2 == 6) { // 200MHZ
+			*(volatile unsigned long *)MC_CFG4 = 0xA ;
+		}
+		if(idx2 == 7) { // INVALID
+			while(1) ;
+		}
+
+		//MC_CFG5 = 0x190202  ; 	// MC_CFG5
+			temp = 0x02 ;
+			temp |= 0x02 << 8 ;
+			temp |= 0x19 << 16 ;
+			*(volatile unsigned long *)MC_CFG5 = temp ;
+		*(volatile unsigned long *)MC_CFG6 = 0x0 ;	// MC_CFG6
+// NOT USED!!    0x1,	// CFG7
+		*(volatile unsigned long *)MC_P1_ARBT = 0x23 ;	// MC_P1_ARBT
+		*(volatile unsigned long *)MC_P1_CNT = 0x5 ;	// MC_P!_CNT
+
+//    {0x2A,0x1E,0x30,0x24,0x30,0x1E,0x2A}, // MC_P2_ARBT
+
+		if(idx2 == 0) { // 180MHZ
+			*(volatile unsigned long *)MC_P2_ARBT = 0x2A ;
+		}
+		if(idx2 == 1) { // 133MHZ
+			*(volatile unsigned long *)MC_P2_ARBT = 0x1E ;
+		}
+		if(idx2 == 2) { // 250MHZ
+			*(volatile unsigned long *)MC_P2_ARBT = 0x30 ;
+		}
+		if(idx2 == 3) { // 160MHZ
+			*(volatile unsigned long *)MC_P2_ARBT = 0x24 ;
+		}
+		if(idx2 == 4) { // 220MHZ
+			*(volatile unsigned long *)MC_P2_ARBT = 0x30 ;
+		}
+		if(idx2 == 5) { // 125MHZ
+			*(volatile unsigned long *)MC_P2_ARBT = 0x1E ;
+		}
+		if(idx2 == 6) { // 200MHZ
+			*(volatile unsigned long *)MC_P2_ARBT = 0x2A ;
+		}
+		if(idx2 == 7) { // INVALID
+			while(1) ;
+		}
+
+		*(volatile unsigned long *)MC_P2_CNT = 0x1 ;	// MC_P2_CNT
+		*(volatile unsigned long *)MC_P3_ARBT = 0x64 ;	// MC_P3_ARBT
+		*(volatile unsigned long *)MC_P3_CNT = 0x0	; 	// MC_P3_CNT
+		*(volatile unsigned long *)MC_P4_ARBT = 0x64 ;	// MC_P4_ARBT
+		*(volatile unsigned long *)MC_P4_CNT = 0x0 ;	// MC_P4_CNT
+		//*(volatile unsigned long *)MC_P5_ARBT = 0x1B8 ;	// MC_P5_ARBT
+			temp = 0xB8 ;
+			temp |= 0x01 << 8 ;
+			*(volatile unsigned long *)MC_P5_ARBT = temp ;
+		*(volatile unsigned long *)MC_P5_CNT = 0x5 ;	// MC_P5_CNT
+		*(volatile unsigned long *)MC_P6_ARBT = 0xC ;	// MC_P6_ARBT
+		*(volatile unsigned long *)MC_P6_CNT = 0x0	; 	// MC_P6_CNT
+		*(volatile unsigned long *)MC_P1_INCRS = 0x2 ;	// MC_P1_INCRS
+		*(volatile unsigned long *)MC_P2_INCRS = 0x2 ;	// MC_P2_INCRS
+		*(volatile unsigned long *)MC_P3_INCRS = 0x4 ;	// MC_P3_INCRS
+		*(volatile unsigned long *)MC_P4_INCRS = 0x4 ;	// MC_P4_INCRS
+		*(volatile unsigned long *)MC_DLL_0	= 0x10106;
+		*(volatile unsigned long *)MC_DLL_1	= 0x106;
+		// NOT USED    0x00010106,
+		// NOT USED    0x00000106
+	} ; // End IDX1 == 0
+	if (idx1 == 1) { // 64MB
+		while (1) ;
+	} // end idx1 == 1
+	if (idx1 == 2) { // 32MB
+		while (1) ;
+	} // end idx1 == 2
+
+}
+
+
+
+
