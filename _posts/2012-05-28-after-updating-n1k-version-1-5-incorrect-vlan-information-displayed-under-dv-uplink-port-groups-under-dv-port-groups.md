---
author: karim
comments: true
date: 2012-05-28 16:24:20+00:00
layout: post
slug: after-updating-n1k-version-1-5-incorrect-vlan-information-displayed-under-dv-uplink-port-groups-under-dv-port-groups
title: After Updating N1K to Version 1.5, Incorrect VLAN Information Displayed Under
  DV Uplink Port Groups but not Under DV Port Groups
wordpress_id: 1342
categories:
- Networking
- VMware
ef_usergroup:
- JJK
following_users:
- jarret
- kelatov
tags:
- DV Port Group
- DV Uplink Port Group
- N1K
- Nexus 1000v
- show tech-support svs
- VLAN ID
---

I had recently ran into an interesting issue with the nexus 1000v switch. After upgrading from 4.2(1)SV1(4a) to 4.2(1)SV1(5.1). Uplink ports are not reporting VLAN IDs correctly when viewed from the VSM->Uplink->Ports tab->VLAN-ID Column. However, the VLAN IDs ARE reporting correctly on the individual VM Distributed PortGroups (VSM->DV-PortGroup->Ports tab->VLAN-ID Column). Here is how the DV Uplink Port Group looked like:
[![](http://virtuallyhyper.com/wp-content/uploads/2012/04/uplink_1.jpg)](http://virtuallyhyper.com/wp-content/uploads/2012/04/uplink_1.jpg)
And here is how the DV Port Group looked like:
[![](http://virtuallyhyper.com/wp-content/uploads/2012/04/dportgroup_11.jpg)](http://virtuallyhyper.com/wp-content/uploads/2012/04/dportgroup_11.jpg)

We grabbed a '_show tech-support svs_' from the N1K. More information on what a '_show tech-support svs_' is, can be found at the following cisco [web-page](http://www.cisco.com/en/US/docs/switches/datacenter/nexus1000/sw/4_0/troubleshooting/configuration/guide/trouble_18b4contact.html). From the logs we saw the following messages that corresponded to the DV Uplink port Group:


```

Event:E_DEBUG, length:351, at 951023 usecs after Fri Apr 13 14:46:54 2012
[102] msp_vppm_handle_port_profile_update_resp(434): (ERR) profile DV_Uplink_PG update failed: Syserr code: 0x41b1001d,
user string: [port-profile alias 'DV_Uplink_PG' not included in current vCenter Server sync operation]
internal string: [port-profile alias 'DV_Uplink_PG' not included in current vCenter Server sync operation]

```


We contacted Cisco and they had identified a known bug on the N1K. If you have cisco account you can check out bug [CSCtz24512](http://tools.cisco.com/Support/BugToolKit/search/getBugDetails.do?method=fetchBugDetails&bugId=CSCtz24512). It's just a cosmetic bug and no network traffic is impacted by this issue. It looks like this also made it to the VMware communities ([2025321](http://communities.vmware.com/message/2025321))
