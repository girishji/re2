
<!-- README.md is generated from README.Rmd. Please edit that file -->
<!-- Keep this file sync'ed with vignette -->

# re2

## Overview

re2 provides regular-expression functions based on Google’s RE2 (C++)
regular-expression library. re2’s functionality mirrors *grep*, *regex*,
and *sub* family of functions in base R, as well as functions in
[stringr](https://github.com/tidyverse/stringr) package.

Why re2?

Regular expression matching can be done in two ways: using recursive
backtracking or using finite automata-based techniques.

Perl, PCRE, Python, Ruby, Java, and many other languages rely on
recursive backtracking for their regular expression implementations. The
problem with this approach is that performance can degrade very quickly.
Time complexity can be exponential. In contrast, re2 uses finite
automata-based techniques for regular expression matching, guaranteeing
linear time execution and a fixed stack footprint. See links to Russ
Cox’s excellent articles below.

## Installation

``` r
# Install the development version from GitHub:
# install.packages("devtools")
devtools::install_github("girishji/re2")
```

## Usage

re2 provides three types of regular-expression functions: - Find the
presence of a pattern in string - Extract matched substrings - Replace
(substitute) matched substrings

Regular expression patterns can be pre-compiled and reused. All
functions take a vector of strings as argument. Regular-expression
pattern is also vectorized.

1.  `re2_detect(x, pattern)` finds if a pattern is present in string

``` r
re2_detect(c("barbazbla", "foobar", "foxy brown"), "(foo)|(bar)baz")
#> [1]  TRUE  TRUE FALSE
```

1.  `re2_count(x, pattern)` counts the number of matches in string

``` r
re2_count(c("yellowgreen", "steelblue", "maroon"), "e")
#> [1] 3 3 0
```

1.  `re2_subset(x, pattern)` selects strings that match

``` r
re2_subset(c("yellowgreen", "steelblue", "goldenrod"), "ee")
#> [1] "yellowgreen" "steelblue"
```

1.  `re2_match(x, pattern, simplify = FALSE)` extracts first matched
    substring

``` r
re2_match("ruby:1234 68 red:92 blue:", "(\\w+):(\\d+)")
#>      .0          .1     .2    
#> [1,] "ruby:1234" "ruby" "1234"
```

Groups can be named:

``` r
re2_match(c("barbazbla", "foobar"), "(foo)|(?P<TestGroup>bar)baz")
#>      .0       .1    TestGroup
#> [1,] "barbaz" NA    "bar"    
#> [2,] "foo"    "foo" NA
```

Using pre-compiled regular-expression:

``` r
re <- re2_regexp("(foo)|(bar)baz", case_sensitive = FALSE)
re2_match(c("BaRbazbla", "Foobar"), re)
#>      .0       .1    .2   
#> [1,] "BaRbaz" NA    "BaR"
#> [2,] "Foo"    "Foo" NA
```

1.  `re2_match_all(x, pattern)` extracts all matched substrings

``` r
re2_match_all("ruby:1234 68 red:92 blue:", "(\\w+):(\\d+)")
#> [[1]]
#>      .0          .1     .2    
#> [1,] "ruby:1234" "ruby" "1234"
#> [2,] "red:92"    "red"  "92"
```

1.  `re2_replace(x, pattern, rewrite)` replaces first matched pattern in
    string

``` r
re2_replace("yabba dabba doo", "b+", "d")
#> [1] "yada dabba doo"
```

Rewritten using matched groups:

``` r
re2_replace("bunny@wunnies.pl", "(.*)@([^.]*)", "\\2!\\1")
#> [1] "wunnies!bunny.pl"
```

1.  `re2_replace_all(x, pattern, rewrite)` replaces all matched patterns
    in string

``` r
re2_replace_all("yabba dabba doo", "b+", "d")
#> [1] "yada dada doo"
```

1.  `re2_extract_replace(x, pattern, rewrite)` extracts and substitutes
    (ignores non-matching portions of x, unlike `re2_replace()`)

``` r
re2_extract_replace("bunny@wunnies.pl", "(.*)@([^.]*)", "\\2!\\1")
#> [1] "wunnies!bunny"
```

1.  `re2_split(x, pattern, simplify = FALSE, n = Inf)` splits string
    based on pattern

``` r
re2_split("How vexingly quick daft zebras jump!", " quick | zebras")
#> [[1]]
#> [1] "How vexingly" "daft"         " jump!"
```

1.  `re2_locate(x, pattern)` seeks the start and end of pattern in a
    string

``` r
re2_locate(c("yellowgreen", "steelblue"), "l")
#>      begin end
#> [1,]     3   3
#> [2,]     5   5
```

1.  `re2_locate_all(x, pattern)` locates start and end of all
    occurrences of pattern in a string

``` r
re2_locate_all(c("yellowgreen", "steelblue"), "l")
#> [[1]]
#>      begin end
#> [1,]     3   3
#> [2,]     4   4
#> 
#> [[2]]
#>      begin end
#> [1,]     5   5
#> [2,]     7   7
```

In all the above functions, regular-expression pattern can be
pre-compiled with `re2_regexp(pattern, ...)`. Here are some of the
options:

-   `case_sensitive`: Match is case-sensitive
-   `encoding`: UTF8 or Latin1
-   `literal`: Interpret pattern as literal, not regexp
-   `longest_match`: Search for longest match, not first match
-   `posix_syntax`: Restrict regexps to POSIX egrep syntax

`re2_get_options(regexp_ptr)` returns a list of options stored in the
pre-compiled regular-expression object.

## Regexp Syntax

re2 supports pearl style regular expressions (with extensions like \\d,
\\w, \\s, …) and provides most of the functionality of PCRE – eschewing
only backreferences and look-around assertions.

See [RE2 Syntax](https://github.com/google/re2/wiki/Syntax) for the
syntax supported by RE2, and a comparison with PCRE and PERL regexps.

For those not familiar with Perl’s regular expressions, here are some
examples of the most commonly used extensions:

|                        |                                                |
|------------------------|------------------------------------------------|
| `"hello (\\w+) world"` | `\w` matches a “word” character                |
| `"version (\\d+)"`     | `\d` matches a digit                           |
| `"hello\\s+world"`     | `\s` matches any whitespace character          |
| `"\\b(\\w+)\\b"`       | `\b` matches non-empty string at word boundary |
| `"(?i)hello"`          | `(?i)` turns on case-insensitive matching      |
| `"/\\*(.*?)\\*/"`      | `.*?` matches . minimum no. of times possible  |

The double backslashes are needed when writing R string literals.
However, they should not be used when writing raw string literals:

|                          |                                                 |
|--------------------------|-------------------------------------------------|
| `r"(hello (\w+) world)"` | `\w` matches a “word” character                 |
| `r"(version (\d+))"`     | `\d` matches a digit                            |
| `r"(hello\s+world)"`     | `\s` matches any whitespace character           |
| `r"(\b(\w+)\b)"`         | `\b` matches non-empty string at word boundary  |
| `r"((?i)hello)"`         | `(?i)` turns on case-insensitive matching       |
| `r"(/\*(.*?)\*/)"`       | `.*?` matches `.` minimum no. of times possible |

## References

-   [Regular Expression Matching Can Be Simple And
    Fast](https://swtch.com/~rsc/regexp/regexp1.html)
-   [Regular Expression Matching: the Virtual Machine
    Approach](https://swtch.com/~rsc/regexp/regexp2.html)
-   [Regular Expression Matching in the
    Wild](https://swtch.com/~rsc/regexp/regexp3.html)
-   [RE2 Syntax](https://github.com/google/re2/wiki/Syntax)
-   [RE2 C++ source](https://github.com/google/re2)
