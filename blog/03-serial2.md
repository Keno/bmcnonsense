Last time, we identified serial ports on the X8SIE-F and X8SIL-F boards (both of which I have since accidentally bricked). Having a bit of a better idea of what a serial port would look like if not exposed on a header, I went hunting on the X11SSH-F board I have and after a bit of poking with the oscilloscope, I was successful in finding the debug serial TX pin. The TX signal is on one of the pads of each of the RM33 and RM39 resistors. These are unpopulated on my board, but are located very close tothe bottom left of the AST2400:

![pads](images/x11sshf-1.jpg)

With a quick soldering job, I was able to hook this up to my trusty beaglebone

![hooked up](images/x11sshf-2.jpg)

and was able to capture the following output:

```
DRAM Init-DDR3
CBR0-1357135670123456701234567
CBR13Done


U-Boot 2009.01-00053-g03e9b5e-dirty ( 3月 12 2015 - 11:47:53) ASPEED (v.0.21) 

I2C:   ready
DRAM:  128 MB
Flash: 32 MB
*** Warning - bad CRC, using default environment

In:    serial
Out:   serial
Err:   serial
H/W:   AST2400 series chip
COM:   port1 and port2
PWM:   port[ABCDH]
Un-Protected 1 sectors
Erasing Flash...Writing to Flash... done
Protected 1 sectors
Hit any key to stop autoboot:  3  2  1  0 
## Booting kernel from Legacy Image at 21400000 ...
   Image Name:   21400000
   Image Type:   ARM Linux Kernel Image (gzip compressed)
   Data Size:    1536440 Bytes =  1.5 MB
   Load Address: 40008000
   Entry Point:  40008000
   Verifying Checksum ... OK
   Uncompressing Kernel Image ... OK

Starting kernel ...

Linux version 2.6.28.9 (root@ubuntu) (gcc version 4.9.1 (crosstool-NG 1.20.0) ) #1 Tue Dec 15 18:45:57 CST 2015
CPU: ARM926EJ-S [41069265] revision 5 (ARMv5TEJ), cr=00053177
CPU: VIVT data cache, VIVT instruction cache
Machine: ASPEED-AST2300
Memory policy: ECC disabled, Data cache writeback
Built 1 zonelists in Zone order, mobility grouping on.  Total pages: 20066
Kernel command line: console=ttyS1,115200 root=/dev/mtdblock2 rootfstype=cramfs noinitrd rw mem=79M
PID hash table entries: 512 (order: 9, 2048 bytes)
Console: colour dummy device 80x30
console [ttyS1] enabled
Dentry cache hash table entries: 16384 (order: 4, 65536 bytes)
Inode-cache hash table entries: 8192 (order: 3, 32768 bytes)
Memory: 79MB = 79MB total
Memory: 76736KB available (2848K code, 358K data, 112K init)
SLUB: Genslabs=12, HWalign=32, Order=0-3, MinObjects=0, CPUs=1, Nodes=1
Calibrating delay loop... 191.69 BogoMIPS (lpj=958464)
Mount-cache hash table entries: 512
CPU: Testing write buffer coherency: ok
net_namespace: 636 bytes
NET: Registered protocol family 16
SCSI subsystem initialized
NET: Registered protocol family 2
IP route cache hash table entries: 1024 (order: 0, 4096 bytes)
TCP established hash table entries: 4096 (order: 3, 32768 bytes)
TCP bind hash table entries: 4096 (order: 2, 16384 bytes)
TCP: Hash tables configured (established 4096 bind 4096)
TCP reno registered
NET: Registered protocol family 1
NetWinder Floating Point Emulator V0.97 (extended precision)
NTFS driver 2.1.29 [Flags: R/W].
JFFS2 version 2.2. (NAND) (SUMMARY)  © 2001-2006 Red Hat, Inc.
fuse init (API version 7.10)
msgmni has been set to 149
alg: No test for stdrng (krng)
io scheduler noop registered
io scheduler anticipatory registered
io scheduler deadline registered
io scheduler cfq registered (default)
Non-volatile memory driver v1.2
ttyS0 at MMIO 0x1e783000 (irq = 9) is a ASPEED UART
ttyS1 at MMIO 0x1e784000 (irq = 10) is a ASPEED UART
brd: module loaded
loop: module loaded
nbd: registered device at major 43
Driver 'sd' needs updating - please use bus_type methods
Driver 'sr' needs updating - please use bus_type methods
BMC flash ID:0xc21920c2
 platform_flash: MX25L25635E (32768 Kbytes)
Creating 7 MTD partitions on "spi0.0":
0x00000000-0x00100000 : "bootloader"
0x00100000-0x00400000 : "nvram"
0x00400000-0x01400000 : "rootFS"
0x01400000-0x01700000 : "kernel"
0x01700000-0x01f40000 : "webpage"
0x00000000-0x01fc0000 : "all_part"
0x01fc0000-0x01fd0000 : "uboot_env"
mice: PS/2 mouse device common for all mice
ip_tables: (C) 2000-2006 Netfilter Core Team
TCP cubic registered
NET: Registered protocol family 10
lo: Disabled Privacy Extensions
ip6_tables: (C) 2000-2006 Netfilter Core Team
IPv6 over IPv4 tunneling driver
sit0: Disabled Privacy Extensions
NET: Registered protocol family 17
RPC: Registered udp transport module.
RPC: Registered tcp transport module.
802.1Q VLAN Support v1.8 Ben Greear <greearb@candelatech.com>
All bugs added by David S. Miller <davem@redhat.com>
VFS: Mounted root (cramfs filesystem) readonly.
Freeing init memory: 112K
starting pid 334, tty '': '/etc/init.d/rcS'
udevd[359]: specified group 'tty' unknown

udevd[359]: specified group 'kmem' unknown

populate devices node .............
Block at 0x00250000: free 0x0000a714, dirty 0x00000000, unchecked 0x000053cc, used 0x000004e8, wasted 0x00000038
JFFS2 notice: (539) jffs2_build_xattr_subsystem: complete building xattr subsystem, 0 of xdatum (0 unchecked, 0 orphan) and 0 of xref (0 dead, 0 orphan) found.
Ethernet Channel Bonding Driver: v3.3.0 (June 10, 2008)
bonding: MII link monitoring set to 100 ms
usb_drv.c: usb_init - User defind MAJOR NUM # 222.
usb_init - Request USB Driver IRQ successful., 5
BMC RAM 128M
Video RAM 16M
Memory Clock = 396
Memory Bandwidth = 633
video Mode detect failed.
USB HID Module insert, Build Time 18:46:22
RegisterDevice : p_St_Dev->b_DevicesBitMap=1  !! 
usb: DevEnable =====        0!! ====, speed = 0
I2C Module insert, Build Time 18:46:16
[DRIVER] Insert the driver module insmod /bin/module/i2c.ko BoardID=0x0 
[CHE_DBG_I2C] Create file.
[CHE_DBG_I2C] Failed to create file.
=============== Platform Setting ====================
HWConfig= 1, Board ID= 884
Perpherial Lists:
======================================================
[DRIVER] Insert the driver module insmod /bin/module/gpiodrv.ko BoardID=0x884 
ftgmac: module license 'unspecified' taints kernel.
FTGMAC100: ASPEED FTGMAC Driver, (Linux Kernel 2.6.28.9) 09/13/2010 - by ASPEED
MAC1 Support NCSI
FTGMAC100: eth0:ftgmac100_reset, phyAddr=0x0, miiPhyId=0x0000_0002
Intel NCSI can be supported.
wait for completing PHY reset. count 0
FTGMAC100: eth1:ftgmac100_reset, phyAddr=0x0, miiPhyId=0x001c_c915
RTL8211 PHY chip can be supported.
PHY Link speed 100M
[DRIVER] Insert the driver module insmod /bin/module/ftgmac.ko BoardID=0x884 
Got the default zero MAC address !
sync PS to bootloader
Update uboot env ipaddr = 192.168.7.11
Update uboot env ethaddr = ac:1f:6b:17:8c:b2
IPMB Module insert, Build Time 18:46:16
IPMB num = 2
[IPMB] Channel=4  Slave address 0x20
[IPMB] Channel=5  Slave address 0x20
[DRIVER] Insert the driver module insmod /bin/module/ipmb.ko BoardID=0x884  bmc_address=32 ipmb_channel=4,5
[DRIVER] Insert the driver module insmod /bin/module/wdt_drv.ko BoardID=0x884 
Configure GPIOE2 E3 as input pin
[DRIVER] Insert the driver module insmod /bin/module/uart_drv.ko BoardID=0x884 
configurUnRegisterDevice : p_St_Dev->b_DevicesBitMap=0  !! 
e mouse mode!
RegisterDevice : p_St_Dev->b_DevicesBitMap=1  !! 
usb: DevEnable =====        0!! ====, speed = 0
[UtilUsbHidSetMouseMode] The current Modes Absolute Mode
[DRIVER] Insert the driver module insmod /bin/module/peci.ko BoardID=0x884 
ast_pwm_tacho: driver successfully loaded.
[DRIVER] Insert the driver module insmod /bin/module/ast_pwm_fan.ko BoardID=0x884 
[DRIVER] Insert the driver module insmod /bin/module/adc_drv.ko BoardID=0x884 
[UtilSDRCmdsInit] SDR records: 32
[UtilSDRCmdsInit] Sensor number: 25
SMBUS CLK: 400KHZ
Loading the configuration of the riser card
DCMI_module initialize .... 
Create debug files - /tmp/message
[sync_system_time]Sync time 13:09:18 (59da237e) from ME!
Starting VM daemon .....
[DRIVER]KCS 3 data Reg h=c l=a2
 Insert the driver module insmod /bin/module/kcs_drv.ko BoardID=0x884 
Waiting for ready message from kernel
Recieve Events from NIC Drive
LAN Notifier Ready
ADDRCONF(NETDEV_UP): bond0: link is not ready
ifenslave: bond0: can't set hw address
ifenslave: can't enslave eth0 to bond0: Cannot assign requested address
ifenslave: bond0: can't set hw address
ifenslave: can't enslave eth1 to bond0: Cannot assign requested address
[debug] cmd = ip link set bond0 down
Starting IKVM daemon .....
[UtilSetMACAddr Debug] cmd = ip link set eth0 addr ac:1f:6b:17:8c:b2
start Web server .............
[UtilSetMACAddr Debug] cmd = ip link set eth1 addr ac:1f:6b:17:8c:b2
ADDRCONF(NETDEV_UP): bond0: link is not ready
eth0:ftgmac100_open
MAC1 Support NCSI
FTGMAC100: eth0:ftgmac100_reset, phyAddr=0x0, miiPhyId=0x0000_0002
Intel NCSI can be supported.
eth0:ftgmac100_enable
[NCSI] NCSI_CMD 82  
[DeSelect_Package] 
[NCSI] NCSI_CMD 81  
[Select_Package] Found PackageID  0
[NCSI] NCSI_CMD 80  
[Clear_Initial_State] 
[NCSI]Discovery NCSI channel 0
Found NCSI Network Controller at (0, 0)
[NCSI] NCSI_CMD 95  
[Get_Version_ID] 
[NCSI]Channel numbers: 1
[Get_Capabilities] 
[NCSI] NCSI_CMD 81  
[Select_Active_Package] 
[Enable_Set_MAC_Address] MAC ac:1f:6b:17:8c:b2
[NCSI] NCSI_CMD 8e  
[Enable_Set_MAC_Address] 
[NCSI] NCSI_CMD 90  
[Enable_Broadcast_Filter] 
[UnSupport/Err CMD 89]Response Code 100 Reason Code 409
Retry: Command = 9, Response_Code = ffff
[UnSupport/Err CMD 89]Response Code 100 Reason Code 409
Retry: Command = 9, Response_Code = ffff
[UnSupport/Err CMD 89]Response Code 100 Reason Code 409
Retry: Command = 9, Response_Code = ffff
[UnSupport/Err CMD 89]Response Code 100 Reason Code 409
Retry: Command = 9, Response_Code = ffff
[NCSI] NCSI_CMD 8c  
[Enable_VLAN] 
[NCSI] NCSI_CMD 97  
[Get_Parameters] 
[NCSI] NCSI_CMD 87  
[Disable_Network_TX 0] 
[NCSI] NCSI_CMD 84  
[Disable_Channel 0] 
Start ncsi channel 0
[Enable_Channel 0] 
[NCSI] NCSI_CMD 83  
[Enable_Network_TX 0] 
[NCSI] NCSI_CMD 86  
Empty IKVM Port in PS,use default port 5900
UART Set SerialMode = 1
UtilDisableHWSOL
UART no hardware flow control
SSL certificate verified OK.
2017-10-08 13:09:25: (network.c.260) warning: please use server.use-ipv6 only for hostnames, not without server.bind / empty address; your config will break if the kernel default for IPV6_V6ONLY changes 
-n Starting sfcb: 
bonding: bond0: Warning: failed to get speed and duplex from eth0, assumed to be 100Mb/sec and Full.
bonding: bond0: making interface eth0 the new active one.
bonding: bond0: first active interface up!
bonding: bond0: enslaving eth0 as an active interface with an up link.
ADDRCONF(NETDEV_CHANGE): bond0: link becomes ready
eth1:ftgmac100_open
wait for completing PHY reset. count 0
FTGMAC100: eth1:ftgmac100_reset, phyAddr=0x0, miiPhyId=0x001c_c915
RTL8211 PHY chip can be supported.
PHY Link speed 100M
eth1:ftgmac100_enable
done.
bonding: bond0: Warning: failed to get speed and duplex from eth1, assumed to be 100Mb/sec and Full.
bonding: bond0: enslaving eth1 as a backup interface with an up link.
udhcpc (v1.23.1) started
[LanConfigApply Debug] arpping , cmd = arping -b -c 2 0.0.0.0 -I bond0 >>/dev/null
bonding: bond0: link status definitely down for interface eth1, disabling it
arping: can't connect to remote host (0.0.0.0): Invalid argument
Sending discover...
Starting STUNNEL .....
Verify the ssh key-/nv/dropbear/dropbear_rsa_host_key. Please wait.
ip: RTNETLINK answers: No such process
route: SIOCADDRT: Network is unreachable
route: SIOCADDRT: Invalid argument
Sending discover...
Verify the ssh key-/nv/dropbear/dropbear_dss_host_key. Please wait.
Starting SMASH daemon .....
stunnel: LOG5[810:1073875376]: stunnel 4.56 on arm-unknown-linux-gnu platform
stunnel: LOG5[810:1073875376]: Compiled/running with OpenSSL 1.0.1p 9 Jul 2015
stunnel: LOG5[810:1073875376]: Threading:PTHREAD Sockets:POLL,IPv6 SSL:ENGINE,OCSP
stunnel: LOG5[810:1073875376]: Reading configuration from file /nv/stunnel.conf
stunnel: LOG4[810:1073875376]: Insecure file permissions on /etc/stunnel/mail.pem
stunnel: LOG5[810:1073875376]: Configuration successful

Please press Enter to activate this console. Sending discover...
UtilGetSystemResetStatus=0
udhcpc leasefail
No lease, forking to background
```
