---
author: karim
comments: true
date: 2013-02-13 17:29:48+00:00
layout: post
slug: installing-musiccabinet-on-top-of-subsonic
title: Installing MusicCabinet on Top of SubSonic
wordpress_id: 6004
categories:
- Home Lab
- OS
following_users:
- jarret
- joechan
- kelatov
tags:
- /usr/share/subsonic
- /var/lib/pgsql/data/pg_hba.conf
- /var/subsonic
- getent
- lastfm
- MusicCabinet
- PostgreSQL
- postgresql-setup initdb
- psql
- rpm -ql
- scrobbling
- subsonic
- systemctl
- tar cpvjf
- unzip
- wget
- \password postgres
---

A while back I [posted](http://virtuallyhyper.com/2012/10/installing-subsonic-on-fedora-17/) about installing _subsonic_. While I was using the application, I  noticed that the search feature is very limited and I wanted to expand it. So I ran across [MusicCabinet](http://dilerium.se/musiccabinet/), from their webpage:



> 
Subsonic is a streaming music server, providing instant access to your personal music library. It is written by Sindre Mehus and released under the GPL.

MusicCabinet is an add-on for Subsonic. It uses the free musical knowledge gathered at Last.fm, to seamlessly:




It's an add-on for _subsonic_ and among other features it expands the searching functionality. 

I couldn't find any good instructions on how to install the add-on. The only link I found was [this](http://forum.subsonic.org/forum/viewtopic.php?f=8&t=10220) forum. The instructions were for an Ubuntu install and I was running Fedora. So I decided to write up instructions on how to install the plugin in Fedora on top of the _subsonic_ RPM. Checking out the RPM, here are the files the original _subsonic_ package contained:


```

[elatov@moxz ~]$ rpm -ql subsonic-4.7-3105.i386
/etc/init.d/subsonic
/etc/sysconfig/subsonic
/usr/share/subsonic/subsonic-booter-jar-with-dependencies.jar
/usr/share/subsonic/subsonic.sh
/usr/share/subsonic/subsonic.war
/var/subsonic/transcode/ffmpeg
/var/subsonic/transcode/lame

```


Not that much stuff. The database for subsonic is under **/var/subsonic**:


```

[elatov@moxz ~]$ ls /var/subsonic/
db         jetty    subsonic.log         subsonic_sh.log  transcode
db.backup  lucene2  subsonic.properties  thumbs

```


You have also need to java version 7 installed. I already had that installed:


```

[elatov@moxz ~]$ java -version
java version "1.7.0_09-icedtea"
OpenJDK Runtime Environment (fedora-2.3.4.fc17-i386)
OpenJDK Client VM (build 23.2-b09, mixed mode)

```


I covered the install of java in the initial post [here](http://virtuallyhyper.com/2012/10/installing-subsonic-on-fedora-17/), check it out if necessary. Now let's get started on the _MusicCabinet_ install. 



### 1. Install and Setup PostgreSQL


Here is how the install looks like with _YUM_:


```

[elatov@moxz ~]$ sudo yum install postgresql postgresql-server
Resolving Dependencies
--> Running transaction check
---> Package postgresql.i686 0:9.1.7-1.fc17 will be installed
...
...
Installed:
  postgresql.i686 0:9.1.7-1.fc17  postgresql-server-9.1.7-1.fc17.i686.rpm                                             

Dependency Installed:
  postgresql-libs.i686 0:9.1.7-1.fc17                                          

Complete!

```


That went well, now let's initialize the database:


```

[elatov@moxz ~]$ sudo postgresql-setup initdb
Initializing database ... OK

```


Next let's enable the **postgresql** service and start it:


```

[elatov@moxz ~]$ sudo systemctl enable postgresql.service
ln -s '/usr/lib/systemd/system/postgresql.service' '/etc/systemd/system/multi-user.target.wants/postgresql.service'
[elatov@moxz ~]$ sudo systemctl start postgresql.service

```


Confirming that the service is running:


```

[elatov@moxz ~]$ sudo systemctl status postgresql.service
postgresql.service - PostgreSQL database server
          Loaded: loaded (/usr/lib/systemd/system/postgresql.service; enabled)
          Active: active (running) since Wed, 30 Jan 2013 17:13:05 -0800; 49s ago
         Process: 32310 ExecStart=/usr/bin/pg_ctl start -D ${PGDATA} -s -o -p ${PGPORT} -w -t 300 (code=exited, status=0/SUCCESS)
         Process: 32305 ExecStartPre=/usr/bin/postgresql-check-db-dir ${PGDATA} (code=exited, status=0/SUCCESS)
        Main PID: 32315 (postgres)
          CGroup: name=systemd:/system/postgresql.service
                  ├ 32315 /usr/bin/postgres -D /var/lib/pgsql/data -p 5432
                  ├ 32316 postgres: logger process   
                  ├ 32318 postgres: writer process   
                  ├ 32319 postgres: wal writer process   
                  ├ 32320 postgres: autovacuum launcher process   
                  └ 32321 postgres: stats collector process   

```


That all looks good. It looks like the install also creates a **postgres** user:


```

[elatov@moxz ~]$ getent passwd postgres
postgres:x:26:26:PostgreSQL Server:/var/lib/pgsql:/bin/bash

```


Then using the **postgres** user, reset the password for itself:


```

[elatov@moxz ~]$ sudo -u postgres psql postgres 
psql (9.1.7)
Type "help" for help.

postgres=# \password postgres
Enter new password: 
Enter it again: 
postgres-# \q

```


Lastly, allow logins with local password authentication to the _psql_ instance. Edit the following file: **/var/lib/pgsql/data/pg_hba.conf** and change these lines:


```

# "local" is for Unix domain socket connections only
local   all             all                                     peer
# IPv4 local connections:
host    all             all             127.0.0.1/32            ident

```


to look like this:


```

# "local" is for Unix domain socket connections only
local   all             all                                     md5
# IPv4 local connections:
host    all             all             127.0.0.1/32            md5

```


After the change, restart the _postgresql_ service:


```

[elatov@moxz ~]$ sudo systemctl restart postgresql.service

```


Then you should be able to login with the password you set from any user:


```

[elatov@moxz ~]$ psql -U postgres
Password for user postgres: 
psql (9.1.7)
Type "help" for help.

postgres=# \q

```


Even when you try to connect to the localhost IP:


```

[elatov@moxz ~]$ psql -U postgres -h localhost
Password for user postgres: 
psql (9.1.7)
Type "help" for help.

postgres=# \q

```


That will be it for the **postgresql** setup.



### 2. Backup the Subsonic Database and Install Files


The files are stored under **/var/subsonic**, so let's tar that up. First let's stop the subsonic service:


```

[elatov@moxz ~]$ sudo systemctl stop subsonic.service

```


Now let's check the status:


```

[elatov@moxz ~]$ sudo systemctl status subsonic.service
subsonic.service - LSB: Subsonic daemon
          Loaded: loaded (/etc/rc.d/init.d/subsonic)
          Active: inactive (dead) since Wed, 30 Jan 2013 17:28:15 -0800; 5s ago
         Process: 32425 ExecStop=/etc/rc.d/init.d/subsonic stop (code=exited, status=0/SUCCESS)
          CGroup: name=systemd:/system/subsonic.service

Jan 23 21:06:47 moxz.dnsd.me subsonic[707]: Starting subsonic ...
Jan 23 21:06:48 moxz.dnsd.me subsonic[707]: Started Subsonic [PID 795, /var...]
Jan 30 17:28:15 moxz.dnsd.me subsonic[32425]: Stopping subsonic ...[  OK  ]

```


That looks good. Now let's back up the data:


```

[elatov@moxz ~]$ sudo tar cpvjf subsonic_backup.tar.bz2 /var/subsonic/
/var/subsonic/thumbs/200/315cff71c50a61d933b3d30c392b0343.jpeg
...
...

```


Since the application files go under **/usr/share/subsonic**, let's backup those up as well:


```

[elatov@moxz ~]$ sudo tar cpvjf subsonic_usr_backup.tar.bz2 /usr/share/subsonic
/usr/share/subsonic/
/usr/share/subsonic/subsonic.sh
/usr/share/subsonic/subsonic-booter-jar-with-dependencies.jar
/usr/share/subsonic/subsonic.war

```


Here are the two resulting files:


```

[elatov@moxz ~]$ ls -lh subsonic*
-rw-r--r-- 1 root root 61M Jan 30 17:33 subsonic_backup.tar.bz2
-rw-r--r-- 1 root root 29M Jan 30 17:35 subsonic_usr_backup.tar.bz2

```


That looks good.



### 3. Download and Extract MusicCabinet


This is pretty easy, let's create a temporary directory and download the install file there:


```

[elatov@moxz ~]$ mkdir files
[elatov@moxz ~]$ cd files/
[elatov@moxz files]$ wget http://dilerium.se/musiccabinet/subsonic-installer-standalone.zip
--2013-01-30 17:39:24--  http://dilerium.se/musiccabinet/subsonic-installer-standalone.zip
Resolving dilerium.se (dilerium.se)... 193.202.110.54
Connecting to dilerium.se (dilerium.se)|193.202.110.54|:80... connected.
HTTP request sent, awaiting response... 200 OK
Length: 29481485 (28M) [application/zip]
Saving to: `subsonic-installer-standalone.zip'

100%[=====================================>] 29,481,485   482K/s   in 76s     

2013-01-30 17:40:41 (380 KB/s) - `subsonic-installer-standalone.zip' saved [29481485/29481485]

```


Now let's extract the contents:


```

[elatov@moxz files]$ unzip subsonic-installer-standalone.zip 
Archive:  subsonic-installer-standalone.zip
   creating: subsonic-installer-standalone/
  inflating: subsonic-installer-standalone/subsonic-main.war  
  inflating: subsonic-installer-standalone/subsonic-booter.jar  
  inflating: subsonic-installer-standalone/Getting Started.html  
  inflating: subsonic-installer-standalone/LICENSE.TXT  
  inflating: subsonic-installer-standalone/README.TXT  
  inflating: subsonic-installer-standalone/subsonic.bat  
  inflating: subsonic-installer-standalone/subsonic.sh  

```


Now to the final steps:



### 4. Copy the Installation Files to the SubSonic Install


We just need to copy over two files. Here are the permissions before copying the files over:


```

[elatov@moxz files]$ ls -l /usr/share/subsonic/
total 30724
-rw-r--r-- 1 root root 10801885 Sep 11 13:06 subsonic-booter-jar-with-dependencies.jar
-rwxr-xr-x 1 root root     5030 Sep 11 13:06 subsonic.sh
-rw-r--r-- 1 root root 20603945 Sep 11 13:06 subsonic.war

```


Now to copy over the file:


```

[elatov@moxz files]$ cd subsonic-installer-standalone
[elatov@moxz subsonic-installer-standalone]$ sudo cp subsonic-main.war /usr/share/subsonic/subsonic.war
[elatov@moxz subsonic-installer-standalone]$ sudo cp subsonic-booter.jar /usr/share/subsonic/subsonic-booter-jar-with-dependencies.jar

```


Here is how the files look after the copy:


```

[elatov@moxz files]$ ls -l /usr/share/subsonic/
total 30016
-rw-r--r-- 1 root root  7550525 Jan 30 17:52 subsonic-booter-jar-with-dependencies.jar
-rwxr-xr-x 1 root root     5030 Sep 11 13:06 subsonic.sh
-rw-r--r-- 1 root root 23130290 Jan 30 17:48 subsonic.war

```


The permissions looks good. Now let's start the subsonic service:


```

[elatov@moxz ~]$ sudo systemctl start subsonic.service

```


Now let's make sure it stated up fine:


```

[elatov@moxz ~]$ sudo systemctl status subsonic.service
subsonic.service - LSB: Subsonic daemon
          Loaded: loaded (/etc/rc.d/init.d/subsonic)
          Active: active (exited) since Wed, 30 Jan 2013 17:53:46 -0800; 5s ago
         Process: 32425 ExecStop=/etc/rc.d/init.d/subsonic stop (code=exited, status=0/SUCCESS)
         Process: 32606 ExecStart=/etc/rc.d/init.d/subsonic start (code=exited, status=0/SUCCESS)
          CGroup: name=systemd:/system/subsonic.service

Jan 30 17:53:45 moxz.dnsd.me subsonic[32606]: Starting subsonic ...
Jan 30 17:53:46 moxz.dnsd.me subsonic[32606]: Started Subsonic [PID 32632, ...]

```


That looks good.



### 5. Configure MusicCabinet via the Browser


Visit the _subsonic_ web page and the interface will look different:

[![music-cabinet-subsonic](http://virtuallyhyper.com/wp-content/uploads/2013/01/music-cabinet-subsonic.png)](http://virtuallyhyper.com/wp-content/uploads/2013/01/music-cabinet-subsonic.png)

Then go to "Settings" -> "MusicCabinet" and specify the postgres user password:

[![subsonic-music-cabinet-settings](http://virtuallyhyper.com/wp-content/uploads/2013/01/subsonic-music-cabinet-settings.png)](http://virtuallyhyper.com/wp-content/uploads/2013/01/subsonic-music-cabinet-settings.png)

After you type in the _postgres_ password it will ask you to "Upgrade Database":

[![music_cabin_upgrade_db](http://virtuallyhyper.com/wp-content/uploads/2013/01/music_cabin_upgrade_db.png)](http://virtuallyhyper.com/wp-content/uploads/2013/01/music_cabin_upgrade_db.png)

After it's done updating the database, it will ask you to update your search index:

[![music_cabinet_update_index](http://virtuallyhyper.com/wp-content/uploads/2013/01/music_cabinet_update_index.png)](http://virtuallyhyper.com/wp-content/uploads/2013/01/music_cabinet_update_index.png)

While the scan is going you can check the progress of the scan:

[![music-cabinet-scan-progress](http://virtuallyhyper.com/wp-content/uploads/2013/01/music-cabinet-scan-progress.png)](http://virtuallyhyper.com/wp-content/uploads/2013/01/music-cabinet-scan-progress.png)

While it's indexing, you can also enable _lastfm_ 'scrobbling' by going to "Settings" -> "Personal", if you scroll down a little bit you will an option to "Configure lastfm scrobbling", like so:

[![music-cabinet-lastfm-scrobbling](http://virtuallyhyper.com/wp-content/uploads/2013/01/music-cabinet-lastfm-scrobbling.png)](http://virtuallyhyper.com/wp-content/uploads/2013/01/music-cabinet-lastfm-scrobbling.png)

After the indexing is done, the settings page for _MusicCabinet_ will look like this:

[![music-cabinet-index-done](http://virtuallyhyper.com/wp-content/uploads/2013/01/music-cabinet-index-done.png)](http://virtuallyhyper.com/wp-content/uploads/2013/01/music-cabinet-index-done.png)

Now if you search for something there will be an option to do an "Advanced Search", here is how it looks like:

[![advanced_search_music_cabinet](http://virtuallyhyper.com/wp-content/uploads/2013/01/advanced_search_music_cabinet.png)](http://virtuallyhyper.com/wp-content/uploads/2013/01/advanced_search_music_cabinet.png)

You now can also view your library by Tags, I still prefer the File-based browsing but this just adds more functionality. Also, every time you play a song it updates your _lastfm_ library as well. I am sure there are a lot more updates, but those are the ones that stood out to me.
