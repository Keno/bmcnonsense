--- olduboot/vendors/ami/ncsi/ncsistart.c	1969-12-31 19:00:00.000000000 -0500
+++ uboot/vendors/ami/ncsi/ncsistart.c	2008-07-31 19:51:09.000000000 -0400
@@ -0,0 +1,395 @@
+/****************************************************************
+ ****************************************************************
+ **                                                            **
+ **    (C)Copyright 2005-2006, American Megatrends Inc.        **
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
+
+  Author	: Samvinesh Christopher
+
+  Module	: NCSI Startup code.
+		  Calls the necessary NCSI functions to enable
+		  NCSI Pass thru.
+			
+  Revision	: 1.0  
+
+  Changelog : 1.0 - Initial Version [SC]
+
+*****************************************************************/
+#include <common.h>
+#ifdef CONFIG_NCSI_SUPPORT
+#include <exports.h>
+#include <ncsi.h>
+
+#define RESET_CHANNEL_COMPLIANCE 1
+
+extern void GetMACAddr(UINT8 *MACAddr);
+extern int  GetDefaultPort(void);
+
+#define MAX_CHANNELS 4
+
+typedef struct
+{	
+	UINT8 Valid;
+	UINT8 PackageID;
+	UINT8 ChannelID;
+	UINT8 ArbitSupport;
+//	UINT32 Caps;
+//	UINT32 BcastCaps;
+//	UINT32 McastCaps;
+//	UINT32 AENCaps;
+	/* Add Vlan if needed */
+} CHANNEL_INFO;
+
+
+static CHANNEL_INFO ChannelInfo[MAX_CHANNELS];
+
+
+void
+NCSI_Start(void)
+{
+	UINT8 PrevPackageID;
+	UINT8 PackageID;
+	UINT8 ChannelID;
+	UINT8 MACAddr[6];
+	int saved_verbose;
+	int i;
+	int ValidPackages =0;
+	int TotalChannels =0;
+	UINT32 Caps,Ver1,Ver2;
+	UINT8 Major;
+	int DefaultPort = 0;
+	int BlindEnable = 0;
+
+
+	NetSetHandler (NCSIHandler);
+
+	/* Cleanup Channel Info */
+	for (i=0;i<MAX_CHANNELS;i++)
+		ChannelInfo[i].Valid = 0;
+
+	/* Get MAC Address to use */
+	GetMACAddr(MACAddr);
+	if ((MACAddr[0] == 0) && (MACAddr[1] == 0) && (MACAddr[2] == 0) &&
+	    (MACAddr[3] == 0) && (MACAddr[4] == 0) && (MACAddr[5] == 0))
+	{
+		printf("NCSI: Error! Mac Address is 0.\n");
+		//printf("NCSI: Error! Mac Address is 0. Cannot enable NCSI\n");
+		//return;
+	}
+
+	/* Disable Verbose messages */
+#ifdef  NCSI_DEBUG
+	saved_verbose = EnableVerbose(DUMP_BUFFER| SHOW_MESSAGES);
+#else
+	saved_verbose = EnableVerbose(0);
+#endif
+
+	/* Blindly deselect all  packages */
+	for (PackageID = 0; PackageID < MAX_PACKAGE_ID; PackageID++)
+		NCSI_Issue_DeSelectPackage(PackageID);
+
+	/* Discover Packages and Channels */
+	for (PackageID = 0; PackageID < MAX_PACKAGE_ID; PackageID++)
+	{
+		/* Issue Select Package with Hw Arbit Disable*/
+		if (NCSI_Issue_SelectPackage(PackageID,1) != 0)
+			continue;
+
+
+		/* Find the number of channels support by this packages */
+		for (ChannelID = 0; ChannelID < MAX_PACKAGE_ID; ChannelID++)
+		{
+			/* Issue Cleear Init State  for each channel */
+			if (NCSI_Issue_ClearInitialState(PackageID,ChannelID) != 0)
+				break;
+
+			/* Get Version ID and verify it is > 1.0  */
+			if (NCSI_Issue_GetVersionID(PackageID,ChannelID,&Ver1,&Ver2) != 0)
+			{
+				printf("NCSI:%d.%d Get Version IDFailed\n",PackageID, ChannelID);	
+				continue;
+			}
+			Major = (Ver1 >> 24) & 0xFF;
+			if ((Major & 0xF0)== 0xF0)
+				Major = Major & 0x0F;
+			if (Major < 1)
+			{
+				printf("NCSI:%d.%d Version(0x%08x) is < 1.0  Not supported\n",
+						PackageID, ChannelID,Ver1);
+				continue;
+			}
+
+			/* Get Capabilities and set ArbitSupport */
+			if (NCSI_Issue_GetCapabiliites(PackageID,ChannelID, &Caps) != 0)
+			{
+				printf("NCSI:%d.%d Get Capabilities Failed\n",PackageID, ChannelID);	
+				continue;
+			}
+			if (Caps & HW_ARBITRATION_SUPPORT)
+				ChannelInfo[TotalChannels].ArbitSupport = 1;
+			else
+				ChannelInfo[TotalChannels].ArbitSupport = 0;
+
+			ChannelInfo[TotalChannels].PackageID = PackageID;
+			ChannelInfo[TotalChannels].ChannelID = ChannelID;
+			printf("Found NC-SI at Package:Channel (%d:%d)\n", PackageID,ChannelID);
+			TotalChannels++;
+			// [joe] we only have 1 channel working, so found, then quit
+			break;	
+		}
+		/* Deselect previusly selected package */
+		NCSI_Issue_DeSelectPackage(PackageID);
+	}
+
+	/* Get the Default Port and Validate it */
+	DefaultPort = GetDefaultPort();
+	if ((DefaultPort > 0) && (DefaultPort <= TotalChannels))
+		BlindEnable = 1;
+
+#ifdef CONFIG_NCSI_FORCE_DEFAULT_PORT
+	/* If the env default port is not valid, try the default port if any */
+	if (!BlindEnable)
+	{
+		DefaultPort=CONFIG_NCSI_FORCE_DEFAULT_PORT;
+		if ((DefaultPort > 0) && (DefaultPort <= TotalChannels))
+			BlindEnable = 1;
+	}
+#endif
+			
+
+
+
+	PrevPackageID = -1;
+	/* Configure the detected channels */
+	for(i=0;i<TotalChannels;i++)
+	{
+
+		PackageID = ChannelInfo[i].PackageID;
+		ChannelID = ChannelInfo[i].ChannelID;
+
+		/* Issue Select Package with Hw Arbit Disable*/
+		if (NCSI_Issue_SelectPackage(PackageID,1) != 0)
+		{
+			printf("NCSI:%d.%d Select Package Failed\n",PackageID, ChannelID);	
+			continue;
+		}
+		
+		/* Issue a Reset Channel to clear all previous config */
+#if RESET_CHANNEL_COMPLIANCE
+		if (NCSI_Issue_ResetChannel(PackageID,ChannelID) != 0)
+#else
+		if (NCSI_Issue_ChannelCommands(CMD_RESET_CHANNEL,PackageID,ChannelID) != 0)
+#endif
+		{
+			printf("NCSI:%d.%d Reset Channel Failed. Will issue a Clear Init State command and try again\n",PackageID, ChannelID);	
+		
+			/* Some NCSI controllers requires a ClearInitialState command before issuing ResetChannel command  */	
+			/* Issue Cleear Init State to enter into init state  */
+			if (NCSI_Issue_ClearInitialState(PackageID,ChannelID) != 0)
+			{
+				printf("NCSI:%d.%d Clear Init State Failed\n",PackageID, ChannelID);	
+				continue;
+			}
+		
+			/* Issue a Reset Channel again to clear all previous config */
+#if RESET_CHANNEL_COMPLIANCE
+			if (NCSI_Issue_ResetChannel(PackageID,ChannelID) != 0)
+#else
+			if (NCSI_Issue_ChannelCommands(CMD_RESET_CHANNEL,PackageID,ChannelID) != 0)
+#endif
+			{
+				printf("NCSI:%d.%d Reset Channel Failed.\n",PackageID, ChannelID);	
+				continue;
+			}
+		}
+ 
+		/* Issue Cleear Init State to enter into init state  */
+		if (NCSI_Issue_ClearInitialState(PackageID,ChannelID) != 0)
+		{
+			printf("NCSI:%d.%d Clear Init State Failed\n",PackageID, ChannelID);	
+			continue;
+		}
+
+		/* Setup MAC Address */
+		if (NCSI_Issue_SetMacAddress(PackageID,ChannelID,MACAddr,1,0) != 0)
+		{
+			printf("NCSI:%d.%d Set Mac Address Failed\n",PackageID, ChannelID);	
+			continue;
+		}
+	
+		/* Enable Broaccast filter */
+		if (NCSI_Issue_EnableBcastFilter(PackageID,ChannelID,1,1,1,1) != 0)
+		{
+			printf("NCSI:%d.%d Enable Bcast Filter Failed\n",PackageID, ChannelID);	
+			continue;
+		}
+		/* Setup AEN Messages */
+		if (NCSI_Issue_EnableAEN((UINT8)PackageID,(UINT8)ChannelID,1,0,0) != 0)
+		{
+			printf("NCSI:%d.%d Enable AEN Failed\n",PackageID, ChannelID);	
+			continue;
+		}
+
+		/* Enable Auto Negotiation */
+		if (NCSI_Issue_SetLink((UINT8)PackageID,(UINT8)ChannelID,1,0,0) != 0)
+		{
+			printf("NCSI:%d.%d Set Link Failed\n",PackageID, ChannelID);	
+	// When Host driver is loaded, we cannot SetLink. 
+	// So don't quit on SetLink failure
+	//		continue;
+		}
+		else
+		{
+			/* Wait for one second for Set Link to complete */
+			udelay(1000*1000);
+		}
+
+
+		/* Set this channel info is setup */
+		ChannelInfo[i].Valid = 1;
+
+		/* Count the number of valid packages*/
+		if (PrevPackageID != PackageID)
+		{
+			ValidPackages++;
+			PrevPackageID = PackageID;
+		}
+
+		/* Deselect previusly selected package */
+		NCSI_Issue_DeSelectPackage(PackageID);
+	}
+
+	if (ValidPackages > 1)
+	{
+		printf("Multiple Packages Found\n");
+		for(i=0;i<TotalChannels;i++)
+		{
+			if (ChannelInfo[i].Valid == 0)
+				continue;
+			if (ChannelInfo[i].ArbitSupport == 0)
+			{
+				PackageID = ChannelInfo[i].PackageID;
+				ChannelID = ChannelInfo[i].ChannelID;
+				printf("WARNING: Channel (%d:%d) does not support Arbitration.Disabled Channel\n");
+			}	
+		}
+	}
+
+	/* Enable the Valid channels */
+	for(i=0;i<TotalChannels;i++)
+	{
+		if (ChannelInfo[i].Valid == 0)
+			continue;
+
+		PackageID = ChannelInfo[i].PackageID;
+		ChannelID = ChannelInfo[i].ChannelID;
+
+		if (BlindEnable)
+		{
+			DefaultPort--;
+			if (DefaultPort != 0)
+				continue;
+			if (EnableChannel(PackageID,ChannelID) != 0)
+			{
+				printf("ERROR: Unable to Enable Package:Channel (%d:%d)\n",
+															PackageID,ChannelID);
+				break;
+			}
+		}
+		else
+		{
+			if (EnableChannelOnLink(PackageID,ChannelID) != 0)
+				continue;
+		}
+
+		/* Only one channel should be enable at a time */
+		printf("NC-SI Enabled for Package:Channel (%d:%d)\n",PackageID,ChannelID);
+		break;
+	}
+	EnableVerbose(saved_verbose);
+	return;
+}
+
+
+int
+EnableChannelOnLink(UINT8 PackageID,UINT8 ChannelID)
+{
+
+	UINT32 LinkStatus;
+
+	/* Issue Select Package with Hw Arbit Enable */
+	if (NCSI_Issue_SelectPackage(PackageID,0) != 0)
+	{
+		printf("NCSI:%d.%d Select Package (Hw Arbit Enable)Failed\n",PackageID, ChannelID);	
+		return 1;
+	}
+
+	/* Get Link Status to check if Channel can be enabled */	
+	if (NCSI_Issue_GetLinkStatus((UINT8)PackageID,(UINT8)ChannelID,&LinkStatus) != 0)
+	{
+		printf("NCSI:%d.%d Link Status Failed\n",PackageID, ChannelID);	
+		return 1;
+	}
+
+	if (!(LinkStatus & 0x01))
+	{
+		printf("NCSI:%d.%d Link is down\n",PackageID,ChannelID);
+		//return 1;
+	}
+	else
+		printf("NCSI:%d.%d Link Status = 0x%08X\n",PackageID,ChannelID,LinkStatus);
+
+	/* Issue an Enable TX for the channel */
+	if (NCSI_Issue_ChannelCommands(CMD_ENABLE_CHANNEL_TX,PackageID,ChannelID) != 0)
+	{
+		printf("NCSI:%d.%d Enable Channel Tx Failed\n",PackageID, ChannelID);	
+		return 1;
+	}
+		
+	/* Enable the channel */
+	if (NCSI_Issue_ChannelCommands(CMD_ENABLE_CHANNEL,PackageID,ChannelID) != 0)
+	{
+		printf("NCSI:%d.%d Enable Channel Failed\n",PackageID, ChannelID);	
+		return 1;
+	}
+
+	return 0;
+}
+
+int
+EnableChannel(UINT8 PackageID,UINT8 ChannelID)
+{
+
+	/* Issue Select Package with Hw Arbit Enable */
+	if (NCSI_Issue_SelectPackage(PackageID,0) != 0)
+	{
+		printf("NCSI:%d.%d Select Package (Hw Arbit Enable)Failed\n",PackageID, ChannelID);	
+		return 1;
+	}
+
+	/* Issue an Enable TX for the channel */
+	if (NCSI_Issue_ChannelCommands(CMD_ENABLE_CHANNEL_TX,PackageID,ChannelID) != 0)
+	{
+		printf("NCSI:%d.%d Enable Channel Tx Failed\n",PackageID, ChannelID);	
+		return 1;
+	}
+		
+	/* Enable the channel */
+	if (NCSI_Issue_ChannelCommands(CMD_ENABLE_CHANNEL,PackageID,ChannelID) != 0)
+	{
+		printf("NCSI:%d.%d Enable Channel Failed\n",PackageID, ChannelID);	
+		return 1;
+	}
+
+	return 0;
+}
+#endif
