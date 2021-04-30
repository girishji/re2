### 'R CMD check' does not Reliably compare the output of examples with 
### Rout.save file. 

library(re2)

############################################################
### count

color <- c("yellowgreen", "steelblue", "goldenrod", "forestgreen")
stopifnot(re2_count(color, "e") == c(3, 3, 1, 3))
stopifnot(re2_count(color, "r") == c(1, 0, 1, 2))

# Regular expression vs literal string)
stopifnot(re2_count(c("..", "a...", "foo.b"), ".") == c(2, 4, 5))
stopifnot(re2_count(c("..", "a...", "foo.b"), re2_regexp(".", literal = TRUE)) 
          == c(2, 3, 1))

############################################################
### detect

## Character vector input
s <- c("barbazbla", "foobar", "not present here ")
pat <- "(foo)|(bar)baz"
stopifnot(re2_detect(s, pat) == c(TRUE, TRUE, FALSE))

## Use precompiled regexp
re <- re2_regexp("(foo)|(bAR)baz", case_sensitive = FALSE)
stopifnot(re2_detect(s, re) == c(TRUE, TRUE, FALSE))

############################################################
###  extract_replace

# Returns extracted string with substitutions
stopifnot(re2_extract_replace(
  "bunny@wunnies.pl",
  "(.*)@([^.]*)",
  "\\2!\\1"
) == c("wunnies!bunny"))

# Case insensitive
stopifnot(re2_extract_replace(
  "BUNNY@wunnies.pl",
  re2_regexp("(b.*)@([^.]*)", case_sensitive = FALSE),
  "\\2!\\1"
) == c("wunnies!BUNNY"))

# Max submatch too large (1 match group, 2 submatches needed)
stopifnot(re2_extract_replace("foo", "f(o+)", "\\1\\2") == c(""))

############################################################
### locate

r1 <- c(
  12, 11,
  10,  9,
  10,  9,
  12, 11
)
r2 <- c(
  3, 3,
  5, 5,
  3, 3,
  NA, NA
)
r3 <- c(
  2,  2,
  3,  3,
  5,  5,
  4,  4
)
r4 <- c(
  3,  4,
  1,  2,
  3,  4,
  5,  6
)
color <- c("yellowgreen", "steelblue", "goldenrod", "forestgreen")
stopifnot(all(c(t(re2_locate(color, "$"))) == r1))
stopifnot(all(na.omit(c(t(re2_locate(color, "l")))) == na.omit(r2)))
stopifnot(all(c(t(re2_locate(color, "e"))) == r3))

# String length can be a multiple of pattern length
stopifnot(all(c(t(re2_locate(color, c("l(l|d)?", "st")))) == r4))

# Locate all occurrences
r <- re2_locate_all(color, "l")
stopifnot(all(c(t(r[[1]])) == c(3, 3, 4, 4)))
stopifnot(all(c(t(r[[2]])) == c(5, 5, 7, 7)))
stopifnot(all(c(t(r[[3]])) == c(3, 3)))

r <- re2_locate_all(color, "e")
stopifnot(all(c(t(r[[1]])) == c(2, 2, 9, 9, 10, 10)))
stopifnot(all(c(t(r[[2]])) == c(3, 3, 4, 4, 9, 9)))
stopifnot(all(c(t(r[[3]])) == c(5, 5)))
stopifnot(all(c(t(r[[4]])) == c(4, 4, 9, 9, 10, 10)))

r <- re2_locate_all(color, ".")
stopifnot(all(c(t(r[[1]])) 
              == c(1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7,
                   7, 8, 8, 9, 9, 10, 10, 11, 11)))

############################################################
###  which

color <- c("yellowgreen", "steelblue", "GOLDENROD", "forestgreen")
stopifnot(all(re2_which(color, "o") == c(1, 4)))
stopifnot(all(re2_subset(color, "o") == c("yellowgreen", "forestgreen")))

stopifnot(all(re2_which(c("x", "y", NA, "foo", ""), ".") == c(1, 2, 4)))
stopifnot(all(re2_subset(c("x", "y", NA, "foo", ""), ".") 
              == c("x", "y", "foo")))

# Use precompiled regexp
re <- re2_regexp("[a-z]")
stopifnot(all(re2_which(color, re) == c(1, 2, 4)))
stopifnot(all(re2_subset(color, re) 
              == c("yellowgreen", "steelblue", "forestgreen")))

re <- re2_regexp("[a-z]", case_sensitive = FALSE)
stopifnot(all(re2_which(color, re) == c(1, 2, 3, 4)))
stopifnot(all(re2_subset(color, re) 
              == c("yellowgreen", "steelblue", "GOLDENROD", "forestgreen")))
# Vector of patterns
stopifnot(all(re2_which(color, c("^o", "bl.e$", re, "$")) == c(2, 3, 4)))

############################################################
### replace

string <- c("yabba dabba doo", "famabbb sb")
stopifnot(all(re2_replace(string, "b+", "d") 
              == c("yada dabba doo", "famad sb")))
stopifnot(all(re2_replace_all(string, "b+", "d") 
              == c("yada dada doo", "famad sd")))
# Rearrange matching groups in replaced string
stopifnot(all(re2_replace(
  "boris@kremvax.ru",
  "(.*)@([^.]*)", "\\2!\\1"
) == c("kremvax!boris.ru")))

# Use complied pattern
string <- "the quick brown fox jumps over the lazy dogs."
re <- re2_regexp("(qu|[b-df-hj-np-tv-z]*)([a-z]+)")
rewrite <- "\\2\\1ay"
stopifnot(all(re2_replace(string, re, rewrite) 
              == c("ethay quick brown fox jumps over the lazy dogs.")))
stopifnot(all(re2_replace_all(string, re, rewrite) 
              == c("ethay ickquay ownbray oxfay umpsjay overay ethay azylay ogsday.")))

string <- "abcd.efghi@google.com"
re <- re2_regexp("\\w+")
rewrite <- "\\0-NOSPAM"
stopifnot(all(re2_replace(string, re, rewrite) 
              == c("abcd-NOSPAM.efghi@google.com")))
stopifnot(all(re2_replace_all(string, re, rewrite) 
              == c("abcd-NOSPAM.efghi-NOSPAM@google-NOSPAM.com-NOSPAM")))

string <- "aba\naba"
re <- re2_regexp("a.*a")
rewrite <- "(\\0)"
stopifnot(all(re2_replace(string, re, rewrite) == c("(aba)\naba")))
stopifnot(all(re2_replace_all(string, re, rewrite) == c("(aba)\n(aba)")))

# Vectorize string and pattern
string <- c("ababababab", "bbbbbb", "bbbbbb", "aaaaa")
pattern <- c("b", "b+", "b*", "b*")
rewrite <- "bb"
stopifnot(all(re2_replace(string, pattern, rewrite) 
              == c("abbabababab", "bb", "bb", "bbaaaaa")))
stopifnot(all(re2_replace_all(string, pattern, rewrite) 
              == c("abbabbabbabbabb", "bb", "bb", "bbabbabbabbabbabb")))

############################################################
### regexp


re2p <- re2_regexp("hello world")
stopifnot(mode(re2p) == "externalptr")

## UTF-8 and matching interface
# By default, pattern and input text are interpreted as UTF-8.
# The Latin1 option causes them to be interpreted as Latin-1.
x <- "fa\xE7ile"
Encoding(x) <- "latin1"
stopifnot(re2_detect(x, re2_regexp("fa\xE7", encoding = "Latin1")) == TRUE)

## Case insensitive
stopifnot(re2_detect("fOobar ", re2_regexp("Foo", case_sensitive = FALSE)) 
          == TRUE)

## Literal string (as opposed to regular expression)
## Matches only when 'literal' option is TRUE
stopifnot(re2_detect("foo\\$bar", re2_regexp("foo\\$b", literal = TRUE)) 
          == TRUE)
stopifnot(re2_detect("foo\\$bar", re2_regexp("foo\\$b", literal = FALSE)) 
          == FALSE)

## Use of never_nl
re <- re2_regexp("(abc(.|\n)*def)", never_nl = FALSE)
stopifnot(re2_match("abc\ndef\n", re)[1, 1:3] == c("abc\ndef", "abc\ndef", "\n"))
re <- re2_regexp("(abc(.|\n)*def)", never_nl = TRUE)
stopifnot(all(is.na(re2_match("abc\ndef\n", re)[1, 1:3])))


############################################################
### split

panagram <- c(
  "The quick brown fox jumps over the lazy dog",
  "How vexingly quick daft zebras jump!"
)

r <- re2_split(panagram, " quick | over | zebras ")
list1 <- list(
  c("The", "brown fox jumps", "the lazy dog"),
  c("How vexingly", "daft", "jump!")
)
stopifnot(identical(r, list1))

r <- re2_split(panagram, " quick | over | zebras ", simplify = TRUE)
m1 <- rbind(
  c("The", "brown fox jumps", "the lazy dog"),
  c("How vexingly", "daft", "jump!")
)
stopifnot(r == m1)

# Use compiled regexp
re <- re2_regexp("quick | over |how ", case_sensitive = FALSE)
r <- re2_split(panagram, re)
list2 <- list(
  c("The ", "brown fox jumps", "the lazy dog"),
  c("", "vexingly ", "daft zebras jump!")
)
stopifnot(identical(r, list2))

r <- re2_split(panagram, re, simplify = TRUE)
m2 <- rbind(
  c("The ", "brown fox jumps", "the lazy dog"),
  c("", "vexingly ", "daft zebras jump!")
)
stopifnot(r == m2)

# Restrict number of matches
r <- re2_split(panagram, " quick | over | zebras ", n = 2)
list3 <- list(
  c("The", "brown fox jumps over the lazy dog"),
  c("How vexingly", "daft zebras jump!")
)
stopifnot(identical(r, list3))


############################################################
### match


## Substring extraction
strings <- c("barbazbla", "foobar")
pattern <- "(foo)|(?P<TestGroup>bar)baz"

result <- re2_match(strings, pattern)
m <- rbind(
  c("barbaz", NA, "bar"),
  c("foo", "foo", NA)
)

lvec <- function(lst) {
  na.omit(unlist(lst))
}
mvec <- function(mat) {
  na.omit(c(mat))
}
stopifnot(mvec(result) == mvec(m))
stopifnot(is.matrix(result))
stopifnot(names(result) == c(".0", ".1", "TestGroup"))

result <- re2_match(strings, pattern, simplify = FALSE)
lst <- list(
  rbind(
    c("barbaz", NA, "bar")
  ),
  rbind(
    c("foo", "foo", NA)
  )
)
stopifnot(lvec(result) == lvec(lst))
stopifnot(is.list(result))
stopifnot(is.character(result[[1]]))

## Compile regexp
re <- re2_regexp("(foo)|(BaR)baz", case_sensitive = FALSE)
r <- re2_match(strings, re)
m <- rbind(
  c("barbaz", NA, "bar"),
  c("foo", "foo", NA)
)
stopifnot(mvec(r) == mvec(m))

strings <- c(
  "Home: 743 733 5365", "373-733-5753 ", "foobar",
  "733.335.3457 and Work: 573-433-7577 "
)
re <- re2_regexp("([0-9]{3})[- .]([0-9]{3})[- .]([0-9]{4})")
r <- re2_match(strings, re)
m <- rbind(
  c("743 733 5365", "743", "733", "5365"),
  c("373-733-5753", "373", "733", "5753"),
  c(NA, NA, NA, NA),
  c("733.335.3457", "733", "335", "3457")
)
stopifnot(mvec(r) == mvec(m))

## Vectorized over patterns
r <- re2_match(strings, c(re, "53 $", "^foo", re))
m <- rbind(
  c("743 733 5365", "743", "733", "5365"),
  c("53 ", NA, NA, NA),
  c("foo", NA, NA, NA),
  c("733.335.3457", "733", "335", "3457")
)
stopifnot(mvec(r) == mvec(m))

## Match all occurances, not just the first
r <- re2_match_all(strings, re)
lst <- list(
  rbind(
    c("743 733 5365", "743", "733", "5365")
  ),
  rbind(
    c("373-733-5753", "373", "733", "5753")
  ),
  rbind(
    c("733.335.3457", "733", "335", "3457"),
    c("573-433-7577", "573", "433", "7577")
  )
)
stopifnot(unlist(r) == unlist(lst))

r <- re2_match_all("ruby:1234 68 red:92 blue:", "(\\w+):(\\d+)")
m <- rbind(
  c("ruby:1234", "ruby", "1234"),
  c("red:92", "red", "92")
)
stopifnot(r[[1]] == m)

## Vectorized over patterns (matching all occurances)

r <- re2_match_all(strings, c(re, "53 $", "^foo", re))
lst <- list(
  rbind(
    c("743 733 5365", "743", "733", "5365")
  ),
  rbind(
    c("53 ")
  ),
  rbind(
    c("foo")
  ),
  rbind(
    c("733.335.3457", "733", "335", "3457"),
    c("573-433-7577", "573", "433", "7577")
  )
)
stopifnot(unlist(r) == unlist(lst))
