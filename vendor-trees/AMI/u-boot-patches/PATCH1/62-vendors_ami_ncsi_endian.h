--- uboot.org/vendors/ami/ncsi/endian.h	1969-12-31 19:00:00.000000000 -0500
+++ uboot/vendors/ami/ncsi/endian.h	2007-08-02 14:42:25.000000000 -0400
@@ -0,0 +1,105 @@
+/****************************************************************
+ ****************************************************************
+ **                                                            **
+ **    (C)Copyright 2005-2007, American Megatrends Inc.        **
+ **                                                            **
+ **            All Rights Reserved.                            **
+ **                                                            **
+ **        6145-F, Northbelt Parkway, Norcross,                **
+ **                                                            **
+ **        Georgia - 30071, USA. Phone-(770)-246-8600.         **
+ **                                                            **
+ ****************************************************************
+ ****************************************************************/
+/****************************************************************
+  Author	: Samvinesh Christopher
+
+  Module	: Endian Macros
+
+  Revision	: 1.0  
+
+  Changelog : 1.0 - Initial Version [SC]
+
+ *****************************************************************/
+#ifndef __ENDIAN_H__
+#define __ENDIAN_H__
+
+typedef unsigned long  uint32;
+typedef unsigned short uint16;
+typedef unsigned char  uint8;
+
+#ifdef  CFG_PROJ_ENDIAN_LITTLE 
+#define CPU_BYTE_ORDER_LITTLE_ENDIAN
+#endif
+
+#ifdef  CFG_PROJ_ENDIAN_BIG
+#define CPU_BYTE_ORDER_BIG_ENDIAN
+#endif
+
+#if !defined (CPU_BYTE_ORDER_BIG_ENDIAN) && ! defined (CPU_BYTE_ORDER_LITTLE_ENDIAN)
+#error Cannot determine CPU endianess. define CPU_BYTE_ORDER
+#endif
+
+#if defined (CPU_BYTE_ORDER_BIG_ENDIAN) && defined (CPU_BYTE_ORDER_LITTLE_ENDIAN)
+#error Cannot determine CPU endianess as both Little and Big endian specified
+#endif
+
+/* Generic Swap Functions */
+#define swap8(x)	((uint8)(x))
+#define swap16(x)	((uint16)(((x) << 8) | ((x) >> 8)))	
+#define swap32(x)   ((uint32)(((x) >> 24)|((x) << 24)|(((x)&0x00ff0000) >> 8)|(((x)&0x0000ff00) << 8)))
+
+#if defined (CPU_BYTE_ORDER_BIG_ENDIAN)	/* Big Endian CPU*/
+
+#define cpu_to_be32(x)	((uint32)(x))
+#define cpu_to_be16(x)	((uint16)(x))
+#define cpu_to_be8(x)	((uint8)(x))
+
+#define be32_to_cpu(x)	((uint32)(x))
+#define be16_to_cpu(x)	((uint16)(x))
+#define be8_to_cpu(x)	((uint8)(x))
+
+#define cpu_to_le32(x)	((uint32)(swap32((x))))
+#define cpu_to_le16(x)	((uint16)(swap16((x))))
+#define cpu_to_le8(x)	((uint8) (swap8 ((x))))
+
+#define le32_to_cpu(x)	((uint32)(swap32((x))))
+#define le16_to_cpu(x)	((uint16)(swap16((x))))
+#define le8_to_cpu(x)	((uint8) (swap8 ((x))))
+
+#endif
+
+#if defined (CPU_BYTE_ORDER_LITTLE_ENDIAN)	/* Little Endian CPU*/
+
+#define cpu_to_le32(x)	((uint32)(x))
+#define cpu_to_le16(x)	((uint16)(x))
+#define cpu_to_le8(x)	((uint8)(x))
+
+#define le32_to_cpu(x)	((uint32)(x))
+#define le16_to_cpu(x)	((uint16)(x))
+#define le8_to_cpu(x)	((uint8)(x))
+
+#define cpu_to_be32(x)	((uint32)(swap32((x))))
+#define cpu_to_be16(x)	((uint16)(swap16((x))))
+#define cpu_to_be8(x)	((uint8) (swap8 ((x))))
+
+#define be32_to_cpu(x)	((uint32)(swap32((x))))
+#define be16_to_cpu(x)	((uint16)(swap16((x))))
+#define be8_to_cpu(x)	((uint8) (swap8 ((x))))
+
+#endif
+
+
+/* Non endian dependent conversion functions */
+#define le32_to_be32(x)	((uint32)(swap32((x))))
+#define le16_to_be16(x)	((uint16)(swap16((x))))
+#define le8_to_be8(x)	((uint8) (swap8 ((x))))
+
+#define be32_to_le32(x)	((uint32)(swap32((x))))
+#define be16_to_le16(x)	((uint16)(swap16((x))))
+#define be8_to_le8(x)	((uint8) (swap8 ((x))))
+
+
+
+
+#endif  /* __ENDIAN_H__ */
