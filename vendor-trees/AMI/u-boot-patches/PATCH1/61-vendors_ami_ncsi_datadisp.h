--- uboot.org/vendors/ami/ncsi/datadisp.h	1969-12-31 19:00:00.000000000 -0500
+++ uboot/vendors/ami/ncsi/datadisp.h	2007-08-02 14:45:48.000000000 -0400
@@ -0,0 +1,37 @@
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
+#ifndef REX_DBG_DATADISP_H
+#define REX_DBG_DATADISP_H
+#include "config.h"
+#include "types.h"
+
+#define FORMAT_BYTE		sizeof(UBYTE)
+#define FORMAT_WORD		sizeof(UWORD)
+#define FORMAT_DWORD	sizeof(UDWORD)
+
+void DumpContents(UINT32 Address, char *Buffer, UINT16 BufLen, UINT8 Format, void (*WaitFn)(void));
+
+#endif  
