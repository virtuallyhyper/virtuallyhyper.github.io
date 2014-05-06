---
author: karim
comments: true
date: 2012-04-09 00:42:24+00:00
layout: post
slug: why-scp-and-dd-from-an-esxi-host-are-not-good-performance-tools
title: Why SCP and DD from an ESX(i) Host are not Good Performance Tools
wordpress_id: 538
categories:
- Networking
- Storage
- VMware
following_users:
- kelatov
tags:
- dd
- io analyzer
- netperf
- performance
- scp
---

I am sure many have noticed that whenever you try to scp a file to or from an ESX(i) host the performance is a little slow. Let's take the following example, here is an scp between two hosts on the same switch utilizing links that are 1GB.

    
    ~# scp 1gb_File 10.10.10.10:/vmfs/volumes/datastore_1
    root@10.10.10.10's password:
    1gb_File                                       100% 1000MB  24.4MB/s   00:41


Now running a test with netperf from within two VMs on those two sames host, going through the same path (ie: using the same vmnics), we see the following:


```

~ # /tmp/netserver
Starting netserver at port 12865

~ # /tmp/netperf -H 10.10.10.11 -t TCP_STREAM -l 30
TCP STREAM TEST to 10.10.10.11
Recv   Send    Send
Socket Socket  Message  Elapsed
Size   Size    Size     Time     Throughput
bytes  bytes   bytes    secs.    10^6bits/sec
  65536  32768  32768    30.00     939.59

```


So we are capable of utilizing the full 1GB but the hypervisor is limiting the userworlds other than VMs (ie: dd, scp). I do understand that this is not an apples-to-apples comparison (scp is actually reading and writing data to and from the local disks) but we are seeing scp transferring at 195Mb/s while netperf reaches 939Mb/s.

This is by design and makes perfect sense. We want all our resources dedicated to the VMs and not to any of the tools within the hypervisor. There is a pretty good article entitled [What is the architecture of VMWare ESX like and how does it compare to other operating systems?](http://www.quora.com/What-is-the-architecture-of-VMWare-ESX-like-and-how-does-it-compare-to-other-operating-systems) and it talks about some of intricacies of the vmkernel. The same thing goes for 'dd', even using 'dd' from within a linux VM is not a good test. Physical Linux has a buffer cache so in the case of running dd, the writes are first written into memory and not doing real I/Os. In ESX, it does not cache writes at all to ensure data consistency, so all writes go to physical disks and therefore will be slower. Instead of using 'dd'  fire up a new VM and use IOmeter to bench test your hardware, another person blogged about this here: '[A couple of internal EMC+VMware threads worth sharing…](http://virtualgeek.typepad.com/virtual_geek/2012/03/a-couple-of-internal-emcvmware-threads-worth-sharing.html)'. There is actually a quote from an EMC engineer in that post. If you don't want to deal with the hassle of setting up a new VM, you can always use[ I/O Analyzer](http://labs.vmware.com/flings/io-analyzer). It's a VMware appliance that comes pre-installed with a Linux distro and has IOmeter preloaded on it. I might do a post on how to use that tool, as well. :)
