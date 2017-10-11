# Flashing SPI chips using the BeagleBone Black

These blog posts are starting to get a theme.
"I broke something, so I had to do something else".
Well, last weekend I broke the USB port on my BeagleBone Black
(BBB). You may recall from the previous posts that I
was using a CH341A USB programmer for all my SPI flashing
needs which was attached to said USB port. At first
I thought I had fried the programmer, but a replacement
programmer wasn't working either, so I suspect I fried the
USB logic. To get around this issue, I decided to revisit
the footnote from the first blog post. In particular,
the BeagleBone has two SPI masters that in theory
should be usable for all my programming needs.

# Wiring up the beaglebone

Looking at the beaglebone's pinout diagram:
![Pinout](https://graycat.io/wp-content/uploads/2015/04/beaglebone_pinout1.png)
we see that SPI1 is populated on P9 pins 28-31
(I'm not using SPI0, because that shares pins
with UART2, which I'm using to interface with
the BMC's serial port).

One thing that's not shown on the diagram above is
that you can swap MISO and MOSI, which I did (mostly
because I looked at a different picture).

The whole setup looked like this:
![BBBSPI](images/BBBSPI.jpg)

Those following along closely may have noticed that I'm
using the aligator clips from the external programmer again.
In this case, this is not to hold the BMC in reset, but simply
because I wanted to avoid stressing the BBB's voltage regulator.
The motherboard takes about 400mA, which while not a huge number
is not insignificant either. One of my theories is that the large
power drain is what fried the USB port. With this, I hope to be
a bit safer and be able to avoid frying the BBB.

# Enabling SPI in the pinmux.

I followed the instructions from https://elinux.org/BeagleBone_Black_Enable_SPIDEV#SPI1_D0_Output_and_D1_Input,
which worked well, except that I had to redo the `echo BB-SPI1-01 > /sys/devices/bone_capemgr.*/slots
` after the reboot to actually get the device to show up. For completeness (and my future reference),
 I'm dumping the full commands here:

```
# cd ~
# cat <<<END
/dts-v1/;
/plugin/;

/* SPI1 */
/* D0 Output and D1 Input */

/ {
    compatible = "ti,beaglebone", "ti,beaglebone-black";

    /* identification */
    part-number = "spi1mux";

    fragment@0 {
        target = <&am33xx_pinmux>;
        __overlay__ {
            spi1_pins_s0: spi1_pins_s0 {
                        pinctrl-single,pins = <
                                0x190 0x33      /* mcasp0_aclkx.spi1_sclk, INPUT_PULLUP | MODE3 */
                                0x194 0x13      /* mcasp0_fsx.spi1_d0, OUTPUT_PULLUP | MODE3 */
                                0x198 0x33      /* mcasp0_axr0.spi1_d1, INPUT_PULLUP | MODE3 */
                                0x19c 0x13      /* mcasp0_ahclkr.spi1_cs0, OUTPUT_PULLUP | MODE3 */
                >;
            };
        };
    };

    fragment@1 {
        target = <&spi1>;
        __overlay__ {

             #address-cells = <1>;
             #size-cells = <0>;
             status = "okay";
             pinctrl-names = "default";
             pinctrl-0 = <&spi1_pins_s0>;
             ti,pindir-d0-out-d1-in = <1>;

             spidev@1 {
                 spi-max-frequency = <24000000>;
                 reg = <0>;
                 compatible = "linux,spidev";
            };
        };
    };
};
END
# dtc -O dtb -o BB-SPI1-01-00A0.dtbo -b 0 -@ BB-SPI1-01-00A0.dts
# cp BB-SPI1-01-00A0.dtbo /lib/firmware/
# echo BB-SPI1-01 > /sys/devices/bone_capemgr.*/slots
# vi /boot/uEnv.txt # Add BB-SPI1-01 to enable_partno
# reboot
# echo BB-SPI1-01 > /sys/devices/bone_capemgr.*/slots
```

# Patch flashrom

Unfortunately, flashrom did not work out of the box, and I had to put in a
[small patch](https://github.com/flashrom/flashrom/pull/24) on top of the
already patched flashrom for 4ba support (see previous posts).

# Success

And with that, we're all set and able to use flashrom like so
```
../flashrom/flashrom -p linux_spi:dev=/dev/spidev1.0 -w obmc-phosphor-image-palmetto-20170930163941.static.mtd
```
which will write the given OpenBMC image to the flash. The BBB is significantly
faster at programming than my USB programmer was, so I suppose
despite having to brick the programmer to find out, this represents
an improvement to my setup.
