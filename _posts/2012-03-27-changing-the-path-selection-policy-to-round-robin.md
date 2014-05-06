---
author: jarret
comments: true
date: 2012-03-27 03:03:29+00:00
layout: post
slug: changing-the-path-selection-policy-to-round-robin
title: Changing the Path Selection Policy to Round Robin
wordpress_id: 291
categories:
- Storage
- VMware
following_users:
- jarret
tags:
- Change PSP
- esxcli
- LUNs
- PSP
- Round Robin
- SATP
---

We are frequently asked the best way to change the default [Path Selection Policy](http://kb.vmware.com/kb/1011340) (PSP) for all LUNs to Round Robin. There are a few ways to accomplish this task.

Please note that Round Robin is not supported on LUNs containing [Microsoft Clustering Server (MSCS)](http://kb.vmware.com/kb/1037959) nodes or RDMs used for MSCS. [These LUNs can be set back to Fixed or MRU by specifying the PSP](http://kb.vmware.com/kb/1036189).


## Change the default PSP for an SATP


I prefer to change the default PSP for an Storage Array Type Plugin (SATP) since any new LUNs will automatically have the right PSP. It is more of a "setup and forget" type of solution rather than having to remember to change each LUN on each host. The largest problem with this method is that it only works on LUNs that have never had their PSP changed. That means that if a LUN has been specifically set to another PSP in the GUI or on the command line, that will take precedence over the default PSP. When a PSP is specified for a LUN, an entry is put into the esx.conf. Manually editing the esx.conf is dangerous and not advised. If you have never specified a PSP for a LUN, this procedure will automatically claim that LUN as the PSP we set.

VMware has a good [KB article](http://kb.vmware.com/kb/1017760) on how to accomplish this task. The problem is that it leaves out some vital steps to getting the existing LUNs to accept the new PSP.

First off we need to find out what our current LUNs are actually using.

**NOTE:** ESXi 5.x changed the structure of esxcli. In 5.x the commands will have a "storage" in them before nmp. They will look like the following.

    
    ~ # esxcli storage nmp device list


**NOTE:** ESX/ESXi 4.x does not have the extra storage in them. The command for esxcli is below. For the purposes of this example I will use the 5.x format. If you need to run this on 4.x please drop the "storage".

    
    ~ # esxcli nmp device list


The output will show all of the LUNs; the excerpt below is only from one of these LUN. We can see that this is claimed by the SATP VMW_SATP_ALUA and is using the VMW_PSP_MRU PSP. This is only one of the LUNs that I want to move to VMW_PSP_RR. From the command below, we need to note the PSP and SATP. I have only included the output from a single LUN below.

    
    ~ # esxcli storage nmp device list
    naa.600144f070cc440000004e9f25580001
       Device Display Name: NEXENTA iSCSI Disk (naa.600144f070cc440000004e9f25580001)
       Storage Array Type: <strong>VMW_SATP_ALUA</strong>
       Storage Array Type Device Config: {implicit_support=on;explicit_support=off; explicit_allow=on;alua_followover=on;{TPG_id=0,TPG_state=AO}}
       Path Selection Policy: <strong>VMW_PSP_MRU</strong>
       Path Selection Policy Device Config: Current Path=vmhba37:C1:T0:L10
       Path Selection Policy Device Custom Config:
       Working Paths: vmhba37:C1:T0:L10


Before changing the default PSP for VMW_SATP_ALUA to VMW_PSP_RR we need to know what the default PSP really is. It would seem that the default PSP is VMW_PSP_MRU, but let's confirm. The command below (remember that it would be "esxcli nmp satp list" in ESX/ESXi 4.x) shows us that the default PSP for VMW_SATP_ALUA is indeed VMW_PSP_MRU.

    
    ~ # esxcli storage nmp satp list
    Name                 Default PSP    Description
    -------------------  -------------  -------------------------------------------------------
    <strong>VMW_SATP_ALUA    VMW_PSP_MRU</strong>    Supports non-specific arrays that use the ALUA protocol
    VMW_SATP_MSA         VMW_PSP_MRU    Placeholder (plugin not loaded)
    VMW_SATP_DEFAULT_AP  VMW_PSP_MRU    Placeholder (plugin not loaded)
    VMW_SATP_SVC         VMW_PSP_FIXED  Placeholder (plugin not loaded)
    VMW_SATP_EQL         VMW_PSP_FIXED  Placeholder (plugin not loaded)
    VMW_SATP_INV         VMW_PSP_FIXED  Placeholder (plugin not loaded)
    VMW_SATP_EVA         VMW_PSP_FIXED  Placeholder (plugin not loaded)
    VMW_SATP_ALUA_CX     VMW_PSP_FIXED  Placeholder (plugin not loaded)
    VMW_SATP_SYMM        VMW_PSP_FIXED  Placeholder (plugin not loaded)
    VMW_SATP_CX          VMW_PSP_MRU    Placeholder (plugin not loaded)
    VMW_SATP_LSI         VMW_PSP_MRU    Placeholder (plugin not loaded)
    VMW_SATP_DEFAULT_AA  VMW_PSP_FIXED  Supports non-specific active/active arrays
    VMW_SATP_LOCAL       VMW_PSP_FIXED  Supports direct attached devices


Now it is time to change the default PSP for VMW_SATP_ALUA to VMW_PSP_RR.

    
    ~ # esxcli storage nmp satp  set  -s VMW_SATP_ALUA -P VMW_PSP_RR
    Default PSP for VMW_SATP_ALUA is now VMW_PSP_RR


Lets confirm that the the default PSP has changed.

    
    ~ # esxcli storage nmp satp  list
    Name                 Default PSP    Description
    -------------------  -------------  -------------------------------------------------------
    <strong>VMW_SATP_ALUA     VMW_PSP_RR</strong>     Supports non-specific arrays that use the ALUA protocol
    VMW_SATP_MSA         VMW_PSP_MRU    Placeholder (plugin not loaded)
    VMW_SATP_DEFAULT_AP  VMW_PSP_MRU    Placeholder (plugin not loaded)
    VMW_SATP_SVC         VMW_PSP_FIXED  Placeholder (plugin not loaded)
    VMW_SATP_EQL         VMW_PSP_FIXED  Placeholder (plugin not loaded)
    VMW_SATP_INV         VMW_PSP_FIXED  Placeholder (plugin not loaded)
    VMW_SATP_EVA         VMW_PSP_FIXED  Placeholder (plugin not loaded)
    VMW_SATP_ALUA_CX     VMW_PSP_FIXED  Placeholder (plugin not loaded)
    VMW_SATP_SYMM        VMW_PSP_FIXED  Placeholder (plugin not loaded)
    VMW_SATP_CX          VMW_PSP_MRU    Placeholder (plugin not loaded)
    VMW_SATP_LSI         VMW_PSP_MRU    Placeholder (plugin not loaded)
    VMW_SATP_DEFAULT_AA  VMW_PSP_FIXED  Supports non-specific active/active arrays
    VMW_SATP_LOCAL       VMW_PSP_FIXED  Supports direct attached devices


Now we have a few options. We can try to reclaim the LUNs, or we can reboot the ESX/ESXi host. The better way is to reboot the host. This would ensure that all LUNs are not in use and can change the PSP. LUNs with high I/O may not be able to do a reclaim.

A reclaim will unclaim and claim the LUNs by the ESX/ESXi host. That means that the host will temporarily drop the LUN from the Native Multipathing Plug-in (NMP) and then bring it in again. When a LUN is claimed by the NMP, it will be run against a set of rules to determine what the LUN is and how to handle it. One of those rules is the SATP, so when the LUN gets assigned the VMW_SATP_ALUA SATP, it will automatically get assigned to the PSP that we just set. In this case we need to unclaim the current LUNs and then claim them so that the new PSP will take affect. A reclaim will do both operations for us.

A reclaim can be done live, but I would recommend putting the host into Maintenance Mode (MM) before making any major changes on a host. I would personally recommend rebooting the host, but you can use the following command to reclaim all of the LUNs on the ESX/ESXi host. Instead of for loops, I opted for [xargs](http://unixhelp.ed.ac.uk/CGI/man-cgi?xargs).

    
    ~ # esxcfg-scsidevs -c | egrep -o "^naa.[0-9a-f]+" |xargs -n 1 esxcli storage core claiming reclaim -d


After the reboot (or reclaim) let's take a look at our LUN configuration. It is now showing that VMW_PSP_RR is the PSP that our LUN is using. New LUNs will use this PSP. Remember that any LUN that had another PSP specified will not be affected by this change.

    
    ~ # esxcli storage nmp device list -d naa.600144f070cc440000004e9f25580001
    naa.600144f070cc440000004e9f25580001
       Device Display Name: NEXENTA iSCSI Disk (naa.600144f070cc440000004e9f25580001)
       Storage Array Type: VMW_SATP_ALUA
       Storage Array Type Device Config: {implicit_support=on;explicit_support=off; explicit_allow=on;alua_followover=on;{TPG_id=0,TPG_state=AO}}
       Path Selection Policy: <strong>VMW_PSP_RR</strong>
       Path Selection Policy Device Config: {policy=rr,iops=1000,bytes=10485760,useANO=0;lastPathIndex=0: NumIOsPending=0,numBytesPending=0}
       Path Selection Policy Device Custom Config:
       Working Paths: vmhba37:C0:T0:L10, vmhba37:C1:T0:L10




## Select the PSP for each LUN


This can be done manually or with a script. The downside is that every LUN that has been changed has an entry in the esx.conf. This entry will take precedence over claimrules, so any desired changes to the PSP will need to be preformed manually. Also new LUNs will have to be changed unless the default PSP for the SATP has been changed as above.

This can be done through the GUI for each LUN on each host through the vCenter Client.



	
  1. Click the **Configuration** tab.

	
  2. In the left panel under Hardware Adapters, select **Storage Adapters**.

	
  3. On the right panel, select the vmhba that connects to the LUNs.

	
  4. Right-click the LUN whose path policy you want to change, and choose **Manage Paths**.

	
  5. In the resulting dialog box, under Policy, **set Path Selection** to Round Robin.


Manually doing this on all hosts for all LUNs can get very time consuming and tedious, so we can do it on the command line on each host.

First lets take a look at the LUNs that we have on the system. As above, I will be running the commands for ESXi 5.x, you can run these on ESX/ESXi 4.x by removing the "storage" after esxcli. The output has been truncated to only include a single LUN.

    
    ~ # esxcli storage nmp device list
    <strong>naa.600144f070cc440000004e9f25580001</strong>
       Device Display Name: NEXENTA iSCSI Disk (naa.600144f070cc440000004e9f25580001)
       Storage Array Type: VMW_SATP_ALUA
       Storage Array Type Device Config: {implicit_support=on;explicit_support=off; explicit_allow=on;alua_followover=on;{TPG_id=0,TPG_state=AO}}
       Path Selection Policy:<strong> VMW_PSP_MRU</strong>
       Path Selection Policy Device Config: Current Path=vmhba37:C1:T0:L10
       Path Selection Policy Device Custom Config:
       Working Paths: vmhba37:C1:T0:L10


We can see that it is set up for VMW_PSP_MRU currently. We want to change this to use VMW_PSP_RR so we can set it for this LUN by running the following command. We will use the naa provided above along with the PSP that we want to change it to.

    
    ~ # esxcli storage nmp device set --device naa.600144f070cc440000004e9f25580001 --psp VMW_PSP_RR


Lets confirm that the changes took. This time we will run the list command only on this LUN to avoid extraneous output.

    
    ~ # esxcli storage nmp device list -d naa.600144f070cc440000004e9f25580001
    naa.600144f070cc440000004e9f25580001
       Device Display Name: NEXENTA iSCSI Disk (naa.600144f070cc440000004e9f25580001)
       Storage Array Type: VMW_SATP_ALUA
       Storage Array Type Device Config: {implicit_support=on;explicit_support=off; explicit_allow=on;alua_followover=on;{TPG_id=0,TPG_state=AO}}
       Path Selection Policy: <strong>VMW_PSP_RR</strong>
       Path Selection Policy Device Config: {policy=rr,iops=1000,bytes=10485760,useANO=0;lastPathIndex=1: NumIOsPending=0,numBytesPending=0}
       Path Selection Policy Device Custom Config:
       Working Paths: vmhba37:C1:T0:L10, vmhba37:C1:T1:L10


Now we can see that the PSP has been set to VMW_PSP_RR. This can be tedious as well, so we can script it. As with "Changing the default PSP for an SATP", the steps below will set all LUNs from an array to Round Robin. If you are using MSCS, please [set LUNs can be set back to Fixed or MRU by specifying the PSP](http://kb.vmware.com/kb/1036189).

First thing we need to do is identify the LUNs that we want to set to Round Robin. We can do all LUNs, but it is way better to know exactly what will be set rather than just setting all LUNs. Each NAA (EUI/T10/SYM) is a unique identifier. Most arrays will pack in a serial number or a unique identifier for the array in the NAA. What we want to do is identify the part that is the same across all of the LUNs that we want to set. For my array (a home built Nexenta box) this is the beginning of the NAA.

The command below will take all LUNs presented to the ESX host that begin with naa and grab their naa. This list will contain all FC, iSCSI and SAS drives connected to the host. (Different array vendors will have a different unique identifier, this can be changed to a EUI,T10 or any other unique identifier)

    
    ~ # esxcfg-scsidevs -c |egrep -o "^naa.[0-9a-f]+"
    <strong>naa.600144f070cc440000004</strong>cd42fa10001
    <strong>naa.600144f070cc440000004</strong>cd432260002
    <strong>naa.600144f070cc440000004</strong>dd9c9310002
    <strong>naa.600144f070cc440000004</strong>dd9c9460003
    <strong>naa.600144f070cc440000004</strong>dd9c95f0004
    <strong>naa.600144f070cc440000004</strong>dd9e7c20005
    <strong>naa.600144f070cc440000004</strong>dda7ec40006
    <strong>naa.600144f070cc4400</strong>11104dda7ec40006


Above we can see that naa.600144f070cc440000004 is consistent across all of the LUNs that we want to set. naa.600144f070cc440011104dda7ec40006 is not a LUN that we want to set as Round Robin (it is a shared LUN between two cluster VMs in my environment). If we use naa.600144f070cc440000004 as part of the pattern match it will not match naa.600144f070cc440011104dda7ec40006, so we can set all LUNs that begin with naa.600144f070cc440000004. This will be different in every environment.

Now that we have a pattern match, we can now set all of the LUNs with a command. There are MANY ways to do this, but I prefer [xargs](http://unixhelp.ed.ac.uk/CGI/man-cgi?xargs).

    
    ~ # esxcfg-scsidevs -c |egrep -o "^naa.[0-9a-f]+" |grep naa.600144f070cc440000004 |xargs -n 1 esxcli storage nmp device set --psp VMW_PSP_RR -d


Now that we have specifically set every one of our pattern matching LUN to use RoundRobin, let's confirm on a single LUN.

    
    ~ # esxcli storage nmp device list -d naa.600144f070cc440000004e9f25580001
    naa.600144f070cc440000004e9f25580001
       Device Display Name: NEXENTA iSCSI Disk (naa.600144f070cc440000004e9f25580001)
       Storage Array Type: VMW_SATP_ALUA
       Storage Array Type Device Config: {implicit_support=on;explicit_support=off; explicit_allow=on;alua_followover=on;{TPG_id=0,TPG_state=AO}}
       Path Selection Policy: <strong>VMW_PSP_RR</strong>
       Path Selection Policy Device Config: {policy=rr,iops=1000,bytes=10485760,useANO=0;lastPathIndex=1: NumIOsPending=0,numBytesPending=0}
       Path Selection Policy Device Custom Config:
       Working Paths: vmhba37:C1:T0:L10, vmhba37:C1:T1:L10


That looks good. Let's confirm across all LUNs. All of the LUNs are set for RoundRobin, except for the LUN that we excluded (It did not match our pattern).

    
    ~ # esxcli storage nmp device list |grep "Path Selection Policy:" |sort |uniq -c
    <strong>1</strong>    Path Selection Policy: <strong>VMW_PSP_FIXED</strong>
    <strong>7</strong>    Path Selection Policy: <strong>VMW_PSP_RR</strong>


New LUNs will have to be manually changed, or the command above can be run again when new LUNs are presented. This command specifies the PSP for this LUN, so it will put an entry in the /etc/vmware/esx.conf. The entry looks like the one below.

    
    /storage/plugin/NMP/config[VMW_SATP_ALUA]/defaultpsp = "VMW_PSP_RR"
    /storage/plugin/NMP/device[naa.600144f070cc440000004dda7ec40006]/psp = "VMW_PSP_RR"
