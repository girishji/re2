## Character vector input
s <- c("barbazbla", "foobar", "not present here ")
pat <- "(foo)|(bar)baz"
re2_detect(s, pat)

## Use precompiled regexp
re <- re2_regexp("(foo)|(bAR)baz", case_sensitive = FALSE)
re2_detect(s, re)
