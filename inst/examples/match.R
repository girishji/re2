## Substring extraction
strings <- c("barbazbla", "foobar")
pattern <- "(foo)|(?P<TestGroup>bar)baz"

re2_match(strings, pattern)
result <- re2_match(strings, pattern)
is.matrix(result)

re2_match(strings, pattern, simplify = FALSE)
result <- re2_match(strings, pattern, simplify = FALSE)
is.list(result)

## Compile regexp
re <- re2_regexp("(foo)|(BaR)baz", case_sensitive = FALSE)
re2_match(strings, re)

strings <- c(
  "Home: 743 733 5365", "373-733-5753 ", "foobar",
  "733.335.3457 and Work: 573-433-7577 "
)
re <- re2_regexp("([0-9]{3})[- .]([0-9]{3})[- .]([0-9]{4})")
re2_match(strings, re)

## Vectorized over patterns
re2_match(strings, c(re, "53 $", "^foo", re))

## Match all occurances, not just the first
re2_match_all(strings, re)
re2_match_all("ruby:1234 68 red:92 blue:", "(\\w+):(\\d+)")

## Vectorized over patterns (matching all occurances)
re2_match_all(strings, c(re, "53 $", "^foo", re))
