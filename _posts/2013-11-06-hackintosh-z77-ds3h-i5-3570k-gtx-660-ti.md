---
author: joe
comments: true
date: 2013-11-06 17:46:12+00:00
layout: post
slug: hackintosh-z77-ds3h-i5-3570k-gtx-660-ti
title: 'Hackintosh: Z77-DS3H, i5-3570K, GTX 660 Ti'
wordpress_id: 9372
categories:
- OS
ef_usergroup:
- JJK
following_users:
- joechan
- kelatov
tags:
- Hackintosh
- MultiBeast
- UniBeast
---

## Ivy Bridge Hackintosh running OSX 10.8.4 (Mountain Lion): Z77-DS3H, i5-3570K, GTX 660 Ti





This is a guide on how I built my first hackintosh.





### Why?





I wanted a desktop that ran OSX, but I did not want to pay the price for an iMac/Mac Pro. I wanted something powerful that could run OSX, was affordable, expandable, and quiet (mostly in that order). A Mac Pro would meet most of the criteria, but it wouldn't be so affordable. An iMac would meet most of the criteria, but it would not be expandable/reusable. A Mac Mini would meet most of the criteria, but not  be very powerful. A Hackintosh met all my wants best.





I also did not want to spend too much time troubleshooting things. Looking through the OSX86 community and the various guides out there, it looked like OSX86 had matured to a point where it was really simple to get a machine up and running nearly 100% assuming you purchased compatible hardware (more on this below in the **The Build** section).





## Summary







  * Specs: 3.4Ghz (overclocked 4.2Ghz) i5, 16GB RAM, GTX 660 Ti 2GB VRAM 120GB SSD, 500GB HDD


  * OS OSX 10.8.4 (Mountain Lion)


  * Cost: $740





Works:







  * Sound


  * Network


  * GPU (Triple monitor setup: 2x DVI, 1x HDMI)


  * Sleep/Wake


  * CPU Overclocking


  * iCloud (not super significant, but mentioning as I've seen other hackintoshes having issues with this online)





Mostly works:







  * USB 3.0 ports are not backwards compatible (i.e. they ONLY work with USB 3.0 devices).





## The Build





These are the parts I have, although at different prices.







  * [Gigabyte GA-Z77-DS3H](http://amzn.to/17pmfDn)


  * [Intel i5-3570K CPU](http://amzn.to/12VtvoY)


  * [G.SKILL Ares 16GB DDR3-1600 RAM](http://amzn.to/1fp8o0T)


  * [MSI GeForce GTX 660 TI 2GB GPU](http://amzn.to/12VtXDH)


  * [Cooler Master V850 850W PSU](http://amzn.to/14paHfs)


  * [120GB SSD](http://amzn.to/15rDa3H)


  * [500GB HDD](http://amzn.to/1dZ00az)


  * [Fractal Define R4 Computer Case](http://amzn.to/1dNvZN7)





This is the first PC I have ever actually built, so I'll add some of my thoughts how I chose my some of my parts in case you are in the same boat.





In choosing my parts, I mostly stuck to the [CustoMac buyer's guide for August 2013](http://www.tonymacx86.com/375-building-customac-buyer-s-guide-august-2013.html#budget_atx) for mobo/CPU/GPU; the other parts (like RAM/PSU/case) don't matter as much for Hackintosh compatibility. I did this because I wanted minimal setup and maintenance as I did not want to spend a lot of debugging kernel panics and troubleshooting drivers :). The GA-Z77-DS3H motherboard does not require a [DSDT](http://wiki.osx86project.org/wiki/index.php/DSDT), which eliminates some unnecessary complexity.





I chose Ivy Bridge because Haswell support hadn't really been fully vetted by the TonyOSX/OSX86 community yet. I went with the 3570K because I didn't really know any better. In hindsight, I would have probably opted for the 3570 instead for the VT-d CPU extensions, as I plan on running VMs more than I plan to overclock.





My PSU is overkill on this build, but I found a good deal on it ($96 AR for modular, Gold-rated Seasonic).





For the case, I knew I wanted something quiet. Researching online showed that a lot of folks liked [Nanoxia Deep Silence 1](http://www.newegg.com/Product/Product.aspx?Item=N82E16811281001) and the [Fractal Define R4](http://amzn.to/1dNvZN7). At such similar pricepoints, the general consensus was that the German-built Nanoxia was the superior case. However, NewEgg was out of stock, and the only places I found that had them were in Europe (killer shipping). I found the Define R4 on sale and in stock, so I went with that. I really like it.





In the end, the build turned out really nice. It's super fast, quiet, and runs cool.





## The Install





### Preparing UniBeast flashdrive





While you're waiting for your parts to arrive, you can at least start preparing the Unibeast installer. I followed [this guide](http://www.tonymacx86.com/61-unibeast-install-os-x-mountain-lion-any-supported-intel-based-pc.html) to prepare my Unibeast flash drive. This was pretty straightforward.





Make sure to keep the flash drive even after install as it can be useful for recoveries/troubleshooting when things go wrong (i.e. after an accidental OSX update).





### Configuring the BIOS





I followed [the following guide](http://www.tonymacx86.com/99-quick-guide-configuring-uefi-gigabyte-s-7-series-lga-1155-boards.html) to configure my BIOS.





My Z77-DS3H motherboard was revision 1.1 already flashed with the latest non-beta (as of 2013-08-30) BIOS, version F9. I would reommend that you update to the latest non-beta version as well. [Here's a link to the BIOS download page](http://www.gigabyte.us/products/product-page.aspx?pid=4326#bios).





I would recommend using the _Profiles_ feature (if possible, save to SSD/HDD, rather than CMOS) in the BIOS to save your configuration as soon as you have a bootable working configuration so you have something to go back to if you want to tinker with more settings after the base config.





For sleep/wake to work properly, I mucked around with my BIOS settings until I had the following:







  * Boot Option #1: NOT UEFI


  * Init Display First: PEG


  * Internal Graphics: Disabled


  * Internal Graphics Standby Mode: Disabled


  * Internal Graphics Deep Standby Mode: Disabled





[Here are all the screenshots](http://imgur.com/a/KxfiS) of everything I changed in the BIOS. You can change the Turbo Ratios if you want to overclock. The memory profile can be changed to XMP if your RAM supports it (mine did). More about this in the **Overclocking and X.M.P. (optional)** section below.





![Bios settings 1](http://virtuallyhyper.com/wp-content/uploads/2013/09/D2Wr4Un.png)
![Bios settings 2](http://virtuallyhyper.com/wp-content/uploads/2013/09/D2Wr4Un.png)
![Bios settings 3](http://virtuallyhyper.com/wp-content/uploads/2013/09/D2Wr4Un.png)





### Installing OSX with UniBeast





You will be booting from the UniBeast USB flash drive you made earlier to install OSX on your harddrive.





I followed [this guide](http://www.tonymacx86.com/61-unibeast-install-os-x-mountain-lion-any-supported-intel-based-pc.html) to install Unibeast.





I was able to run through the installer with my monitor plugged directly into the GTX 660. Again, pretty straightforward.





### Running MultiBeast (post install)





Once you have OSX installed on the harddrive, you will need to run MultiBeast in order to install the bootloader and other necessary drivers for your rig. I followed [this guide](http://www.tonymacx86.com/61-unibeast-install-os-x-mountain-lion-any-supported-intel-based-pc.html) to run through MultiBeast. I used [Loginfailed's golden build](http://www.tonymacx86.com/golden-builds/74578-loginfaileds-build-i7-3770k-ga-z77-ds3h-16gb-ram-6850-a.html) as a guideline for the most part. Exceptions in the **Gotcha** section below. I ran MultiBeast 5.4.3. In the end, here are my **MultiBeast** settings:





![Multibeast settings](http://i.imgur.com/tn2pdCI.png)





### Gotcha: Z77-DS3H and Ethernet





This is the only part that took the most troubleshooting/research, so I hope this saves you some time and hassle.





[It appears](http://www.tonymacx86.com/desktop-compatibility/77447-ar8161-lan-new-mobo-revisions-ga-z77-ds3h-ga-h77-ds3h-others.html) that the onboard NIC this motherboard, the Z77-DS3H `revision 1.0` runs the AR8151 chipset, whereas the newer revision 1.1 runs the AR8161 chipset. For a while, folks running revision 1.1 could not get onboard ethernet to work. This was until Shailua created a driver for the newer chipset.





This meant I could not follow [Loginfailed's golden build](http://www.tonymacx86.com/golden-builds/74578-loginfaileds-build-i7-3770k-ga-z77-ds3h-16gb-ram-6850-a.html) exactly because `Maolj's AtherosL1cEthernet` kext will only work for the older chipset (AR8151).





The kext that does work for revision 1.1 is kind of an experimental driver and folks have reported issues (kernel panics) when performing large network file transfers. The developer, Shailua, seems to have gone MIA.





Here is where your path may differ from mine depending on your mobo revision and use case.





#### Use the onboard NIC - Shailua's kext







  * Pros: Easy, no need to purchase additional hardware (free)


  * Cons: Other users have reported instabliities with large file transfers





Options:







  * If you have running the older AR8151 chipset on revision 1.0 motherboards, check `Bootloaders -> Drivers -> Network -> AtherosMaolj's AtherosL1cEthernet` in **MultiBeast**.


  * If you are running the newer AR8161 chipset on revision 1.1 motherboards, check `Bootloaders -> Drivers -> Network -> Atheros - Shailua's ALXEthernet v1.0.2` in **MultiBeast**.





#### Buy a PCIe NIC (Realtek RTL8169 chipset) - Lnx2Mac kext







  * Pros: Mature kext, relatively inexpensive


  * Cons: Not free





Options:







  * [DX-PCIGB](http://amzn.to/17SkRaW) ($7)


  * [tg-3269](http://amzn.to/14GJQMc) ($10)


  * [Ultra PCIE](http://amzn.to/1d0eTYM) ($17)





#### Buy a PCIe NIC - (Marvell 88E8053 chipset) - works out of box







  * Pros: Works out of the box (same chipset that Apple uses in their hardware), my assumption is that it is more stable


  * Cons: Most expensive





Options:







  * [Rosewill RC-401-EX](http://amzn.to/18Qd3oZ) ($20) 


  * [Koutech IO-PEN121](http://www.newegg.com/Product/Product.aspx?Item=N82E16833143011) (??)


  * [Sonnet Presto](http://amzn.to/19BTT8I]) ($40) <-- This is supposedly the one that Apple puts in their products





#### Go Wireless







  * [TP-LINK](http://amzn.to/17Slrp9)





### My decision





I went with the onboard NIC for now. If I see any stability issues, I'll probably get one of the PCIe Marvell 88E8053 chipset adapters.





## Overclocking and X.M.P. (optional)





If you want to overclock the 3570K CPU, you can adjust the Turbo Ratio values in the BIOS (note that there is no `v_core` configuration available on the Z77-DS3H motherboard). If your memory supports [X.M.P](http://www.intel.com/content/www/us/en/gaming/gaming-computers/intel-extreme-memory-profile-xmp.html), you can enable that as well in the BIOS.





![bios4](http://virtuallyhyper.com/wp-content/uploads/2013/09/hackintosh-bios4.png)
![bios5](http://virtuallyhyper.com/wp-content/uploads/2013/09/hackintosh-bios5.png)





## Benchmarks (to come later, maybe)





## What if something goes wrong





Boot into safe mode by entering the following in the bootloader with the `-x` boot flag. You can add the `-v` flag to get more information if you are getting kernel panics.





## Conclusion





All in all, this was a fun project and I got exactly what I wanted out of it: a relatively affordable, low-maintenance, powerful, and quiet Mac. Just don't install any OSX updates until they are vetted by the OSX86 community!





## References





### Similar builds







  * [Loginfailed's Build - i7-3770k / GA-Z77-DS3H / 16GB RAM / 6850](http://www.tonymacx86.com/golden-builds/74578-loginfaileds-build-i7-3770k-ga-z77-ds3h-16gb-ram-6850-a.html)


  * [djsmootht's Build: GA-Z77-DS3H - i5-3570K - 9600GT - 16GB - Photo Editing Machine](http://www.tonymacx86.com/user-builds/75407-success-mountain-lion-gigabyte-ga-z77-ds3h-i5-3570k-ivy-bridge-16gb.html)


  * [Slugnet's Video Editor GA-Z77X-UD5H / i7-3770k / 16GB RAM / GIGABYTE GeForce GTX 660 Ti](http://www.tonymacx86.com/golden-builds/70530-slugnets-video-editor-ga-z77x-ud5h-i7-3770k-gigabyte-geforce-gtx-660-ti.html)


  * [Frankencat's Pro Tools build: Core i5 3570K - Gigabyte Z77-DS3H - 16GB RAM - GeForce GT 610 - PCI-E Ethernet](http://www.tonymacx86.com/user-builds/77683-frankencats-pro-tools-build-core-i5-3570k-gigabyte-z77-ds3h-16gb-ram-geforce-gt-610-pci-e-ethernet.html)


  * [Luisdavilab's build: i5-3570k - Ga-Z77-DS3H - 16 GB - Sapphire 6870](http://www.tonymacx86.com/user-builds/65522-success-luisdavilab-build-i5-3570k-ga-z77-ds3h-16-gb-sapphire-6870-1-gb.html)


  * [MJayVizzle's Gigabyte GA-Z77-DS3H, i5-3750K, GT-640](http://www.tonymacx86.com/mountain-lion-desktop-guides/77749-success-gigabyte-ga-z77-ds3h-i5-3750k-gt-640-a.html)





### Issues







  * [Big thread on DS3H rev 1.1 onboard AR8161 NIC issue](http://www.tonymacx86.com/desktop-compatibility/77447-ar8161-lan-new-mobo-revisions-ga-z77-ds3h-ga-h77-ds3h-others.html)


  * [Sleep issue](http://www.tonymacx86.com/mountain-lion-desktop-support/90804-ga-z77-ds3h-sleep-not-working-all-help.html)



