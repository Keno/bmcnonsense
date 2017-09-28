--- uboot/vendors/ami/fmh/cmd_fmh.c	1969-12-31 19:00:00.000000000 -0500
+++ uboot.new/vendors/ami/fmh/cmd_fmh.c	2007-11-20 16:19:01.000000000 -0500
@@ -0,0 +1,606 @@
+# include <common.h>
+# include <config.h>
+# include <command.h>
+# include <flash.h>
+# include <i2c.h>
+# include "cmd_fmh.h"
+# include "fmh.h"
+# include "cmd.h"
+
+
+# define  KERNEL_LOADADDR	0xC00000
+# define  KERNEL_FILE		"/boot/uImage"
+
+
+/* Note: Currently FMH support is done only for the first bank */
+extern flash_info_t flash_info[CFG_MAX_FLASH_BANKS];
+
+extern short ReadJumperConfig(void);
+extern int rconsole(void);
+extern int do_protect (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]);
+
+#ifdef CFG_YAFU_SUPPORT
+extern int fwupdate(void);
+#endif
+
+
+/* U-boot's cmd function to list FMH */
+int
+do_fmh (cmd_tbl_t *cmdtp, int flag , int argc, char *argv[])
+{
+	return ListFMH();
+}
+
+U_BOOT_CMD(
+	fmh,	1,	1,	do_fmh,
+	"fmh     - List the Flash Module Headers\n",
+	"    - List the Flash Module Headers\n"
+);
+
+/* U-boot's cmd function to boot using FMH  Has one argument with
+ * the values from 0 to 2.  It is  either  0 (OS), 1 (DIAG), 2 (RECOVERY)
+ */
+int
+do_bootfmh(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
+{
+	short PathID;
+
+	if (argc == 1)
+	{
+		/* Get the BootPath ID from the Hardware configuration Jumper */
+		PathID = ReadJumperConfig();
+
+		/* If Read Jumper Config not supported, boot to OS */
+		if (PathID == -1)
+			PathID = 0;
+	}
+	else
+	{
+		/* Get the BootPath ID from the Argument */
+		PathID = (unsigned short)simple_strtoul(argv[1],NULL,0);
+	}
+
+	if (PathID > 4)
+	{
+		printf("Usage : bootfmh <Arg> . Arg = 0 to 4\n");
+		return 1;
+	}
+
+	return BootFMH((unsigned short)1 << PathID);
+}
+
+U_BOOT_CMD(bootfmh,	2,	0,	do_bootfmh,
+	"bootfmh - Boot using FMH Modules.\n",
+	"    - Boot using FMH Modules. Arg = 0 to 2\n"
+);
+
+int
+do_bootmenu (cmd_tbl_t *cmdtp, int flag , int argc, char *argv[])
+{
+	unsigned short PathID = 5;
+	unsigned char *mac;
+	
+	while (PathID >= 5)
+	{
+    	mac = getenv("ethaddr");
+		if (strncmp(mac,"00:00:00:00:00:00",17) == 0)
+			printf("WARNING: Ethernet MAC Address is not set\n");
+		printf("------ Boot Options-------\n");
+		printf("\t0. Normal Boot\n");
+		printf("\t1. Diagnostics\n");
+		printf("\t2. Remote Recovery\n");
+		printf("\t3. Management Console\n");
+		printf("\t4. Raw Console\n");
+		printf("Select Boot Option:\n");
+		PathID = getc() - '0';
+		
+	}
+	return BootFMH((unsigned short)1 << PathID);
+}
+
+U_BOOT_CMD(bootmenu,	1,	1,	do_bootmenu,
+	"bootmenu- Show Boot Path menu.\n",
+	"   - Show Boot Path menu\n"
+);
+
+
+/* Actual function implementing FMH Listing*/
+/* Returns 0 on success, else 1 on failure */
+int
+ListFMH(void)
+{
+	flash_info_t *flinfo;
+	unsigned long SectorSize;
+	unsigned long SectorCount;
+	long SkipSize;
+	int i,bank;
+	FMH	*fmh;
+	MODULE_INFO *mod;
+	unsigned char Name[9];
+
+	printf("Listing FMH Modules\n");
+
+//	printf("Flash Size : %ld KB \n",flinfo->size >> 10);
+//	printf("Sector Count : %ld \n",SectorCount);
+
+	for (bank=0;bank<CFG_MAX_FLASH_BANKS;bank++)
+	{
+		flinfo = &flash_info[bank];
+		SectorCount = flinfo->sector_count;
+		if (flinfo->size == 0)
+			break;
+
+		for (i=0;i<SectorCount;)
+		{
+			if (i == (SectorCount-1))
+				SectorSize = flinfo->size - flinfo->start[i];
+			else
+				SectorSize = flinfo->start[i+1]-flinfo->start[i];
+
+			fmh = ScanforFMH((unsigned char *)flinfo->start[i],SectorSize);
+			if (fmh == NULL)
+			{
+				i++;
+				continue;
+			}
+
+			/* Print Information about the FMH and its module */
+			printf("\nFMH Located at 0x%08lx of Size 0x%08x\n",
+					(unsigned long)flinfo->start[i],le32_to_host(fmh->FMH_AllocatedSize));
+
+			mod = &(fmh->Module_Info);
+			strncpy(Name,mod->Module_Name,8);
+			Name[8]=0;
+			printf("Name    : %s\n",Name);
+			printf("Ver     : %d.%d\n",mod->Module_Ver_Major,mod->Module_Ver_Minor);
+			printf("Type 	: 0x%04x\n",le16_to_host(mod->Module_Type));
+			printf("Flags	: 0x%04x\n",le16_to_host(mod->Module_Flags));
+			printf("Size 	: 0x%08x\n",le32_to_host(mod->Module_Size));
+			printf("Location: 0x%08lx\n",flinfo->start[i] +
+								le32_to_host(mod->Module_Location));
+			printf("LoadAddr: 0x%08x\n",le32_to_host(mod->Module_Load_Address));
+
+			/* Validate CheckSum */
+			if (le16_to_host(mod->Module_Flags) & MODULE_FLAG_VALID_CHECKSUM)
+			{
+				if (CalculateCRC32((unsigned char *)flinfo->start[i]+
+								le32_to_host(mod->Module_Location),
+								le32_to_host(mod->Module_Size))
+								== le32_to_host(mod->Module_Checksum))
+					printf("CheckSum: Valid\n");
+				else
+					printf("CheckSum: Invalid\n");
+			}
+			else
+					printf("CheckSum: Not Computed\n");
+
+
+
+			printf("--------------------------------------------------\n");
+
+
+			/* Skip the Sectors occupied by the Module */
+			SkipSize = le32_to_host(fmh->FMH_AllocatedSize);
+			while ((SkipSize > 0) && (i < SectorCount))
+			{
+				if (i == (SectorCount-1))
+					SectorSize= flinfo->size - flinfo->start[i];
+				else
+					SectorSize = flinfo->start[i+1]-flinfo->start[i];
+				SkipSize-=SectorSize;
+				i++;
+			}
+		} /* sector */
+	} /* bank */
+
+	return 0;
+}
+
+/****************************************************************************/
+extern int gDefaultEnvironment;
+
+int
+BootFMH(unsigned short PathID)
+{
+	flash_info_t *flinfo;
+	unsigned long SectorSize;
+	unsigned long SectorCount;	
+	long SkipSize;
+	int i,bank;
+	FMH	*fmh;
+	MODULE_INFO *mod;
+	unsigned char Name[9];
+	unsigned long ExecuteAddr = 0xFFFFFFFF;
+	unsigned short ExecuteType = MODULE_BOOTLOADER;
+	unsigned short Compression;
+	int retval;
+	char *argv[4];
+	char Command[] = "FMHEXECUTE";
+	char AddrStr[] = "0xFFFFFFFF";
+	int  silent = 0;
+
+#ifdef CFG_YAFU_SUPPORT
+	int  boot_fwupd = 0;
+#endif
+
+	char *s;
+	int MtdPartition = 1;		/* mtdblock0 is used for full flash */
+	int RootMtdNo = 0;
+	char bootargs[256];
+	int rootisinitrd = 0; /*indicates root is actually an initrd*/
+	unsigned long initrd_address = 0xFFFFFFFF;
+	char INITRDAddrStr[] = "0xFFFFFFFF";
+	char baudrate_str[16];
+
+	sprintf(baudrate_str,"%ld",CONFIG_BAUDRATE);
+
+#ifdef CFG_YAFU_SUPPORT
+	s = getenv("boot_fwupd");
+	if (s != NULL)
+	{
+		boot_fwupd = (int)simple_strtoul(s,NULL,0);
+		printf("boot_fwupd is %d\n",boot_fwupd);
+	}
+
+	/* Check the variable to find out if we need to go into fwupdate mode or not */
+	if(boot_fwupd)
+	{
+		/* If variable is set, then clear it and call fwupdate() */
+		setenv("boot_fwupd", "0");
+		setenv("mode", "1");   
+		fwupdate();
+	}
+#endif
+	if (gDefaultEnvironment == 1)
+	{
+		gDefaultEnvironment = 0;
+		saveenv ();
+	}
+
+	/* silentboot = 0 (Verbose) 2 (Progress bar) 1 (Quiet) */
+	s = getenv("silent");
+	if (s != NULL)
+		silent = (int)simple_strtoul(s,NULL,0);
+
+	if (PathID == 0x04)
+	{
+		return rconsole();	/* Remote Recovery Console */
+	}
+	if (PathID == 0x08)
+	{
+		return 0;			/* Management Console (Restricted Version of Uboot) */
+	}
+	if (PathID == 0x10)
+	{
+		return 0;			/* Full Access to Uboot Console */
+	}
+	
+
+	for (bank=0;bank<CFG_MAX_FLASH_BANKS;bank++)
+	{
+		flinfo = &flash_info[bank];
+		SectorCount = flinfo->sector_count;
+		if (flinfo->size == 0)
+			break;
+
+		/* Scan through the flash for FMH */
+		for (i=0;i<SectorCount;)
+		{
+			/* Check if FMH found at this sector */
+			if (i == (SectorCount-1))
+				SectorSize = flinfo->size - flinfo->start[i];
+			else
+				SectorSize = flinfo->start[i+1]-flinfo->start[i];
+			fmh = ScanforFMH((unsigned char *)flinfo->start[i],SectorSize);
+			if (fmh == NULL)
+			{
+				i++;
+				continue;
+			}
+
+			/* Extract Module information and module name */
+			mod = &(fmh->Module_Info);
+			strncpy(Name,mod->Module_Name,8);
+			Name[8]=0;
+
+			/* Check if Path ID Matches */
+			if (!(le16_to_host(mod->Module_Flags) & PathID))
+			{
+				goto Skip;
+				continue;
+			}
+
+			/* Check if module checksum to be validated */
+			if (le16_to_host(mod->Module_Flags) & MODULE_FLAG_VALID_CHECKSUM)
+			{
+				if (CalculateCRC32((unsigned char *)flinfo->start[i]+
+								le32_to_host(mod->Module_Location),
+								le32_to_host(mod->Module_Size))
+								!= le32_to_host(mod->Module_Checksum))
+				{
+					printf("CheckSum is Invalid. Ignoring module %s \n",Name);
+					goto Skip;
+					continue;
+				}
+			}
+
+			/*Added by subash..one more possibility is we have a cramfs
+			image that is actually a initrd MODULE_INITRD_CRAMFS*/
+			if ((le16_to_host(mod->Module_Type) == MODULE_JFFS2) ||
+				(le16_to_host(mod->Module_Type) == MODULE_JFFS) ||
+				(le16_to_host(mod->Module_Type) == MODULE_INITRD_CRAMFS))
+			{
+				if (strnicmp(mod->Module_Name,"ROOT",8) == 0)
+				{
+					if(le16_to_host(mod->Module_Type) == MODULE_INITRD_CRAMFS)
+					{
+						rootisinitrd = 1; //found root and it is initrd
+					}
+
+					RootMtdNo = MtdPartition;
+
+					if(rootisinitrd == 0)
+					{
+						printf("Found Root File System @ /dev/mtdblock%d\n",RootMtdNo);
+					}
+					else
+					{
+						initrd_address = flinfo->start[i] + le32_to_host(mod->Module_Location);
+						printf("Found INITRD Image for Root File System @ /dev/mtdblock%d flash address %lx\n",RootMtdNo,initrd_address);
+					}
+				}
+
+				printf("%s @ /dev/mtdblock%d Address %x\n",mod->Module_Name, MtdPartition, flinfo->start[i] + le32_to_host(mod->Module_Location));
+
+				MtdPartition++;
+			}
+
+
+			/* Calculate the compresion Type */
+			Compression = le16_to_host(mod->Module_Flags);
+	    	Compression &= MODULE_FLAG_COMPRESSION_MASK;
+			Compression >>= MODULE_FLAG_COMPRESSION_LSHIFT;
+
+
+
+			/* If module to be loaded to memory */
+			if (le16_to_host(mod->Module_Flags) & MODULE_FLAG_COPY_TO_RAM)
+			{
+				/* TODO: If compression is enabled, decompress to ram */
+				if (Compression == MODULE_COMPRESSION_NONE)
+				{
+					printf("Copying Module of size 0x%lx From 0x%lx to 0x%lx\n",
+									le32_to_host(mod->Module_Size),
+									flinfo->start[i]+le32_to_host(mod->Module_Location),
+									le32_to_host(mod->Module_Load_Address));
+
+					flash_read( /*addr,cnt,dest */
+			   				(ulong)(flinfo->start[i] + le32_to_host(mod->Module_Location)),
+							 le32_to_host(mod->Module_Size),
+							(char *)le32_to_host(mod->Module_Load_Address)
+						  );
+
+					if (strnicmp(mod->Module_Name,"ROOT",8) == 0)
+					{
+						if(rootisinitrd == 1)
+						{
+							initrd_address =  le32_to_host(mod->Module_Load_Address);
+							printf("INITRD Image moved to 0x%lx\n",initrd_address);
+						}
+					}
+				}
+				else
+				{
+					/* TODO: If compression is enabled, decompress to ram */
+					printf("ERROR: Compression support not present in U-Boot\n");
+				}
+			}
+
+			/* If no other previous module to be executed and this module to
+			 * be executed. Save the ExecuteAddr for later execution */
+			if ((le16_to_host(mod->Module_Flags) & MODULE_FLAG_EXECUTE)
+							&& 	(ExecuteAddr == 0xFFFFFFFF))
+			{
+				ExecuteType =le16_to_host(mod->Module_Type);
+				if (le16_to_host(mod->Module_Flags) & MODULE_FLAG_COPY_TO_RAM)
+					ExecuteAddr =le32_to_host(mod->Module_Load_Address);
+				else
+				{
+					if (Compression == MODULE_COMPRESSION_NONE)
+						ExecuteAddr = flinfo->start[i] +
+											le32_to_host(mod->Module_Location);
+					else
+						printf("ERROR: Compressed Module cannot be XIP\n");
+				}
+			}
+
+			/* Skip the Sectors occupied by the Module */
+Skip:
+			SkipSize = le32_to_host(fmh->FMH_AllocatedSize);
+			while ((SkipSize > 0) && (i < SectorCount))
+			{
+				if (i == (SectorCount-1))
+					SectorSize= flinfo->size - flinfo->start[i];
+				else
+					SectorSize = flinfo->start[i+1]-flinfo->start[i];
+				SkipSize-=SectorSize;
+				i++;
+			}
+
+		}	/* For Scan */
+	} /* bank */
+
+	/* Create the bootarguments */
+	if (RootMtdNo > 0)
+	{
+		// Some flash does auto protect on powerup. Linux MTD as of now will return 
+        // error on writes to protected blocks. This will create problems in JFFS2 
+		// writes. So unprotect all blocks before booting to linux
+		argv[0] = &Command[0];
+		argv[1] = "off";
+		argv[2] = "all";
+		argv[3] = NULL;
+		do_protect (NULL,0,3,argv);
+
+		//we found root mtd.now we see if it is initrd
+		if(rootisinitrd == 1)
+		{
+			sprintf(bootargs,"root=/dev/ramdisk ro ip=none ramdisk_blocksize=4096 ");
+		}
+		else
+		{
+			sprintf(bootargs,"root=/dev/mtdblock%d ro ip=none ",RootMtdNo);
+		}
+
+		if (silent > 1)
+		{
+			strcat(bootargs,"console=");
+#ifdef CFG_PROJ_CONSOLE_TTY
+			strcat(bootargs,CFG_PROJ_CONSOLE_TTY);
+#else
+			strcat(bootargs,"ttyS0");
+#endif
+			strcat(bootargs,",");
+			strcat(bootargs,baudrate_str);
+			strcat(bootargs," bootprogress");
+		}
+		else
+		{
+			if (silent == 1)
+				strcat(bootargs,"console=null");
+			else
+			{
+				strcat(bootargs,"console=");
+#ifdef CFG_PROJ_CONSOLE_TTY
+				strcat(bootargs,CFG_PROJ_CONSOLE_TTY);
+#else
+				strcat(bootargs,"ttyS0");
+#endif
+				strcat(bootargs,",");
+				strcat(bootargs,baudrate_str);
+			}
+		}
+		strcat(bootargs," rootfstype=cramfs");
+
+#ifdef CONFIG_BIGPHYSAREA
+		strcat(bootargs," bigphysarea=" CONFIG_BIGPHYSAREA);
+#endif		
+#ifdef CONFIG_EXTRA_BOOTARGS 
+		strcat(bootargs," " CONFIG_EXTRA_BOOTARGS);
+#endif
+
+	}
+
+	/* Check if anything to be executed */
+	if (ExecuteAddr != 0xFFFFFFFF)
+	{
+		/* Form Arguments */
+		if(rootisinitrd != 1)
+		{
+			sprintf(AddrStr,"0x%lx",ExecuteAddr);
+			argv[0] = &Command[0];
+			argv[1] = &AddrStr[0];
+			argv[2] = NULL;
+		}
+		else
+		{
+			sprintf(AddrStr,"0x%lx",ExecuteAddr);
+			sprintf(INITRDAddrStr,"0x%lx",initrd_address);
+			argv[0] = &Command[0];
+			argv[1] = &AddrStr[0];
+			argv[2] = &INITRDAddrStr[0];
+			argv[3] = NULL;
+		}
+
+		/* If module is a ELF Executable */
+		if (ExecuteType == MODULE_ELF)
+		{
+			if (silent == 1)
+				console_assign(stdout,"nulldev");
+			retval = do_bootelf(NULL,0,2,argv);
+			console_assign(stdout,"serial");
+			return retval;
+		}
+
+		/* If module is a U-Boot Format Linux Image */
+		if (ExecuteType == MODULE_PIMAGE)
+		{
+			printf("Booting from MODULE_PIMAGE ...\n");
+			printf("Bootargs = [%s]\n",bootargs);
+			setenv("bootargs",bootargs);
+			if (silent == 1)
+				console_assign(stdout,"nulldev");
+			if(rootisinitrd != 1)
+			{
+				retval =  do_bootm(NULL,0,2,argv);
+			}
+			else
+			{
+				setenv("initrd_high","off");
+				retval = do_bootm(NULL,0,3,argv);
+			}
+			console_assign(stdout,"serial");
+			printf("Failed\n");
+			return retval;
+		}
+
+		/* Other Executable modules, jump directly */
+		if ((ExecuteType == MODULE_KERNEL) || (ExecuteType == MODULE_UNKNOWN))
+		{
+			if (silent == 1)
+				console_assign(stdout,"nulldev");
+			retval =  do_go(NULL,0,2,argv);
+			console_assign(stdout,"serial");
+			return retval;
+		}
+	}
+
+	/* If PathID is 1 (OS) and nothing was executed, try booting
+	 * kernel from JFFS2 File system if any available
+	 */
+	if ((PathID == 1) && (RootMtdNo > 0))
+	{
+#if (CONFIG_COMMANDS & CFG_CMD_JFFS2)
+		printf("Booting from image in %s ...\n",KERNEL_FILE);
+		printf("Bootargs = [%s]\n",bootargs);
+		setenv("bootargs",bootargs);
+		if (silent == 1)
+			console_assign(stdout,"nulldev");
+		/* Change active JFFS2 partition to wherever  (Root FS ) */
+		sprintf(AddrStr,"%d",RootMtdNo-1);	/* -1 because Zero Based */
+		argv[0] = &Command[0];
+		argv[1] = &AddrStr[0];
+		argv[2] = NULL;
+		do_jffs2_chpart(NULL,0,2,argv);
+
+		/* Load linux kernel into memory */
+		sprintf(AddrStr,"0x%x",KERNEL_LOADADDR);
+		argv[0] = &Command[0];
+		argv[1] = &AddrStr[0];
+		argv[2] = KERNEL_FILE;
+		argv[3] = NULL;
+		if (do_jffs2_fsload(NULL,0,3,argv) == 0 )
+		{
+			argv[2] = NULL;
+			/* Boot the kernel from memory */
+			retval = do_bootm(NULL,0,2,argv);
+			console_assign(stdout,"serial");
+			printf("Failed\n");
+			return retval;
+		}
+		console_assign(stdout,"serial");
+#else
+        printf("JFFS2 support is not enabled - Cannot locate kernel File in Root\n");
+#endif                
+		printf("Unable to locate %s\n",KERNEL_FILE);
+	}
+
+
+	/* Module Type JFFS and JFFS2 are File System  - Not executable */
+	/* Module Type FIRMWARE is a information module- Not Executable */
+	/* Module Type FPGA has FPGA microcodes - Not Executable by CPU */
+	/* Module Type BOOT LOADER is loaded automatically */
+
+	return 0;
+}
+
