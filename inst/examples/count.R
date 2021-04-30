color <- c("yellowgreen", "steelblue", "goldenrod", "forestgreen")
re2_count(color, "e")
re2_count(color, "r")

# Regular expression vs literal string
re2_count(c("..", "a...", "foo.b"), ".")
re2_count(c("..", "a...", "foo.b"), re2_regexp(".", literal = TRUE))
