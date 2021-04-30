# Returns extracted string with substitutions
re2_extract_replace(
  "bunny@wunnies.pl",
  "(.*)@([^.]*)",
  "\\2!\\1"
)

# Case insensitive
re2_extract_replace(
  "BUNNY@wunnies.pl",
  re2_regexp("(b.*)@([^.]*)", case_sensitive = FALSE),
  "\\2!\\1"
)

# Max submatch too large (1 match group, 2 submatches needed).
#   Replacement fails and empty string is returned.
re2_extract_replace("foo", "f(o+)", "\\1\\2")

