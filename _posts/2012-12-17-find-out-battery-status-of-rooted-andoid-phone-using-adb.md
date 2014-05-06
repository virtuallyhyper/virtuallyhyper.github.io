---
author: karim
comments: true
date: 2012-12-17 05:23:09+00:00
layout: post
slug: find-out-battery-status-of-rooted-andoid-phone-using-adb
title: Find Out Battery Status of Rooted Andoid Phone Using ADB
wordpress_id: 5142
categories:
- Home Lab
following_users:
- jarret
- kelatov
tags:
- /sys/class/power_supply/battery/batt_attr_text
- adb
- Android
- battery status
- cat
- dumpsys
---

There are two ways to go about finding out battery status of a rooted Android phone. One is simple and uses _cat_, the other is android specific. Here is how the output of the first one looks like:


```

[elatov@klaptop platform-tools]$ ./adb shell cat /sys/class/power_supply/battery/batt_attr_text
vref: 1248
batt_id: 4
batt_vol: 4155
batt_current: 229
batt_discharge_current: 0
batt_temperature: 391
Pd_M: 0
I_MBAT: 229
percent_last(RP): 99
percent_update: 90
level: 90
first_level: 15
full_level: 100
capacity: 1520
charging_source: USB
charging_enabled: Slow

```


That gives most of the battery information. If you just want to find out the current percentage, you can always do this:


```

[elatov@klaptop platform-tools]$ ./adb shell cat /sys/class/power_supply/battery/batt_attr_text | grep ^level
level: 92

```


Another way is using the _dumpsys_ utility, like so:


```

[elatov@klaptop platform-tools]$ ./adb shell dumpsys battery
Current Battery Service state:
  AC powered: false
  USB powered: true
  status: 2
  health: 2
  present: true
  level: 92
  scale: 100
  voltage:4191
  temperature: 348
  technology: Li-ion

```


This is a little bit more organized. Again, if you want to just see the percentage then _grep_ is your friend:


```

[elatov@klaptop platform-tools]$ ./adb shell dumpsys battery | grep level
  level: 92

```



