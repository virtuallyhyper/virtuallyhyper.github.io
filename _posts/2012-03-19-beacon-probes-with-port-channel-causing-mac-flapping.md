---
author: karim
comments: true
date: 2012-03-19 16:17:19+00:00
layout: post
slug: beacon-probes-with-port-channel-causing-mac-flapping
title: Beacon Probes with Port-Channel Causing Mac-Flapping
wordpress_id: 60
categories:
- Networking
- VMware
tags:
- beacon probes
- beacon probing
- beacons
- flapping
- port channels
---

Recently, someone asked me why when using VMware ESX with port-channel and beacon probes causes mac-flapping. This is actually a known cause and effect and I would even say that is the expected behavior. You can read more about it at VMware KB article [1012819](http://kb.vmware.com/kb/1012819). Granted the description of the root cause is not too in-depth, I decided to talk about it. If you want to know more about beacon probes I suggest reading VMware KB article [1005577](http://kb.vmware.com/kb/1005577) and the following VMware blog: [Beaconing Demystified: Using Beaconing to Detect Link Failures](http://blogs.vmware.com/networking/2008/12/using-beaconing-to-detect-link-failures-or-beaconing-demystified.html).  If we don't have any port-channel (port aggregation) setup on the upstream switch then we just send a beacon out of each physical NIC that is part of the port-group which has beacon probing enabled (each beacon has its own MAC address). The beacon is reached across the broadcast domain (so if it hits a router, it would stop there). It would kind of look like this with two uplinks (three uplinks are necessary for beacon to work properly, but this image is just for an example):

[![](http://virtuallyhyper.com/wp-content/uploads/2012/03/BP_No_Port_Channel.jpg)](http://virtuallyhyper.com/wp-content/uploads/2012/03/BP_No_Port_Channel.jpg)

So Beacon 1 (Blue) is sent out of one of the NICs and the other NIC receive it (in actuality the whole broadcast domain receives it, so any other host on that physical switch would also receive that beacon). Same thing for Beacon 2 (Red). Now if we throw a port-channel in there, it would look something like this:

[![](http://virtuallyhyper.com/wp-content/uploads/2012/03/BP_With_Port_Channel.jpg)](http://virtuallyhyper.com/wp-content/uploads/2012/03/BP_With_Port_Channel.jpg)

So now each beacon can and will be seen across both of the ports on the switch (for now, VMware's standard and distributed switches only support IP/hash for its ether-channel algorithm). Since each beacon has its own MAC address and the hashing algorithm is IP-based, the MAC address of the probes (not having any IP) will be seen across both ports of the switch. Some good work-arounds for this issue include Link-State Tracking (a person from above blog has a good example of this) here a link to his blog: [VMware and Link State Tracking](http://www.bctechnet.com/vmware-link-state-tracking/). If you are using blade enclosures, HP's provide SmartLink Functionality, and this page describes it in a very succinct way: [HP Virtual Connect Flex-10 Ethernet Module - SmartLink Feature Not Supported on Individual FlexNICs](http://h20000.www2.hp.com/bizsupport/TechSupport/Document.jsp?objectID=c01780345&lang=en&cc=us&taskId=&prodSeriesId=3794423&prodTypeId=3709945). Don't let the name scare you :). As long as you know what SmartLInk is after visiting that page, I will be more than happy :). If that one doesn't help out, check this one out: [HP Virtual Connect SmartLink](http://blog.michaelfmcnamara.com/2009/08/hp-virtual-connect-smart-link/).
