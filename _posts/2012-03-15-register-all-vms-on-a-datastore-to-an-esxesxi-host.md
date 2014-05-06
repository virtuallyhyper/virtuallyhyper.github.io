---
author: jarret
comments: true
date: 2012-03-15 22:34:35+00:00
layout: post
slug: register-all-vms-on-a-datastore-to-an-esxesxi-host
title: Register All VMs on a Datastore to an ESX/ESXi host
wordpress_id: 284
categories:
- VMware
- vTip
following_users:
- jarret
tags:
- cli
- register vm
- vtip
---

### ESXi


Register all VMs on a single datastore.

    
    ~ # find /vmfs/volumes/<datastorename>/ -maxdepth 2 -name '*.vmx' -exec vim-cmd solo/registervm "{}" \;


Register all VMs on all datastores.

    
    ~ # find /vmfs/volumes/ -maxdepth 3 -name '*.vmx' -exec vim-cmd solo/registervm "{}" \;




### ESX


Register all VMs on a single datastore.

    
    ~ # find /vmfs/volumes/<datastorename>/ -maxdepth 2 -name '*.vmx' -exec vmware-cmd -s register "{}" \;


Register all VMs on all datastores.

    
    ~ # find /vmfs/volumes/ -maxdepth 3 -name '*.vmx' -exec vmware-cmd -s register "{}" \;
