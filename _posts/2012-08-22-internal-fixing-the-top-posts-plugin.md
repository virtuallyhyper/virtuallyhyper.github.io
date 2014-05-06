---
author: jarret
comments: true
date: 2012-08-22 21:38:15+00:00
layout: post
published: false
slug: internal-fixing-the-top-posts-plugin
title: '[Internal] fixing the top posts plugin'
wordpress_id: 2516
categories:
- VMware
following_users:
- jarret
---

The top posts plugin was linking back to virtualaholics. I deited the plugin and changed the origional line to this 


```

          <li><a href="<?php echo get_permalink( $top_post['post_id'] ); ?>"><?php echo $top_post['post_title']; ?></a></li> 

```

