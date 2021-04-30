string <- c("yabba dabba doo", "famabbb sb")
re2_replace(string, "b+", "d")
re2_replace_all(string, "b+", "d")

# Rearrange matching groups in replaced string
re2_replace(
  "boris@kremvax.ru",
  "(.*)@([^.]*)", "\\2!\\1"
)

# Use complied pattern
string <- "the quick brown fox jumps over the lazy dogs."
re <- re2_regexp("(qu|[b-df-hj-np-tv-z]*)([a-z]+)")
rewrite <- "\\2\\1ay"
re2_replace(string, re, rewrite)
re2_replace_all(string, re, rewrite)

string <- "abcd.efghi@google.com"
re <- re2_regexp("\\w+")
rewrite <- "\\0-NOSPAM"
re2_replace(string, re, rewrite)
re2_replace_all(string, re, rewrite)

string <- "aba\naba"
re <- re2_regexp("a.*a")
rewrite <- "(\\0)"
re2_replace(string, re, rewrite)
re2_replace_all(string, re, rewrite)

# Vectorize string and pattern
string <- c("ababababab", "bbbbbb", "bbbbbb", "aaaaa")
pattern <- c("b", "b+", "b*", "b*")
rewrite <- "bb"
re2_replace(string, pattern, rewrite)
re2_replace_all(string, pattern, rewrite)
