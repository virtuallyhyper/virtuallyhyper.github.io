---
author: joe
comments: true
date: 2012-04-17 00:58:48+00:00
layout: post
slug: against-best-practices-vmotion-and-management-on-the-same-network
title: 'Against Best Practices: vMotion and Management on the Same Network'
wordpress_id: 873
categories:
- Networking
- VMware
ef_usergroup:
- JJK
following_users:
- jarret
- joechan
- kelatov
tags:
- advanced configuration
- best practice
- management
- routing table
- tips
- vmk
- vmkernel
- vmotion
---

I recently spoke with someone who was running into the [vMotion Causes Unicast Flooding](http://virtuallyhyper.com/index.php/2012/03/vmotion-causes-unicast-flooding/) issue Karim blogged about.

As he states, it is mentioned in many of VMware's best practice documents that vMotion should on a seperate network devoted to vMotion (for example this [ESXi 4.1 server configuration guide]( http://www.vmware.com/pdf/vsphere4/r41/vsp_41_esxi_server_config.pdf)):


> Keep the vMotion connection on a separate network devoted to vMotion. When migration with vMotion
occurs, the contents of the guest operating system’s memory is transmitted over the network. You can do this either by using VLANs to segment a single physical network or separate physical networks (the latter is preferable).


and if you don't follow the best practices and place vMotion and Management traffic on the same network, you may see vMotion traffic getting sent out the Management interface.

If you do not want to adhere to best practices and would like to avoid that problem, one thing you can try is setting in the advanced settings of the ESX host (Configuration tab -> Advanced Settings -> Migrate), Migrate.BindToVmknic to 1.

For the folks who live in the command line, here is how you set Migrate.BindToVmknic to 1 via the console/SSH.


```

~ # esxcfg-advcfg -s 1 /Migrate/BindToVmknic
~ #

```


Verify that it is actually set properly:


```

~ # esxcfg-advcfg -g /Migrate/BindToVmknic
Value of BindToVmknic is 1

```


or


```

~ # grep Migrate /etc/vmware/esx.conf
/adv/Migrate/BindToVmknic = "1"

```


This should force the vMotion to go over the vmkernel port that vMotion is enabled on, rather than consulting the vmkernel routing table to determine which vmkernel interface to send traffic out.

Please keep in mind that this is still not something I would recommend. But if you have some reason where you ABSOLUTELY CAN NOT separate the vMotion network onto a different network, this little tip may help you configure your host so that vMotion will behave as expected.
