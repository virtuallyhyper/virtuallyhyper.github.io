---
author: karim
comments: true
date: 2012-03-27 14:45:18+00:00
layout: post
slug: possible-reasons-for-rarp-storms-from-an-esx-host
title: Possible reasons for RARP storms from an ESX host
wordpress_id: 355
categories:
- Networking
- VMware
following_users:
- kelatov
tags:
- beacon probing
- GARP
- Gratuitous ARP
- multicast
- pxe boot
- RARP
- RARP Flood
---

I see a lot of different reasons for a RARP storms from an ESX host. The biggest one that I ran into is a PXE boot environment (ie: Citrix Provisioning Server). During the PXE boot process the unicast mac-address changes and the ESX host sends a GARP to notify the switch (sometimes these GARPs show up as RARPs in Wireshark). If you see thousands of RARPs, then you may have run into some known issues that have been fixed with the following patch. For ESX 4.1 [2009136](http://kb.vmware.com/kb/2009136) and for ESXi 4.1 [2009143](http://kb.vmware.com/kb/2009143), for both patches we see the following:


> An erroneous code sends out Reverse Address Resolution Protocol (RARP) broadcasts immediately after a unicast Address is changed. The issue is resolved by timing the RARP broadcasting in a timer to avoid RARP packets from flooding.


If you see a couple of RARPs, then that is normal. Another reason if RARPs is joining a Multicast group, again a couple are fine but an excessive amount is a known issue and for 4.1 the above patch fixes it. From the same articles:

UPDATE: This has been back ported into 4.0 and is included in P11 for ESX [2011767](http://kb.vmware.com/kb/2011767) and for ESXi [2011768](http://kb.vmware.com/kb/2011768)


> ESXi generates unnecessary and excessive Reverse Address Resolution Protocol (RARP) packets when virtual machines join or leave multicast groups.


ESXi 5.0 U1 fixes similar issues, from the [ESXi 5.0 U1 Release Notes](https://www.vmware.com/support/vsphere5/doc/vsp_esxi50_u1_rel_notes.html):





> 

> 
> **ESXi host generates excessive RARP broadcasts with multicast traffic**
ESXi host generates excessive Reverse Address Resolution Protocol (RARP) packets when virtual machines join or leave multicast groups.This issue is resolved in this release.


And lastly, I have seen a lot of RARPs when you have Beacon probes enabled. Another person actually blogged about it already: [Beacon probing resulting in excessive broadcasts](http://virtualrj.wordpress.com/2009/01/14/beacon-probing-resulting-in-excessive-broadcasts/).

Like I mentioned, some RARPs are actually okay. Whenever you have 'notify switch' set to yes on your virtual switch or your network port groups then you will send GARPs (which are seen as RARPs). A VMware document describes what the 'notify switch' option does, [VMware Virtual Networking Concepts](http://www.vmware.com/files/pdf/virtual_networking_concepts.pdf):


> Using the Notify Switches policy setting, you determine how ESX Server communicates with the physical switch in the event of a failover. If you select Yes, whenever a virtual Ethernet  adapter is connected to the virtual switch or whenever that virtual Ethernet adapter’s traffic would be routed over a different physical Ethernet adapter in the team due to a failover event, a notification is sent out over the network to update the lookup tables on physical switches. In almost all cases, this is desirable for the lowest latency when a failover occurs


The way that an ESX host updates the switch is by sending a GARP ([Gratuitous ARP](http://wiki.wireshark.org/Gratuitous_ARP)). Here is how it looks like from the ESXi host when I run tcpdump-uw (I fired a vMotion of a VM with the mac address of 00:50:56:9b:4f:0d):

    
    listening on vmk0, link-type EN10MB (Ethernet), capture size 96 bytes
    01:21:56.178929 00:50:56:9b:4f:0d > ff:ff:ff:ff:ff:ff, ethertype Reverse ARP (0x8035), length 60: Reverse Request who-is 00:50:56:9b:4f:0d tell 00:50:56:9b:4f:0d, length 46
    01:21:56.964847 00:50:56:9b:4f:0d > ff:ff:ff:ff:ff:ff, ethertype Reverse ARP (0x8035), length 60: Reverse Request who-is 00:50:56:9b:4f:0d tell 00:50:56:9b:4f:0d, length 46
    01:21:57.964780 00:50:56:9b:4f:0d > ff:ff:ff:ff:ff:ff, ethertype Reverse ARP (0x8035), length 60: Reverse Request who-is 00:50:56:9b:4f:0d tell 00:50:56:9b:4f:0d, length 46
    01:21:58.964792 00:50:56:9b:4f:0d > ff:ff:ff:ff:ff:ff, ethertype Reverse ARP (0x8035), length 60: Reverse Request who-is 00:50:56:9b:4f:0d tell 00:50:56:9b:4f:0d, length 46
    01:22:00.964920 00:50:56:9b:4f:0d > ff:ff:ff:ff:ff:ff, ethertype Reverse ARP (0x8035), length 60: Reverse Request who-is 00:50:56:9b:4f:0d tell 00:50:56:9b:4f:0d, length 46
    01:22:03.964933 00:50:56:9b:4f:0d > ff:ff:ff:ff:ff:ff, ethertype Reverse ARP (0x8035), length 60: Reverse Request who-is 00:50:56:9b:4f:0d tell 00:50:56:9b:4f:0d, length 46


So you can see that the Mac address 00:50:56:9b:4f:0d (the mac address of the VM) is sending a GARP (but the packet capture shows it as a RARP) and you can see the target machine is itself (tell 00:50:56:9b:4f:0d). If you don't want to apply the patches above, you can just set the 'notify-switch' option to 'no'. Instructions can be found here: [Edit Failover and Load Balancing Policy for a vSphere Standard Switch](http://pubs.vmware.com/vsphere-50/index.jsp?topic=/com.vmware.vsphere.networking.doc_50/GUID-D5EA6315-5DCD-463E-A701-B3D8D9250FB5.html)
