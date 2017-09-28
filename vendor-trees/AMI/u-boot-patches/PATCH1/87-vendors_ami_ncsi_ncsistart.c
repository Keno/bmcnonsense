--- uboot-pristine/vendors/ami/ncsi/ncsistart.c	2009-05-14 10:29:19.000000000 -0700
+++ uboot-changed/vendors/ami/ncsi/ncsistart.c	2009-05-14 10:30:24.000000000 -0700
@@ -34,6 +34,9 @@
 extern void GetMACAddr(UINT8 *MACAddr);
 extern int  GetDefaultPort(void);
 
+extern int GetLinkStatus(char *devname);	// joe
+int UseNCSI=1;
+
 #define MAX_CHANNELS 4
 
 typedef struct
@@ -68,8 +71,17 @@
 	UINT8 Major;
 	int DefaultPort = 0;
 	int BlindEnable = 0;
+	unsigned char retry, failure;
 
-
+	// joe --- begin
+	// if eth0 has link status, then use eth0, skip eth1 for ncsi
+	if (GetLinkStatus("wpcm_eth0"))
+	{
+		UseNCSI=0;
+		return;
+	}
+	// joe --- end
+	
 	NetSetHandler (NCSIHandler);
 
 	/* Cleanup Channel Info */
@@ -104,48 +116,66 @@
 		if (NCSI_Issue_SelectPackage(PackageID,1) != 0)
 			continue;
 
-
+		failure=0;
 		/* Find the number of channels support by this packages */
-		for (ChannelID = 0; ChannelID < MAX_PACKAGE_ID; ChannelID++)
+		for (retry=0; retry<2; retry++)
 		{
-			/* Issue Cleear Init State  for each channel */
-			if (NCSI_Issue_ClearInitialState(PackageID,ChannelID) != 0)
-				break;
-
-			/* Get Version ID and verify it is > 1.0  */
-			if (NCSI_Issue_GetVersionID(PackageID,ChannelID,&Ver1,&Ver2) != 0)
-			{
-				printf("NCSI:%d.%d Get Version IDFailed\n",PackageID, ChannelID);	
-				continue;
-			}
-			Major = (Ver1 >> 24) & 0xFF;
-			if ((Major & 0xF0)== 0xF0)
-				Major = Major & 0x0F;
-			if (Major < 1)
+	//		for (ChannelID = 0; ChannelID < MAX_PACKAGE_ID; ChannelID++)
+			for (ChannelID = 0; ChannelID < 1; ChannelID++)
 			{
-				printf("NCSI:%d.%d Version(0x%08x) is < 1.0  Not supported\n",
+				/* Issue Cleear Init State  for each channel */
+				if (NCSI_Issue_ClearInitialState(PackageID,ChannelID) != 0)
+				{
+					failure=1;
+					break;
+				}
+
+				/* Get Version ID and verify it is > 1.0  */
+				if (NCSI_Issue_GetVersionID(PackageID,ChannelID,&Ver1,&Ver2) != 0)
+				{
+					printf("NCSI:%d.%d Get Version IDFailed\n",PackageID, ChannelID);
+					failure=1;	
+					//continue;
+					break;
+				}
+				
+				Major = (Ver1 >> 24) & 0xFF;
+				if ((Major & 0xF0)== 0xF0)
+					Major = Major & 0x0F;
+				if (Major < 1)
+				{
+					printf("NCSI:%d.%d Version(0x%08x) is < 1.0  Not supported\n",
 						PackageID, ChannelID,Ver1);
-				continue;
-			}
-
-			/* Get Capabilities and set ArbitSupport */
-			if (NCSI_Issue_GetCapabiliites(PackageID,ChannelID, &Caps) != 0)
-			{
-				printf("NCSI:%d.%d Get Capabilities Failed\n",PackageID, ChannelID);	
-				continue;
-			}
-			if (Caps & HW_ARBITRATION_SUPPORT)
-				ChannelInfo[TotalChannels].ArbitSupport = 1;
-			else
-				ChannelInfo[TotalChannels].ArbitSupport = 0;
-
-			ChannelInfo[TotalChannels].PackageID = PackageID;
-			ChannelInfo[TotalChannels].ChannelID = ChannelID;
-			printf("Found NC-SI at Package:Channel (%d:%d)\n", PackageID,ChannelID);
-			TotalChannels++;
-			// [joe] we only have 1 channel working, so found, then quit
-			break;	
-		}
+					failure=1;
+					//continue;
+					break;
+				}
+
+				/* Get Capabilities and set ArbitSupport */
+				if (NCSI_Issue_GetCapabiliites(PackageID,ChannelID, &Caps) != 0)
+				{
+					printf("NCSI:%d.%d Get Capabilities Failed\n",PackageID, ChannelID);	
+					failure=1;
+					break;
+					//continue;
+				}
+
+				if (Caps & HW_ARBITRATION_SUPPORT)
+					ChannelInfo[TotalChannels].ArbitSupport = 1;
+				else
+					ChannelInfo[TotalChannels].ArbitSupport = 0;
+
+				ChannelInfo[TotalChannels].PackageID = PackageID;
+				ChannelInfo[TotalChannels].ChannelID = ChannelID;
+				printf("Found NC-SI at Package:Channel (%d:%d)\n", PackageID,ChannelID);
+				TotalChannels++;
+				// [joe] we only have 1 channel working, so found, then quit
+				break;	
+			} // for ChannelID
+
+			// if no failure, then break, otherwise retry
+			if (!failure) break;
+		} // for retry
 		/* Deselect previusly selected package */
 		NCSI_Issue_DeSelectPackage(PackageID);
 	}
@@ -176,97 +206,130 @@
 		PackageID = ChannelInfo[i].PackageID;
 		ChannelID = ChannelInfo[i].ChannelID;
 
-		/* Issue Select Package with Hw Arbit Disable*/
-		if (NCSI_Issue_SelectPackage(PackageID,1) != 0)
+		failure=0;
+		for (retry=0; retry<2; retry++)
 		{
-			printf("NCSI:%d.%d Select Package Failed\n",PackageID, ChannelID);	
-			continue;
-		}
+			/* Issue Select Package with Hw Arbit Disable*/
+			if (NCSI_Issue_SelectPackage(PackageID,1) != 0)
+			{
+				printf("NCSI:%d.%d Select Package Failed\n",PackageID, ChannelID);
+				failure=1;	
+				//continue;
+				break;
+			}
 		
-		/* Issue a Reset Channel to clear all previous config */
+			/* Issue a Reset Channel to clear all previous config */
 #if RESET_CHANNEL_COMPLIANCE
-		if (NCSI_Issue_ResetChannel(PackageID,ChannelID) != 0)
+			if (NCSI_Issue_ResetChannel(PackageID,ChannelID) != 0)
 #else
-		if (NCSI_Issue_ChannelCommands(CMD_RESET_CHANNEL,PackageID,ChannelID) != 0)
+			if (NCSI_Issue_ChannelCommands(CMD_RESET_CHANNEL,PackageID,ChannelID) != 0)
 #endif
-		{
-			printf("NCSI:%d.%d Reset Channel Failed. Will issue a Clear Init State command and try again\n",PackageID, ChannelID);	
-		
-			/* Some NCSI controllers requires a ClearInitialState command before issuing ResetChannel command  */	
-			/* Issue Cleear Init State to enter into init state  */
-			if (NCSI_Issue_ClearInitialState(PackageID,ChannelID) != 0)
 			{
-				printf("NCSI:%d.%d Clear Init State Failed\n",PackageID, ChannelID);	
-				continue;
-			}
+				printf("NCSI:%d.%d Reset Channel Failed. Will issue a Clear Init State command and try again\n",PackageID, ChannelID);	
 		
+				/* Some NCSI controllers requires a ClearInitialState command before issuing ResetChannel command  */	
+				/* Issue Cleear Init State to enter into init state  */
+				if (NCSI_Issue_ClearInitialState(PackageID,ChannelID) != 0)
+				{
+					printf("NCSI:%d.%d Clear Init State Failed\n",PackageID, ChannelID);
+					failure=1;	
+					//continue;
+					break;
+				}
+
 			/* Issue a Reset Channel again to clear all previous config */
 #if RESET_CHANNEL_COMPLIANCE
-			if (NCSI_Issue_ResetChannel(PackageID,ChannelID) != 0)
+				if (NCSI_Issue_ResetChannel(PackageID,ChannelID) != 0)
 #else
-			if (NCSI_Issue_ChannelCommands(CMD_RESET_CHANNEL,PackageID,ChannelID) != 0)
+				if (NCSI_Issue_ChannelCommands(CMD_RESET_CHANNEL,PackageID,ChannelID) != 0)
 #endif
-			{
-				printf("NCSI:%d.%d Reset Channel Failed.\n",PackageID, ChannelID);	
-				continue;
+				{
+					printf("NCSI:%d.%d Reset Channel Failed.\n",PackageID, ChannelID);	
+					failure=1;
+					//continue;
+					break;
+				}
 			}
-		}
  
-		/* Issue Cleear Init State to enter into init state  */
-		if (NCSI_Issue_ClearInitialState(PackageID,ChannelID) != 0)
-		{
-			printf("NCSI:%d.%d Clear Init State Failed\n",PackageID, ChannelID);	
-			continue;
-		}
+			/* Issue Cleear Init State to enter into init state  */
+			if (NCSI_Issue_ClearInitialState(PackageID,ChannelID) != 0)
+			{
+				printf("NCSI:%d.%d Clear Init State Failed\n",PackageID, ChannelID);
+				failure=1;	
+				//continue;
+				break;
+			}
 
-		/* Setup MAC Address */
-		if (NCSI_Issue_SetMacAddress(PackageID,ChannelID,MACAddr,1,0) != 0)
-		{
-			printf("NCSI:%d.%d Set Mac Address Failed\n",PackageID, ChannelID);	
-			continue;
-		}
+			/* Setup MAC Address */
+			if (NCSI_Issue_SetMacAddress(PackageID,ChannelID,MACAddr,1,0) != 0)
+			{
+				printf("NCSI:%d.%d Set Mac Address Failed\n",PackageID, ChannelID);	
+				failure=1;
+				//continue;
+				break;
+			}
+#ifdef CFG_PROJ_ENABLE_VLAN_SUPPORT_YES
 	
-		/* Enable Broaccast filter */
-		if (NCSI_Issue_EnableBcastFilter(PackageID,ChannelID,1,1,1,1) != 0)
-		{
-			printf("NCSI:%d.%d Enable Bcast Filter Failed\n",PackageID, ChannelID);	
-			continue;
-		}
-		/* Setup AEN Messages */
-		if (NCSI_Issue_EnableAEN((UINT8)PackageID,(UINT8)ChannelID,1,0,0) != 0)
-		{
-			printf("NCSI:%d.%d Enable AEN Failed\n",PackageID, ChannelID);	
-			continue;
-		}
+			/* Enable VLANID filter */
+			if (NCSI_Issue_EnableVLAN(PackageID,ChannelID,VLAN_MODE_ANY_VLAN_NON_VLAN) != 0)
+			{
+				printf("NCSI:%d.%d VLANID filter enable is failed\n",PackageID, ChannelID);	
+				failure=1;
+				//continue;
+				break;
+			}
+			printf("NCSIstart: Enable VLAN in NCSI is completed \n");
+#endif
+			/* Enable Broaccast filter */
+			if (NCSI_Issue_EnableBcastFilter(PackageID,ChannelID,1,1,1,1) != 0)
+			{
+				printf("NCSI:%d.%d Enable Bcast Filter Failed\n",PackageID, ChannelID);	
+				failure=1;
+				//continue;
+				break;
+			}
+			/* Setup AEN Messages */
+			if (NCSI_Issue_EnableAEN((UINT8)PackageID,(UINT8)ChannelID,1,0,0) != 0)
+			{
+				printf("NCSI:%d.%d Enable AEN Failed\n",PackageID, ChannelID);
+				failure=1;	
+				//continue;
+				break;
+			}
 
-		/* Enable Auto Negotiation */
-		if (NCSI_Issue_SetLink((UINT8)PackageID,(UINT8)ChannelID,1,0,0) != 0)
-		{
-			printf("NCSI:%d.%d Set Link Failed\n",PackageID, ChannelID);	
+			/* Enable Auto Negotiation */
+			if (NCSI_Issue_SetLink((UINT8)PackageID,(UINT8)ChannelID,1,0,0) != 0)
+			{
+				printf("NCSI:%d.%d Set Link Failed\n",PackageID, ChannelID);	
+				failure=1;
 	// When Host driver is loaded, we cannot SetLink. 
 	// So don't quit on SetLink failure
 	//		continue;
-		}
-		else
-		{
-			/* Wait for one second for Set Link to complete */
-			udelay(1000*1000);
-		}
+			}
+			else
+			{
+				/* Wait for one second for Set Link to complete */
+				udelay(1000*1000);
+			}
 
 
-		/* Set this channel info is setup */
-		ChannelInfo[i].Valid = 1;
+			/* Set this channel info is setup */
+			ChannelInfo[i].Valid = 1;
 
-		/* Count the number of valid packages*/
-		if (PrevPackageID != PackageID)
-		{
-			ValidPackages++;
-			PrevPackageID = PackageID;
-		}
+			/* Count the number of valid packages*/
+			if (PrevPackageID != PackageID)
+			{
+				ValidPackages++;
+				PrevPackageID = PackageID;
+			}
 
-		/* Deselect previusly selected package */
-		NCSI_Issue_DeSelectPackage(PackageID);
-	}
+			/* Deselect previusly selected package */
+			NCSI_Issue_DeSelectPackage(PackageID);
+
+			// if no failure, then break;
+			if (failure==0) break;
+		} // retry loop
+	} // for TotalChannels
 
 	if (ValidPackages > 1)
 	{
