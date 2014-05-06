---
author: joe
comments: true
date: 2012-04-16 16:13:52+00:00
layout: post
slug: vlan-issue-with-ibm-vfa-be2net-driver-and-dvs
title: VLAN issue with IBM VFA, be2net driver, and DVS
wordpress_id: 494
categories:
- Networking
- VMware
following_users:
- jarret
- joechan
- kelatov
tags:
- be2net
- dvs
- ibm
- portgroups
- vfa
- vlan
- vmnic
---

I came across a really strange issue today with one of my customers. A VM would lose network connectivity every time more than 3 VLANs were trunked in a DVUplink portgroup on the VMware Distributed Virtual Switch. He was running an IBM Bladecenter blade server with the [Emulex 10GB Virtual Fabric Adapter (VFA)](http://www.amazon.com/Emulex-Virtual-Fabric-Adapter-System/dp/B003E7MNWE).


# Configuration




## Physical Switchport



```

switchport mode trunk

```



## DVS Configuration


[caption id="attachment_500" align="alignnone" width="223" caption="DVS Configuration"][![DVS Configuration](http://virtuallyhyper.com/wp-content/uploads/2012/03/Screenshot-3_26_2012-8_06_55-PM.png)](http://virtuallyhyper.com/wp-content/uploads/2012/03/Screenshot-3_26_2012-8_06_55-PM.png)[/caption]


## dvPortgroup Configuration


[caption id="attachment_499" align="alignnone" width="300" caption="dvPortgroup Configuration"][![dvPortgroup Configuration](http://virtuallyhyper.com/wp-content/uploads/2012/03/Screenshot-3_26_2012-8_06_28-PM-300x227.png)](http://virtuallyhyper.com/wp-content/uploads/2012/03/Screenshot-3_26_2012-8_06_28-PM.png)[/caption]


## dvUplink Portgroup Configuration


[caption id="attachment_501" align="alignnone" width="300" caption="DVUplink Portgroup Configuration"][![DVUplink Portgroup Configuration](http://virtuallyhyper.com/wp-content/uploads/2012/03/Screenshot-3_26_2012-8_07_53-PM-300x227.png)](http://virtuallyhyper.com/wp-content/uploads/2012/03/Screenshot-3_26_2012-8_07_53-PM.png)[/caption]

If we configured the dvUplink portgroup configuration to trunk 4 or more VLANs, the VMs using the VLAN-22 portgroup would lose network connectivity. Here are examples that would cause this issue:



	
  * 0-4094 (default)

	
  * 22,23,24,25

	
  * 20-24


Here are examples that would allow communication:

	
  * 22

	
  * 22,23,24

	
  * 21-23




# Diagnostics


Per the "Known Networking Issues" found in the [VMware vSphere® 5.0 Release Notes](https://www.vmware.com/support/vsphere5/doc/vsphere-esx-vcenter-server-50-release-notes.html#networkingissues):


> Using the bundled Emulex BE2/BE2 NICs (be2net driver)
When using vSphere 5.0 with Emulex BE2/BE3 NICs (be2net driver) in a HP FlexFabric/Flex-10 or IBM Virtual Fabric Adapter (VFA) environment, connectivity may not work properly on Windows VMs or the server when VLANs are configured.

Workaround: Do not use the driver bundled with vSphere 5.0. Before upgrading to vSphere 5.0, please obtain an updated driver from Emulex, HP, or IBM that should be used on HP FlexFabric/Flex-10 or IBM VFA systems.


I checked his driver version:


```

~ # ethtool -i vmnic2
driver: be2net
version: 4.0.355.1
firmware-version: 4.0.1062.0
bus-info: 0000:08:00.1

```


and per the [VMware hardware compatibility list](http://www.vmware.com/resources/compatibility/search.php), he was running the Inbox driver that came bundled with ESXi 5.

The latest version appears to be be2net version 4.1.334.0, but the download is nowhere to be found. We will be working with the hardware vendor to locate the latest driver version.

In parallel, to confirm that it is a problem with the be2net driver and/or the Virtual Fabric Adapter, we will also try to see if we can replicate the issue with his Broadcom adapters running the bnx2 driver.

UPDATE: The be2net driver version 4.1.334.0 (we found the link [here](http://downloads.vmware.com/d/details/dt_esxi50_emulex_be2net_413340/dHRAYndld3diZHAlZA )) did not resolve the issue, but a newer driver update from IBM fixed the issue.

It appears that this is an IBM known issue: [Traffic does not passing with Distributed Virtual Switch and Emulex VFA II when vNICs enabled on VFA II - Emulex 10 Gigabit Ethernet Virtual Fabric Adapter Advanced II for IBM BladeCenter](https://www-947.ibm.com/support/entry/portal/docdisplay?lndocid=migr-5089840).


> This behavior will be corrected in a future release of the Emulex VFA II driver, using one of the following versions or higher:

> 
> 
	
>   * ESX 4.x - 4.0.306.2
> 
	
>   * ESX 5.0 - 4.0.355.2
> 




It looks like the issue is resolved in a future release of the driver.
