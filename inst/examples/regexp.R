re2p <- re2_regexp("hello world")
stopifnot(mode(re2p) == "externalptr")

## UTF-8 and matching interface
# By default, pattern and input text are interpreted as UTF-8.
# The Latin1 option causes them to be interpreted as Latin-1.
x <- "fa\xE7ile"
Encoding(x) <- "latin1"
re2_detect(x, re2_regexp("fa\xE7", encoding = "Latin1"))

## Case insensitive
re2_detect("fOobar ", re2_regexp("Foo", case_sensitive = FALSE))

## Literal string (as opposed to regular expression)
## Matches only when 'literal' option is TRUE
re2_detect("foo\\$bar", re2_regexp("foo\\$b", literal = TRUE))
re2_detect("foo\\$bar", re2_regexp("foo\\$b", literal = FALSE))

## Use of never_nl
re <- re2_regexp("(abc(.|\n)*def)", never_nl = FALSE)
re2_match("abc\ndef\n", re)
re <- re2_regexp("(abc(.|\n)*def)", never_nl = TRUE)
re2_match("abc\ndef\n", re)
