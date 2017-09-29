# BMC Nonsense

# Introduction

Over time, I've grown increasingly frustrated with the BMCs found on most server
motherboards. For those that don't know, BMCs are small, low-power embedded computers
(usually ARM based) that have broad access to the host system (depending on the model,
access to host memory, the ability to emulate USB devices, the ability to intercept
video output) as well as (often) their own network connection. The firmware of these
devices is generally distributed as a binary blob. If you're lucky, vendors will ship
security updates for major exploits discovered in their firmware.

So to summarize, we have a chip that:
- Has essentially physical access to your machine (can be trivially escalated to root on the host machine).
- Runs a binary blob that is usually written once and not frequently updated for security best practices
- Has network access

Outch! This is the kind you read at the begging of a security post mortem.
The security problems of these devices are well known (see e.g. 
[here](https://www.itworld.com/article/2708437/security/ipmi--the-most-dangerous-protocol-you-ve-never-heard-of.html) or
[here](https://blog.rapid7.com/2013/07/02/a-penetration-testers-guide-to-ipmi/)).

In addition, I have a few complaints about their primary functionality (remote
management as well):
- The most comprehensive way to configure these devices is via a web interface. However,
  the BMCs are quite slow and barely capable of running a web server, making the experience
  quite excruciating.
- Various features of that web site (such as the KVM redirection) require JAVA applets
- The BMC implements several management protocols IPMI, SMASH, etc. That I don't care the
  least bit about. Not to mention the problems with authentication and security in these
  servers (running as root of course).

Really all I want here is an SSH server that integrates with our standard access control system,
has the few basic management commands as shell commands and doesn't keep me awake at night
worrying about the security implications.

Luckily, I'm not the only in that camp either. Facebook, as well as Google and IBM have started
developing open source BMC software for use in their servers (confusingly both named OpenBMC, despite
being separate projects). These run on the same
BMC chips found in commercial servers. As such, it seems to me that it should be possible
to, with just a bit of elbow grease rid the BMCs of their binary blobs, and replace the software
by an appropriate open source alternative.

The plan thus seems fairly clear:
- Obtain Motherboard with BMC to play with
- Figure out how to flash new firmware
- Port a bootloader (most likely u-boot)
- Port the linux kernel (for both the CPU and the board)
- Use the some OpenBMC effort as the userland

If we're lucky, part of this will have been done for us (e.g. because the ports of the chips
themselves are already done). One thing that makes our task more difficult is that
datasheets and schematics for both boards and chips are very hard to come by, so we'll have to
employ a fair bit of reverse-engineering skill to figure out what's happening. Since Linux
is GPL, the vendors are requied to share their modifications, so we might be able to glean
some information from that (though of course it's based on 10-year old versions of Linux,
so not necessarily the correct way to do things now).

I figured I'd give it a shot and see how far I get. This repository is intended to chronicle
my progress, so that others may pick up where I stop.

# The boards

I have the following boards/BMCs that I'm willing
to brick in the process.

- Supermicro X8SIE-F (Nuvoton WPCM450) - Accidentally destroyed due to improper soldering
- Supermicro X8SIL-F (Nuvoton WPCM450)
- Supermicro X11SSH-F (Aspeed AST24000)
- Lenovo RS160 (Aspeed AST2500)

# Organization of this repository

I'm intending this repository to be a collection of all things I find while
working on this project, e.g. code, data, images. I'm also planning to write
about what I find either in blog post format or diary format (or both).
