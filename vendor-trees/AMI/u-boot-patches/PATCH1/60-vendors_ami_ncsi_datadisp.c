--- uboot.org/vendors/ami/ncsi/datadisp.c	1969-12-31 19:00:00.000000000 -0500
+++ uboot/vendors/ami/ncsi/datadisp.c	2007-08-02 14:47:29.000000000 -0400
@@ -0,0 +1,126 @@
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
+#if 0
+#include <stdio.h>
+#include <ctype.h>
+#include "config.h"
+#include "types.h"
+#include "console.h"
+#include "datadisp.h"
+#else
+#include <common.h>
+#include <exports.h>
+#include "types.h"
+#include "datadisp.h"
+#define BYTES_PER_LINE 16
+#define ConsolePrintf printf
+#define isprint(x) (((x) >=32) && ((x) < 127))
+#endif
+
+void
+DumpContents(UINT32 Address, char *Buffer, UINT16 BufLen, UINT8 Format, void (*WaitFn)(void))
+{
+	UINT32 Index;		/* Index to the Buffer */
+	UINT8  x;			/* No of Bytes written in a line */
+	char   charBuf[BYTES_PER_LINE+1];
+	UINT32 Data;
+	
+	/* Reduce BufLen to nearest Format boundary */
+	BufLen = (BufLen/Format) *Format;	
+
+	/* Null terminate the character buffer */
+	charBuf[BYTES_PER_LINE] = 0;
+
+	/* Initialize No of Bytes written in line to zero */
+	x = 0;
+
+	
+	for (Index=0;Index<BufLen;Index+=Format)
+	{
+		/* Beginning of New Line. Write Address */
+		if (x == 0)
+			ConsolePrintf("0x%08lX : ",Address);
+
+
+		/* Print the Value according to the format */
+		
+		switch (Format)
+		{
+			case FORMAT_BYTE:
+				Data = (UINT32)(*((UINT8 *)(Buffer+Index)));
+				ConsolePrintf("%02X",(UINT8)Data);
+				break;
+			case FORMAT_WORD:
+				Data = (UINT32)(*((UINT16 *)(Buffer+Index)));
+				ConsolePrintf("%04X",(UINT16)Data);
+				break;
+			case FORMAT_DWORD:
+				Data = (UINT32)(*((UINT32 *)(Buffer+Index)));
+				ConsolePrintf("%08lX",(UINT32)Data);
+				break;
+			default:
+				Data = 0;
+				ConsolePrintf("INTERNAL ERROR: Unknown Format type in Data Display\n");
+				break;
+		}			
+
+		/* Add character to Character Buffer */
+		if (Format == FORMAT_BYTE)
+		{
+			if (isprint((UINT8)Data))
+				charBuf[x] = (UINT8)(Data);
+			else
+				charBuf[x] = '.';
+		}
+
+		/* Increment number of bytes written */
+		x+=Format;
+		
+		/* Print Spaces between every data and an '-' in the middle of line */
+		if (x == (BYTES_PER_LINE/2))
+			ConsolePrintf(" - ");
+		else
+			ConsolePrintf(" ");
+
+		/* If end of line */
+		if (x == BYTES_PER_LINE)
+		{
+			if (Format == FORMAT_BYTE)
+				ConsolePrintf(  "%s",charBuf);		/* Print Character Bytes	*/
+			ConsolePrintf("\n");					/* Begin a new Line			*/
+			x = 0;							/* Reset No of bytes in line*/
+			Address+=BYTES_PER_LINE;		/* Increment Address		*/
+		}
+
+		if (WaitFn)
+			(*WaitFn)();
+
+	}  /* for */
+	if (x!=0)
+		ConsolePrintf("\n");	
+	return ;
+}
+
