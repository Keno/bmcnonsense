With functioning serial and ISP programming capabilities for the X11SSH-F, it's time to
start looking at replacing the firmware. The BMC chip on this board is an AST2400, which
luckily is one of the chips already supported by OpenBMC, so we can save ourselves some
work.

# Build U-Boot

One of the things I love about the OpenBMC project is that they're very good about
upstreaming all their work. They haven't gotten around to upstreaming their work on the
AST2400 yet, but they do maintain a u-boot fork that as far as I can tell is based on
forward porting the vendor tree at https://github.com/openbmc/u-boot, so let's build that
for our chip:

```
git clone -b v2016.07-aspeed-openbmc https://github.com/openbmc/u-boot
mkdir u-boot-x11
cd u-boot-x11
make O=$(pwd) -C ../u-boot CROSS_COMPILE=arm-linux-gnueabihf- ast_g4_phy_defconfig
make O=$(pwd) -C ../u-boot CROSS_COMPILE=arm-linux-gnueabihf-
```

Which generates a `u-boot.bin` file that is our boot loader. We can't quite flash that
because it's not the correct size, but simply padding it to the correct size works fine:

```
dd if=/dev/zero ibs=1 count=33554432 > u-boot-padded.bin
dd if=u-boot.bin of=u-boot-padded.bin conv=notrunc
```

# Generate the U-Boot environment image

Next, we need to set some environment variables, such as the BMC's mac address.
This seems to be done like so:

```
$ cat env.txt
ethaddr=xx:xx:xx:xx:xx:xx # Replace with the MAC address of your BMC

# Find these values (CONFIG_ENV_OFFSET/CONFIG_ENV_SIZE) in the u-boot config. Here's mine:
# https://github.com/openbmc/u-boot/blob/v2016.07-aspeed-openbmc/include/configs/ast-common.h
$ ./tools/mkenvimage -s 0x20000 -o u-boot-env.bin env.txt

# Add to the flash image (convert hex to dev first)
dd if=u-boot-env.bin of=u-boot-padded.bin ibs=1 obs=1 seek=393216 count=131072 conv=notrunc
```

# Build Flashrom

The flash chip on the X11SSH-F is a MX25L25635F, which is not supported by vanilla
flashrom at the time of this writing. However, there is a patch awaiting review that
does support this chip and I was able to use it just fine:

```
git clone https://github.com/flashrom/flashrom
cd flashrom
git pull https://review.coreboot.org/flashrom refs/changes/55/19855/2
make
```

# Flash new firmware to the device
```
../flashrom/flashrom -p ch341a_spi -w u-boot-padded.bin
```

This takes a few minutes.

# See it boot up

```
U-Boot 2016.07-00021-g1b3fb95 (Oct 08 2017 - 01:44:24 -0400)

DRAM:  112 MiB
WARNING: Caches not enabled
Flash: 32 MiB
In:    serial
Out:   serial
Err:   serial
Net:   MAC0 : RMII/NCSI
MAC1 : RGMII
FTGMAC100#0, FTGMAC100#1
Error: FTGMAC100#1 address not set.

ast#
```
