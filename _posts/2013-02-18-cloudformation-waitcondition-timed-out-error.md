---
author: joe
comments: true
date: 2013-02-18 07:57:09+00:00
layout: post
slug: cloudformation-waitcondition-timed-out-error
title: CloudFormation WaitCondition Timed Out Error
wordpress_id: 6326
categories:
- AWS
following_users:
- jarret
- joechan
- kelatov
tags:
- Amazon Machine Images
- AWS CloudFormation
---

_Update (05/14/13)_: Amazon no longer supports the AMI I had linked previously. They now bake the helper scripts into the [base Windows AMIs](https://aws.amazon.com/amis/microsoft-windows-server-2008-r2-base). If you are running an older AMI and don't have the helper scripts installed, you can install [them](http://docs.aws.amazon.com/AWSCloudFormation/latest/UserGuide/cfn-helper-scripts-reference.html) manually or just deploy a new Windows AMI from the AWS Management Console.





## CloudFormation WaitCondition Timed Out Error





I recently ran into an issue with a CloudFormation WaitCondition timed out message. The stack was failing to create with this error:




    

```
    #!
    WaitCondition timed out.
    Received 0 conditions when expecting 1
    
```






## How I got here





I had taken the [sample template](https://s3.amazonaws.com/cloudformation-templates-us-east-1/Windows_Single_Server_SharePoint_Foundation.template) taken from [Bootstrapping AWS CloudFormation Windows Stacks](http://docs.aws.amazon.com/AWSCloudFormation/latest/UserGuide/cfn-windows-stacks-bootstrapping.html) and modified it to use one of my custom AMIs.





## Why this happened





Looking at [Creating Wait Conditions in a Template](http://docs.aws.amazon.com/AWSCloudFormation/latest/UserGuide/using-cfn-waitcondition.html):




    

```
    "SharePointFoundationWaitCondition" : {
       "Type" : "AWS::CloudFormation::WaitCondition",
       "DependsOn" : "SharePointFoundation",
       "Properties" : {
         "Handle" : {"Ref" : "SharePointFoundationWaitHandle"},
         "Timeout" : "3600"
       }
    }
    
```






This means that the `WaitCondition` that is currently awaiting a signal from **cfn-signal.exe**, which comes from this line in [my template](https://s3.amazonaws.com/cloudformation-templates-us-east-1/Windows_Single_Server_SharePoint_Foundation.template):




    

```
    "cfn-signal.exe -e %ERRORLEVEL% ", { "Fn::Base64" : {
    "Ref" : "SharePointFoundationWaitHandle" }}, "\n"
    
```






Per the configuration here, the `WaitCondition` timed out after 3600 seconds without getting a signal.





It turns out **cfn-signal.exe** doesn't exist in my AMI because per [Bootstrapping AWS CloudFormation Windows Stacks](http://docs.aws.amazon.com/AWSCloudFormation/latest/UserGuide/cfn-windows-stacks-bootstrapping.html):





> 
  
> 
> The Amazon EBS-Backed Windows Server 2008 R2 English 64-bit - Base for CloudFormation AMI comes supplied with the AWS CloudFormation helper scripts pre-installed in the C:\Program Files (x86)\Amazon\cfn-bootstrap directory.
> 
> 






This template expects the helper scripts to be pre-installed on your AMI, but the AMI I launched with (from the AWS management console quick launch wizard) to create my custom AMI from did not come with the helper scripts pre-installed.





# Solution





I created my own AMI from the Amazon EBS-Backed Windows Server 2008 R2 English 64-bit - Base for CloudFormation AMI, installed my application, then installed the [helper scripts](http://docs.aws.amazon.com/AWSCloudFormation/latest/UserGuide/cfn-helper-scripts-reference.html). Afterwards, I created an AMI from that instance and used that as a base for my CloudFormation template (which expects the helper scripts and existence of **cfn-signal.exe** which now exist!).



