panagram <- c(
  "The quick brown fox jumps over the lazy dog",
  "How vexingly quick daft zebras jump!"
)

re2_split(panagram, " quick | over | zebras ")
re2_split(panagram, " quick | over | zebras ", simplify = TRUE)

# Use compiled regexp
re <- re2_regexp("quick | over |how ", case_sensitive = FALSE)
re2_split(panagram, re)
re2_split(panagram, re, simplify = TRUE)

# Restrict number of matches
re2_split(panagram, " quick | over | zebras ", n = 2)
