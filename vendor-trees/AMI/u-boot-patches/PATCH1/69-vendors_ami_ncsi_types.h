--- uboot.org/vendors/ami/ncsi/types.h	1969-12-31 19:00:00.000000000 -0500
+++ uboot/vendors/ami/ncsi/types.h	2007-08-02 13:10:49.000000000 -0400
@@ -0,0 +1,105 @@
+/*************************************************************************
+RexDebug  - Generic Debugger 
+
+Copyright (c) 2000-2005, Samvinesh Christopher
+Written by Samvinesh Christopher.  Not derived from licensed software.
+
+Permission is granted to anyone to use this software for any purpose 
+on any computer system, and to redistribute it freely,subject to the 
+following restrictions:
+
+1. The author is not responsible for the consequences of use of this 
+   software, no matter how awful, even if they arise from defects in 
+   it.
+
+2. The origin of this software must not be misrepresented, either by 
+   explicit claim or by omission.
+
+3. Altered versions must be plainly marked as such, and must not  be 
+   misrepresented as being the original software.
+
+4. This notice may not be removed or altered.
+
+5. Commercial use of this software is allowed freely subject to prior
+   notificaiton to the author.
+**************************************************************************/
+#ifndef REX_TYPES_H
+#define REX_TYPES_H
+
+/*
+  Removed these as the names don't reflect their type 
+  (signed/unsigned) explicilty. These are replaced with
+  equivalent type defines  prefixed with U (unsigned) 
+  and S (signed)
+*/
+	
+#if 1
+typedef unsigned char	BYTE;
+typedef unsigned short	WORD;
+typedef unsigned long	DWORD;
+typedef signed  char	INT8;
+typedef signed  short	INT16;
+typedef signed 	long	INT32;
+#endif
+
+typedef unsigned char	UBYTE;
+typedef signed   char	SBYTE;
+
+typedef unsigned short	UWORD;
+typedef signed   short	SWORD;
+
+typedef unsigned long	UDWORD;
+typedef signed   long	SDWORD;
+
+typedef unsigned char	UINT8;
+typedef signed   char	SINT8;
+
+typedef unsigned char	INT8U;
+typedef signed   char	INT8S;
+
+typedef unsigned short	UINT16;
+typedef signed   short	SINT16;
+
+typedef unsigned short	INT16U;
+typedef signed   short	INT16S;
+
+
+#ifdef WIN32
+	#include <windows.h>
+	/* The following are already defined by windows.h 
+       but in different form  as belows :
+			typedef unsigned  int	UINT32;
+			typedef signed	  int	INT32;
+	*/
+#else
+	typedef unsigned long	UINT32;
+	typedef signed   long	SINT32;
+
+	typedef unsigned long	INT32U;
+	typedef signed   long	INT32S;
+
+	typedef void *			HANDLE;
+	typedef void *			LPVOID;
+#endif
+
+typedef int				BOOL;
+#define FALSE           0
+#define TRUE            1
+
+/* Define Windows Socket Specific types and defines */
+#ifdef WIN32
+	#include <winsock.h>
+#else
+	typedef int				SOCKET;
+	typedef UBYTE *			LPBYTE;
+	#define INVALID_SOCKET	-1
+	#define SOCKET_ERROR	-1
+	#define closesocket		close
+	#define WINAPI
+	typedef char			TCHAR;
+	typedef UBYTE		INT4U;
+#endif
+
+
+
+#endif		/* REX_TYPES_H*/
