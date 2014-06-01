---
layout: page
title:  Recents Posts
---
{% include JB/setup %}

<ul class="posts">
{% for post in site.posts  limit:10 %}

  <a href="{{ BASE_PATH }}{{ post.url }}"><h3> {{ post.title }}<br /></h3></a>
	<i>Posted on {{ post.date | date_to_string }} by {{ post.author}}<br /></i> 
        {{ post.content | strip_html | truncatewords:75}}
            <a href="{{ post.url }}">Read more...</a>
    {% endfor %}
</ul>
