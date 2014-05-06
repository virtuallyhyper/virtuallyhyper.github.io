---
author: joe
comments: true
date: 2012-03-20 09:21:19+00:00
layout: post
slug: virtual-machine-loses-network-connectivity-after-vmotion
title: Virtual Machine Gets Disconnected After vMotion
wordpress_id: 162
categories:
- Networking
- VMware
following_users:
- joechan
tags:
- network
- troubleshooting
- vmnic
- vmotion
---

This appears to be a very common issue. The actual root causes vary, but hopefully this can help you isolate the issue.

A virtual machine typically loses network connectivity because it gets bound to a vmnic that does not have access to the network resources the VM requires.

If you are using "Route based on originating virtual port ID" or "Route based on source MAC hash", the VM can be bound to any one of the NICs in that team. If you are using "Route based on IP hash", the VM will send traffic out any of the vmnics depending on the source and destination IP addresses (details about that specific process can be found in VMware KB article [1007371](http://kb.vmware.com/kb/1007371)).

Any operation that brings the VM's virtual network adapter online can cause that virtual network adapter (vNIC) to bind to a new physical adapter (vmnic) on the vSwitch. Here are a few examples of actions that can cause this to occur:



	
  * VM powering on

	
  * connecting the virtual machine's network adapter (vNIC)

	
  * vMotion

	
  * Changing a vNIC's assigned portgroup




# Example


For my example, let's say you vMotioned a virtual machine, "Test", from ESX-A to ESX-B and the VM lost network connectivity.

If the issue is still present, you can SSH into ESX-B and run the following to determine which vmnic the "Test" virtual machine is currently being bound to:

- run the following command


```
esxtop
```


- type 'n' (for Networking)

You will be presented with a screen like this:


```
 1:07:14am up  2:33, 163 worlds; CPU load average: 0.01, 0.05, 0.02

   PORT-ID              USED-BY  TEAM-PNIC DNAME              PKTTX/s  MbTX/s    PKTRX/s  MbRX/s %DRPTX %DRPRX
  16777217           Management        n/a vSwitch0              0.00    0.00       0.00    0.00   0.00   0.00
  16777218               vmnic0          - vSwitch0             27.13    0.07       0.00    0.00   0.00   0.00
  16777219                 vmk0     vmnic0 vSwitch0             27.13    0.07       0.00    0.00   0.00   0.00
  33554433           Management        n/a vSwitch1              0.00    0.00       0.00    0.00   0.00   0.00
  33554434               vmnic2          - vSwitch1              0.00    0.00      27.13    0.07   0.00   0.00
  33554435               vmnic3          - vSwitch1              0.00    0.00       0.00    0.00   0.00   0.00
  33554436           21153:Test     vmnic3 vSwitch1              0.00    0.00       0.00    0.00   0.00   0.00
```


Under the USED-BY column, look for the name of your virtual machine. The TEAM-PNIC column will indicate the vmnic it is currently bound to (this does not work if you are using IP hash with port-channels).

In our case, our VM (Test), is bound to vmnic3.

Now, we should test connectivity with the other vmnic in the team. To explicitly bind the VM to another vmnic, you can specify explicit failover in the current portgroup or create a new portgroup for testing purposes and place the VM within it.

In the portgroup settings, you can set vmnic3 to unused and vmnic2 to active to force any VMs within the VM Network portgroup to bind to vmnic2. Here is an example:

[caption id="attachment_341" align="alignnone" width="518" caption="Portgroup Properties"][![Portgroup Properties](http://virtuallyhyper.com/wp-content/uploads/2012/03/ScreenClip.png)](http://virtuallyhyper.com/wp-content/uploads/2012/03/ScreenClip.png)[/caption]

If you regain connectivity, then chances are there is a problem communicating over vmnic2 and you should check your configuration and hardware for vmnic2.

These steps can be repeated for however many vmnics you have attached to the portgroup to isolate which adapters are having issues.
