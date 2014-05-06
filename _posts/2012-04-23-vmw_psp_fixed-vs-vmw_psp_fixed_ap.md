---
author: jarret
comments: true
date: 2012-04-23 22:05:46+00:00
layout: post
slug: vmw_psp_fixed-vs-vmw_psp_fixed_ap
title: VMW_PSP_FIXED vs. VMW_PSP_FIXED_AP
wordpress_id: 583
categories:
- Storage
- VMware
ef_usergroup:
- JJK
following_users:
- jarret
- kelatov
tags:
- fixed
- psa
- PSP
- SATP
- VMW_SATP_FIXED
- VMW_SATP_FIXED_AP
- vtip
---

Recently a vendor asked me what the difference between VMW_PSP_FIXED and VMW_PSP_FIXED_AP is. Since VMW_PSP_FIXED_AP is not specifically listed on the [HCL](http://vmware.com/go/hcl), the vendor was confused on why the SATP had automatically selected VMW_PSP_FIXED_AP instead of VMW_PSP_FIXED.

VMW_PSP_FIXED_AP was first implemented in ESX/ESXi 4.1 and in 5.x it was merged into the main VMW_PSP_FIXED. The difference here is that VMW_PSP_FIXED_AP has added features for [ALUA](http://www.yellow-bricks.com/2009/09/29/whats-that-alua-exactly/). VMW_PSP_FIXED_AP will act as VMW_PSP_FIXED when Array Preference is not implemented.

[PSPs are Path Selection Policies](http://kb.vmware.com/kb/1011340). All that they do is decide which path to use for I/O given a set of paths. These are implemented to be simple as they have to be used with thousands of different arrays. VMW_PSP_FIXED and VMW_PSP_FIXED_AP use a priority system to determine the path to be used.

VMW_PSP_FIXED_AP will use the following criteria when selecting a path.

1. User Preferred Path.
2. Array Path Preference. If the array does not implement this, this will be skipped.
3. Current path state.

The great thing about this is that the array should not let the host use an active non-optimized path.

The real difference is that VMW_PSP_FIXED does not implement #2 and VMW_PSP_FIXED_AP also implements [follow-over](http://blogs.vmware.com/vsphere/2012/02/configuration-settings-for-alua-devices.html). Asymmetric state change is explained [here](http://deinoscloud.wordpress.com/2011/07/04/it-all-started-with-this-question/) in much more detail.
