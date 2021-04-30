color <- c("yellowgreen", "steelblue", "GOLDENROD", "forestgreen")
re2_which(color, "o")
re2_subset(color, "o")

re2_which(c("x", "y", NA, "foo", ""), ".")
re2_subset(c("x", "y", NA, "foo", ""), ".")

# Use precompiled regexp
re <- re2_regexp("[a-z]")
re2_which(color, re)
re2_subset(color, re)

re <- re2_regexp("[a-z]", case_sensitive = FALSE)
re2_which(color, re)
re2_subset(color, re)

# Vector of patterns
re2_which(color, c("^o", "bl.e$", re, "$"))
