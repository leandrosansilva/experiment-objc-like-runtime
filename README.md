# experiment-objc-like-runtime
Pet project - objective-c like runtime in pure C

TL;DR -> experiments with message passing based object oriented "DSL" in C.

This is not really a "project", but some experiments I started in a sunny weekend [1] when I had another anxiety crisis and could not leave home (but you really did not need to know that). So why not, instead of just watch series on Netflix, do some useless programming?

I hate when people say "I am a $programming_language_X programmer". I think this is rather too limiting. Good programmers are just programmers; the programming language is just a tool.

But as I am not a good programmer, I would consider myself a C++ programmer, as it's my "mother tongue" that I have used professionally for most of my career.

Awkwardly in my current job I work essentially with Objective-C, something I had never used before (specially because I had never really touched a Mac before the job interview), which has a implementation of OO quite different from C++, even though both are "C like" languages.

Every obj-c programmer knows this language is basically C with some syntax sugar to enable the message passing approach.

It's a very interesting concept, even though condemned to be slow, as most optimizations and error checking that could be done in compile time cannot be done as (almost) everything is done in runtime.

So I thought "why not try to implement it on my own, just for fun?", so I maybe could learn something and understand why some things in obj-c behave like they do.

No, I did not read the objc specification, so this code is uniquely based on my ideas at the moment I was writing it. Something like "how can solve this problem?", which turns out to create another problem and so on.

Mental note: this is not a propper README.

[1] It's not a sunny weekend in a tropical Brazilian summer that no one cares about, where I come from, but in a cold Bavarian winter.
