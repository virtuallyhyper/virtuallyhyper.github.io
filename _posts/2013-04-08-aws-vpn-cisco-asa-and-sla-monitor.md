---
title: AWS VPN Cisco ASA and SLA Monitor
author: Joe Chan
layout: post
permalink: "/2013/04/aws-vpn-cisco-asa-and-sla-monitor/"
dsq_thread_id: 
  - 1408551340
categories: 
  - AWS
tags: 
  - aws
  - Cisco ASA
  - IPSec
published: true
---

2015-01-29 Updates: Setting up SLA monitors to the `169.254.x.x` addresse(s) and the VPC default gateway at the base of the VPC network range “plus one” (i.e. `10.0.0.1` for `10.0.0.0/16` networks) is no longer recommended.

When configuring the AWS VPC VPN with a Cisco ASA, Amazon recommends that you configure SLA monitoring.

In the pregenerated Cisco ASA configuration downloaded from the AWS VPN Management console (In your AWS VPC Management Console, click on **VPN Connections**, Right Click on your VPN connection, and click **Download Configuration**), you&#8217;ll see something similar to [the example config](http://docs.aws.amazon.com/AmazonVPC/latest/NetworkAdminGuide/Cisco_ASA.html#Cisco_ASA_details). In the config, you may see some lines like this:

    ! In order to keep the tunnel in an active or always up state, the ASA needs to send traffic to the subnet
    ! defined in acl-amzn. SLA monitoring can be configured to send pings to a destination in the subnet and
    ! will keep the tunnel active. This traffic needs to be sent to a target that will return a response.
    ! This can be manually tested by sending a ping to the target from the ASA sourced from the outside interface.
    ! A possible destination for the ping is an instance within the VPC. For redundancy multiple SLA monitors 
    ! can be configured to several instances to protect against a single point of failure.
    !
    ! The monitor is created as #1, which may conflict with an existing monitor using the same
    ! number. If so, we recommend changing the sequence number to avoid conflicts.
    !
    sla monitor 1
       type echo protocol ipIcmpEcho sla_monitor_address interface outside_interface
       frequency 5
    exit
    sla monitor schedule 1 life forever start-time now


Per the <a href="http://docs.aws.amazon.com/AmazonVPC/latest/NetworkAdminGuide/Cisco_ASA_Troubleshooting.html" onclick="javascript:_gaq.push(['_trackEvent','outbound-article','http://docs.aws.amazon.com/AmazonVPC/latest/NetworkAdminGuide/Cisco_ASA_Troubleshooting.html']);">Troubleshooting Cisco ASA Customer Gateway Connectivity doc</a>, this is to keep the IPsec tunnel active:

> In Cisco ASA, the IPsec will only come up after &#8220;interesting traffic&#8221; is sent. To always keep the IPsec active, we recommend configuring SLA monitor. SLA monitor will continue to send interesting traffic, keeping the IPsec active.

# Why do AWS docs suggest to use the SLA monitor?

From <a href="http://www.cisco.com/en/US/docs/ios/12_4/ip_sla/configuration/guide/hsicmp.html" onclick="javascript:_gaq.push(['_trackEvent','outbound-article','http://www.cisco.com/en/US/docs/ios/12_4/ip_sla/configuration/guide/hsicmp.html']);">this doc</a>, the SLA Monitor feature doesn&#8217;t appear to be designed specifically to keep IPsec tunnels up, but the attributes in *bold* below seem to make it a perfect tool of choice (rather than setting up another device specifically for this purpose).

> IP SLAs uses active traffic monitoring—**the generation of traffic in a continuous, reliable, and predictable manner**—for measuring network performance

# What is considered *interesting traffic*?

From this <a href="http://www.ciscopress.com/articles/article.asp?p=24833&seqNum=6" onclick="javascript:_gaq.push(['_trackEvent','outbound-article','http://www.ciscopress.com/articles/article.asp?p=24833&seqNum=6']);">Cisco press article</a>, it&#8217;s traffic that matches the ACL applied to the crypto map:

> Step 1: Interesting traffic initiates the IPSec process—Traffic is deemed interesting when the IPSec security policy configured in the IPSec peers starts the IKE process.
> 
> #### Step 1: Defining Interesting Traffic
> 
> Determining what type of traffic is deemed interesting is part of formulating a security policy for use of a VPN. The policy is then implemented in the configuration interface for each particular IPSec peer. For example, in Cisco routers and PIX Firewalls, access lists are used to determine the traffic to encrypt. The access lists are assigned to a crypto policy such that permit statements indicate that the selected traffic must be encrypted, and deny statements can be used to indicate that the selected traffic must be sent unencrypted. With the Cisco Secure VPN Client, you use menu windows to select connections to be secured by IPSec. When interesting traffic is generated or transits the IPSec client, the client initiates the next step in the process, negotiating an IKE phase one exchange.

On the ASA, it&#8217;s comes from lines like this in the config (note that by default the `access-list` line is commented out):

    crypto map amzn-vpn-map 1 match address acl-amzn
    access-list amzn-filter extended permit ip vpc_subnet vpc_subnet_mask local_subnet local_subnet_mask


# What should you use as the SLA Monitor target?

From the configuration above, there are 3 recommendations for a good SLA monitor target:

1. > This traffic needs to be sent to a target that will return a response.
1. > A possible destination for the ping is an instance within the VPC.
1. > For redundancy multiple SLA monitors can be configured to several instances to protect against a single point of failure.

As such, I would recommend configuring 2 VPC instances that respond to pings from the ASA along with 2 SLA monitors, one targeting each of the instances.

<p class="wp-flattr-button">
  <a class="FlattrButton" style="display:none;" href="http://virtuallyhyper.com/2013/04/aws-vpn-cisco-asa-and-sla-monitor/" title=" AWS VPN Cisco ASA and SLA Monitor" rev="flattr;uid:virtuallyhyper;language:en_GB;category:text;tags:aws,Cisco ASA,IPSec,blog;button:compact;">When configuring the AWS VPC VPN with a Cisco ASA, Amazon recommends that you configure SLA monitoring. In the pregenerated Cisco ASA configuration downloaded from the AWS VPN Management console...</a>
</p>