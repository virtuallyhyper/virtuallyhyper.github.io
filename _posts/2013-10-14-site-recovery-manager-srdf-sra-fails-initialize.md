---
author: jarret
comments: true
date: 2013-10-14 02:30:47+00:00
layout: post
slug: site-recovery-manager-srdf-sra-fails-initialize
title: Site Recovery Manager SRDF SRA Fails to Initialize
wordpress_id: 9553
categories:
- SRM
- VMware
ef_usergroup:
- JJK
following_users:
- jarret
- kelatov
tags:
- sra
- srdf
- srm
---

While configuring a Site Recovery Manager (SRM) instance for a customer, I ran into an issue with the EMC SRDF 5.1.0.0 SRA. After installing Solutions Enabler and then installing the SRA, everything went smooth until I did a _rescan for SRAs_ in the SRM user interface. After the rescan, the SRDF adapter did not show up, but had the following error instead.





> 
  
> 
> Failed to load SRA from 'D:/Program Files/VMware/VMware vCenter Site Recovery Manager/storage/sra/EMC Symmetrix'. SRA command 'queryInfo' failed. Internal error: 'C:/Program' is not recognized as an internal or external command, operable program or batch file.
> 
> 






Right away we know that it has to do with the installation path. From the output of the error, we can see that I installed SRM to the *D:* drive instead of the *C:* drive. This has been known to cause problems, but it is a standard practice for this customer and we wanted to follow it. The Mirrorview SRA works fine with this configuration.





So I jumped into the logs at _C:\ProgramData\VMware\VMware Site Recovery Manager\logs_ and opened the most recent log. After scanning through the log, I saw the following.




    

```
    2013-09-19T16:11:57.583-04:00 [04792 info 'Storage' opID=E910786E-00000014] Command line for queryInfo: "D:\Program Files\VMware\VMware vCenter Site Recovery Manager\external\perl-5.14.2\bin\perl.exe" "D:/Program Files/VMware/VMware vCenter Site Recovery Manager/storage/sra/EMC Symmetrix/command.pl"
    2013-09-19T16:11:57.583-04:00 [04792 verbose 'Storage' opID=E910786E-00000014] Input for queryInfo:
    --> <?xml version="1.0" encoding="UTF-8"?>
    --> <Command xmlns="http://www.vmware.com/srm/sra/v2">
    -->   <Name>queryInfo</Name>
    -->   <OutputFile>C:\Windows\TEMP\vmware-SYSTEM\sra-output-0-145</OutputFile>
    -->   <StatusFile>C:\Windows\TEMP\vmware-SYSTEM\sra-status-1-191</StatusFile>
    -->   <LogLevel>verbose</LogLevel>
    -->   <LogDirectory>C:\ProgramData\VMware\VMware vCenter Site Recovery Manager\Logs\SRAs\EMC Symmetrix</LogDirectory>
    --> </Command>
    2013-09-19T16:11:57.662-04:00 [04792 verbose 'SysCommandWin32' opID=E910786E-00000014] Starting process: "D:\Program Files\VMware\VMware vCenter Site Recovery Manager\external\perl-5.14.2\bin\perl.exe" "D:/Program Files/VMware/VMware vCenter Site Recovery Manager/storage/sra/EMC Symmetrix/command.pl"
    2013-09-19T16:11:57.708-04:00 [04792 verbose 'Storage' opID=E910786E-00000014] Resetting SRA command timeout to '300' seconds in the future
    2013-09-19T16:11:57.708-04:00 [01728 info 'Default'] Thread attached
    2013-09-19T16:11:57.708-04:00 [01728 info 'ThreadPool'] Thread enlisted
    2013-09-19T16:11:57.740-04:00 [04792 verbose 'Storage' opID=E910786E-00000014] Listening for updates to file 'C:\Windows\TEMP\vmware-SYSTEM\sra-status-1-191'
    2013-09-19T16:11:57.927-04:00 [02996 verbose 'DrTask' opID=E910786E-00000014] Created VC task 'com.vmware.vcDr.dr.storage.StorageManager.reloadAdapters:task-17'
    2013-09-19T16:11:59.271-04:00 [02148 error 'Storage' opID=E910786E-00000014] queryInfo's stderr:
    --> 'C:/Program' is not recognized as an internal or external command,
    --> operable program or batch file.
    --> 
    
```






This matches up with what displayed in the vSphere Client, but it does give us a more information. SRM is calling `"D:\Program Files\VMware\VMware vCenter Site Recovery Manager\external\perl-5.14.2\bin\perl.exe" "D:/Program Files/VMware/VMware vCenter Site Recovery Manager/storage/sra/EMC Symmetrix/command.pl"` with the input XML file that is listed below. So we know that the _C:\Program_ error is coming from something downstream. That would mean that the SRA it's self has some static paths built into it. I ran the command in a new **cmd prompt** to verify the same issue.




    

```
    > "D:\Program Files\VMware\VMware vCenter Site Recovery Manager\external\perl-5.14.2\bin\perl.exe" "D:/Program Files/VMware/VMware vCenter Site Recovery Manager/storage/sra/EMC Symmetrix/command.pl"
    'C:/Program' is not recognized as an internal or external command, operable program or batch file.
    
```






So we received the error message as SRM did when it called the SRA. So the next thing to try is to open up the _command.pl_ perl script to see what is going on. The _command.pl_ is the interface to which SRM makes calls. More about the SRA process is described in [this article](http://virtuallyhyper.com/2012/03/how-does-srm-communicate-with-the-array/).





I opened up the _command.pl_ and found that there is a static path to `C:\Program Files (x86)\VMware\VMware vCenter Site Recovery
Manager\storage\sra\EMC Symmetrix\`. I updated this to point to the correct path `D:\Program Files (x86)\VMware\VMware vCenter Site Recovery
Manager\storage\sra\EMC Symmetrix\`, but found the same error when running the command manually, except it was _D:\Program_ this time.




    

```
    > "D:\Program Files\VMware\VMware vCenter Site Recovery Manager\external\perl-5.14.2\bin\perl.exe" "D:/Program Files/VMware/VMware vCenter Site Recovery Manager/storage/sra/EMC Symmetrix/command.pl"
    'D:/Program' is not recognized as an internal or external command, operable program or batch file.
    
```






After a little more research I found [this blog article](http://blog.stfu.se/?p=308) describing a similar issue. They seemed to find a workaround by putting the binaries in the correct _C:_ location and then installing it back to the *D:* drive. Unfortunately I doubt that EMC would support this in production, so I did not implement it in the customers environment. I went back and reinstalled SRM, Soultions Enabler, and the SRDF SRA on the *C:* drive. After installing everything back on the _C:_ drive, everything worked alright for SRDF.



