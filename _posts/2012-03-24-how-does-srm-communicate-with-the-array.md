---
author: jarret
comments: true
date: 2012-03-24 00:19:09+00:00
layout: post
slug: how-does-srm-communicate-with-the-array
title: How does SRM communicate with the array?
wordpress_id: 184
categories:
- SRM
- VMware
following_users:
- jarret
tags:
- command line
- command.pl
- discoverArrays
- discoverDevices
- logs
- sra
- srm
- testFailoverStart
- xml
---

We often get questions on how SRM communicates with the array. I figured that it was time to do a quick write up of the process and some of the basic commands that we will see in the SRM logs. 

The [Storage Replication Adapter](https://www.vmware.com/support/srm/srm-storage-partners.html) (SRA) required for [Site Recovery Manager](http://www.vmware.com/products/site-recovery-manager/overview.html) (SRM) with array based replication is responsible for communication between the SRM service and the array. Since SRM supports many vendors, a standard interface was created for communication to the array. VMware published an SRA spec that all certified SRA must adhere to. The version 2, which is required for SRM 5, has many new features that allow for things like [Reprotect](http://blogs.vmware.com/uptime/2011/07/failback-where-is-the-button.html).

When the SRM service starts, it will load the SRA. SRM will call a queryCapabilities command to get a list of the commands that are available to SRM. The way it was implemented was to use XML for data in and out of the SRA. The command.pl is the executable that is responsible for handling communication. (The .pl extension on command.pl may be a bit misleading. Most vendors did not actually write the SRA in perl, but it is part of the SRA v2 specification. So the command.pl is the front end interface for the SRA.)

In order to figure out what the SRA can support, SRM will call queryCapabilities to get the capabilities of the array. An XML document is sent in to every command and an XML document is expected as the return data from every command. Below is the sequence of a full command. All of this information can be found in the [SRM Logs](http://kb.vmware.com/kb/1021802) after the SRM service is started. 

Call command.pl with a command and send in an XML document.


```
2012-03-08T21:40:14.802+02:00 [06052 info 'Storage'] Command line for queryCapabilities: "C:Program Files (x86)VMwareVMware vCenter Site Recovery Managerextern
alperl-5.8.8binperl.exe" "C:/Program Files (x86)/VMware/VMware vCenter Site Recovery Manager/storage/sra/EMC Symmetrix/command.pl"
2012-03-08T21:40:14.802+02:00 [06052 verbose 'Storage'] Input for queryCapabilities:
--> <?xml version="1.0" encoding="UTF-8"?>
--> <Command xmlns="http://www.vmware.com/srm/sra/v2">
-->   <Name>queryCapabilities</Name>
-->   <OutputFile>C:UsersVCENTE~1AppDataLocalTempvmware-vcentersrvsra-output-4-0</OutputFile>
-->   <StatusFile>C:UsersVCENTE~1AppDataLocalTempvmware-vcentersrvsra-status-5-0</StatusFile>
-->   <LogLevel>verbose</LogLevel>
-->   <LogDirectory>C:ProgramDataVMwareVMware vCenter Site Recovery ManagerLogsSRAsEMC Symmetrix</LogDirectory>
--> </Command>
```


After the command is sent in, SRM will wait for a response. It will listen on a file to see when it is updated. The command above came back with the capabilities of this SRA.


```
2012-03-08T21:40:15.161+02:00 [06052 info 'Storage'] queryCapabilities exited with exit code 0
2012-03-08T21:40:15.161+02:00 [06052 verbose 'Storage'] queryCapabilities responded with:
--> <?xml version="1.0" encoding="UTF-8" ?>
--> <Response xmlns="http://www.vmware.com/srm/sra/v2">
-->   <AdapterCapabilities>
-->     <ReplicationSoftwares>
-->       <ReplicationSoftware>
-->         <Name>SRDF</Name>
-->         <Version>5671</Version>
-->       </ReplicationSoftware>
-->       <ReplicationSoftware>
-->         <Name>SRDF</Name>
-->         <Version>5771</Version>
-->       </ReplicationSoftware>
-->       <ReplicationSoftware>
-->         <Name>SRDF</Name>
-->         <Version>5773</Version>
-->       </ReplicationSoftware>
-->       <ReplicationSoftware>
-->         <Name>SRDF</Name>
-->         <Version>5874</Version>
-->       </ReplicationSoftware>
-->       <ReplicationSoftware>
-->         <Name>SRDF</Name>
-->         <Version>5875</Version>
-->       </ReplicationSoftware>
-->     </ReplicationSoftwares>
-->     <ArrayModels>
-->       <ArrayModel>
-->         <Name>DMX-1</Name>
-->         <Vendor stringId="AdapterVendor">EMC Symmetrix</Vendor>
-->       </ArrayModel>
-->       <ArrayModel>
-->         <Name>DMX-2</Name>
-->         <Vendor stringId="AdapterVendor">EMC Symmetrix</Vendor>
-->       </ArrayModel>
-->       <ArrayModel>
-->         <Name>DMX-3</Name>
-->         <Vendor stringId="AdapterVendor">EMC Symmetrix</Vendor>
-->       </ArrayModel>
-->       <ArrayModel>
-->         <Name>DMX-4</Name>
-->         <Vendor stringId="AdapterVendor">EMC Symmetrix</Vendor>
-->       </ArrayModel>
-->       <ArrayModel>
-->         <Name>VMAX-1</Name>
-->         <Vendor stringId="AdapterVendor">EMC Symmetrix</Vendor>
-->       </ArrayModel>
-->     </ArrayModels>
-->     <Features>
-->       <MultiArrayDiscovery />
-->       <ConsistencyGroups />
-->       <DeviceIdentification>wwn</DeviceIdentification>
-->       <Protocols>
-->         <Protocol>FC</Protocol>
-->       </Protocols>
-->     </Features>
-->     <Commands>
-->       <Command name="queryInfo" />
-->       <Command name="queryErrorDefinitions" />
-->       <Command name="queryCapabilities" />
-->       <Command name="queryConnectionParameters" />
-->       <Command name="discoverArrays" />
-->       <Command name="discoverDevices" />
-->       <Command name="syncOnce">
-->         <ExecutionLocation>source</ExecutionLocation>
-->       </Command>
-->       <Command name="querySyncStatus" />
-->       <Command name="checkTestFailoverStart" />
-->       <Command name="testFailoverStart" />
-->       <Command name="testFailoverStop" />
-->       <Command name="checkFailover" />
-->       <Command name="prepareFailover" />
-->       <Command name="failover" />
-->       <Command name="reverseReplication">
-->         <ExecutionLocation>source</ExecutionLocation>
-->       </Command>
-->       <Command name="restoreReplication">
-->         <ExecutionLocation>source</ExecutionLocation>
-->       </Command>
-->     </Commands>
-->   </AdapterCapabilities>
--> </Response>
2012-03-08T21:40:15.177+02:00 [06052 verbose 'Storage'] XML validation succeeded

```


The SRM to SRA communication will work in the same way for all of the commands that the SRA is capable of. There are many commands that are used for back end communication and service querying. I will go over some of the ones that we see daily.

The queryErrorDefinitions returns a list of error codes to definitions. This list gets printed in the [SRM logs](http://kb.vmware.com/kb/1021802) for assistance with troubleshooting these response codes. The error codes are sent to SRM through exit codes. 

```
<Command name="queryErrorDefinitions" />
```


discoverArrays is a command that will list the parameters of an array and its array peers. It is called when adding an array to array managers and it is also called when pairing multiple arrays. 

```
<Command name="discoverArrays" />
```


A typical output XML file from discoverArrays is below. We can see that it lists the array ID, vendor, replication software and version. For the purposes of pairing arrays it returns the ID of arrays that it replicate to it or it replicates to. 


```
2012-03-08T21:40:30.964+02:00 [02768 info 'SraCommand' opID=656d3ec] discoverArrays exited with exit code 0
2012-03-08T21:40:30.964+02:00 [02768 verbose 'SraCommand' opID=656d3ec] discoverArrays responded with:
--> <?xml version="1.0" encoding="UTF-8" ?>
--> <Response xmlns="http://www.vmware.com/srm/sra/v2">
-->   <Arrays>
-->     <Array id="000292601234">
-->       <Name>000292601234</Name>
-->       <Model>
-->         <Name>VMAX-1</Name>
-->         <Vendor>EMC Corp</Vendor>
-->       </Model>
-->       <ReplicationSoftware>
-->         <Name>SRDF</Name>
-->         <Version>5875</Version>
-->       </ReplicationSoftware>
-->       <PeerArrays>
-->         <PeerArray id="000292602345" />
-->       </PeerArrays>
-->     </Array>
-->   </Arrays>
--> </Response>

```


discoveryDevices (discoverLuns in SRA v1) returns a list of replicated LUNs. This is called on array pairing and when the "Recomputing Datastore Groups" task is showing in vCenter.  It is a good place to start if there is missing LUNs/datastores in SRM. 

```
<Command name="discoverDevices" />
```


syncOnce is a new feature in SRM 5 that allows for SRM to force a sync (single replication). You can see the check box at the bottom of the Recovery Test and Planned Migration menu.

```
<Command name="syncOnce">
```


testFailoverStart is a command that asks the array to present pseudo LUNs to the hosts. The XML input contains LUNs and initiators to present the pseudo LUNs to. The back end technology will be different depending on the vendor.

```
<Command name="testFailoverStart" />
```


failover is the actual failover command. This command is sent when a recovery is run. It will break replication and promote the LUNs on the DR site. The LUNs will then be presented to the ESX hosts on the DR site.

```
<Command name="failover" />
```


reverseReplication is a new command that is built in for reprotect. This command is sent when a reprotect is requested for the GUI. It will reverse the replication of a LUN. 

```
<Command name="reverseReplication">
```

