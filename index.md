---
layout: page
title:  Recent Posts
---
{% include JB/setup %}

<ul class="posts">
{% for post in site.posts  limit:10 %}

  {% assign author_info = site.data.authors[post.author] %}
  <a href="{{ BASE_PATH }}{{ post.url }}"><h3> {{ post.title }}</h3></a>
	<i>Posted by <a href="{{ author_info.web }}">{{ post.author}}</a> on {{ post.date | date_to_string }}</i> <div style="float: right;"><a href="{{ BASE_PATH }}{{ post.url }}#disqus_thread"></a></div><br />
        {{ post.content | strip_html | truncatewords:75}}
            <a href="{{ post.url }}">Read more</a>
    {% endfor %}
</ul>
