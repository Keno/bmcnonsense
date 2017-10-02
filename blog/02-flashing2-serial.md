# Flashing Firmware (Part 2) and Serial Ports

## Flashing Firmware Revisited

In the pervious blog post I managed to find a way to externally
flash the BMC firmware on a Supermicro X8SIE-F motherboard.
Well, uhm, not anymore. I accidentally ripped off the bent up
pin and in the subsequent attempt to replace the chip lifted
the data pins, making the situation unsalvagable. Luckily, I
was expecting that I might brick the board, so I have another
X8SIL-F board, which is essentially the same layout-wise but in
a smaller form factor. Determined to avoid soldering-related
bricking this time around, I played around with in-situ programming
some more and was able to come up with a solution. There were
two key realizations here. The first is that the BMC disable
jumper on the board doesn't actually turn off the BMC, so
the inability to program in-situ is caused by interference from
the BMC. The second is that the BMC and the SPI flash have
different operating voltage regions. The BMC wants 3.3V pretty
precisely, while the SPI flash storage is much more lenient.
The solution is thus pretty simple: Feed the 3.3V rail with a
sufficiently high voltage to power the flash chip, but not high
enough to power the BMC, causing it to interfere on the SPI bus.
After some experimenting, 2.6V seemed to be a good middle ground
here. At lower voltages, I would get an occaisional read error
from the chip, but at 2.6V I had no such problems. The full setup
looked like this (the aligator clips go to a lab power supply set
to 2.6V):

![Programmer with external power](images/programmer_with_ext_power.jpg)


No soldering required.

## Finding serial ports

As noted in [devicenull's blog post](http://blog.devicenull.org/2014/06/06/x8sil-f-ipmi-serial-debug-port.html) the X8SIL-F has the BMC's debug UART
exposed on the (unpopulated) J25/J26 headers. I soldered some headers and
was able to get the debug serial output mentioned in the blog post (yay!).
The whole setup looked like this:

![Serial](images/serial.jpg)

Unfortunately, the board does not seem to react to keyboard input in the serial
console. I verified that the signals arrived on the board using an oscilloscope,
so either the firmware is ignoring input, or I accidentally destroyed the RX
connection when soldering the headers. To be investigated.

By investigating the X8SIL-F board, I was also able to find the same serial port
on the X8SIE-F board (which of course doesn't help me now since the board is bricked,
but maybe it'll help somebody else in the future). The two boards generally share
part numbering and while the X8SIE-F, doesn't have the J25/26 headers, it does
have two (unpopulated) resistors that terminate the signal: R1101 (which sits on the RX line)
and R1094 (which sits on the TX line). R1094 is on the flip side of the board. Here's
the relevant pictures (from the X8SIE-F):

![Top](images/x8sie_top.jpg)
![Bottom](images/x8sie_bottom.jpg)
