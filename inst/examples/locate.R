color <- c("yellowgreen", "steelblue", "goldenrod", "forestgreen")

re2_locate(color, "$")
re2_locate(color, "l")
re2_locate(color, "e")

# String length can be a multiple of pattern length
re2_locate(color, c("l(l|d)?", "st"))

# Locate all occurrences
re2_locate_all(color, "l")
re2_locate_all(color, "e")

# Locate all characters
re2_locate_all(color, ".")
