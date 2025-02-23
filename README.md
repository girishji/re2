
<!-- README.md is generated from README.Rmd. Please edit that file -->
<!-- Keep this file sync'ed with vignette -->

# re2: R interface to Google RE2

## Overview

re2 package provides pattern matching, extraction, replacement and other
string processing operations using Google’s
[RE2](https://github.com/google/re2) (C++) regular-expression library.
The interface is consistent, and similar to
[stringr](https://github.com/tidyverse/stringr).

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
# Install the released version from CRAN:
install.packages("re2")

# Install the development version from GitHub:
# install.packages("devtools")
devtools::install_github("girishji/re2")
```

## Usage

re2 provides three types of regular-expression functions:

-   Find the presence of a pattern in string
-   Extract substrings that match a pattern
-   Replace matched groups

All functions take a vector of strings as argument. Regular-expression
patterns can be compiled, and reused for performance.

Here are the primary verbs of re2:

-   `re2_detect(x, pattern)` finds if a pattern is present in string

``` r
re2_detect(c("barbazbla", "foobar", "foxy brown"), "(foo)|(bar)baz")
#> [1]  TRUE  TRUE FALSE
```

-   `re2_count(x, pattern)` counts the number of matches in string

``` r
re2_count(c("yellowgreen", "steelblue", "maroon"), "e")
#> [1] 3 3 0
```

-   `re2_subset(x, pattern)` selects strings that match

``` r
re2_subset(c("yellowgreen", "steelblue", "goldenrod"), "ee")
#> [1] "yellowgreen" "steelblue"
```

-   `re2_match(x, pattern, simplify = FALSE)` extracts first matched
    substring

``` r
re2_match("ruby:1234 68 red:92 blue:", "(\\w+):(\\d+)")
#>      .0          .1     .2    
#> [1,] "ruby:1234" "ruby" "1234"
```

``` r
# Groups can be named:

re2_match(c("barbazbla", "foobar"), "(foo)|(?P<TestGroup>bar)baz")
#>      .0       .1    TestGroup
#> [1,] "barbaz" NA    "bar"    
#> [2,] "foo"    "foo" NA
```

``` r
# Use pre-compiled regular expression:

re <- re2_regexp("(foo)|(bar)baz", case_sensitive = FALSE)
re2_match(c("BaRbazbla", "Foobar"), re)
#>      .0       .1    .2   
#> [1,] "BaRbaz" NA    "BaR"
#> [2,] "Foo"    "Foo" NA
```

-   `re2_match_all(x, pattern)` extracts all matched substrings

``` r
re2_match_all("ruby:1234 68 red:92 blue:", "(\\w+):(\\d+)")
#> [[1]]
#>      .0          .1     .2    
#> [1,] "ruby:1234" "ruby" "1234"
#> [2,] "red:92"    "red"  "92"
```

-   `re2_replace(x, pattern, rewrite)` replaces first matched pattern in
    string

``` r
re2_replace("yabba dabba doo", "b+", "d")
#> [1] "yada dabba doo"
```

``` r
# Use groups in rewrite:

re2_replace("bunny@wunnies.pl", "(.*)@([^.]*)", "\\2!\\1")
#> [1] "wunnies!bunny.pl"
```

-   `re2_replace_all(x, pattern, rewrite)` replaces all matched patterns
    in string, or performs multiple replacements on each element of string.

``` r
re2_replace_all("yabba dabba doo", "b+", "d")
#> [1] "yada dada doo"
re2_replace_all(c("one", "two"), c("one" = "1", "1" = "2", "two" = "2"))
#> [1] "2" "2"
```

-   `re2_extract_replace(x, pattern, rewrite)` extracts and substitutes
    (ignores non-matching portions of x)

``` r
re2_extract_replace("bunny@wunnies.pl", "(.*)@([^.]*)", "\\2!\\1")
#> [1] "wunnies!bunny"
```

-   `re2_split(x, pattern, simplify = FALSE, n = Inf)` splits string
    based on pattern

``` r
re2_split("How vexingly quick daft zebras jump!", " quick | zebras")
#> [[1]]
#> [1] "How vexingly" "daft"         " jump!"
```

-   `re2_locate(x, pattern)` seeks the start and end of pattern in
    string

``` r
re2_locate(c("yellowgreen", "steelblue"), "l(b)?l")
#>      begin end
#> [1,]     3   4
#> [2,]     5   7
```

-   `re2_locate_all(x, pattern)` locates start and end of all
    occurrences of pattern in string

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

In all the above functions, regular-expression pattern is vectorized.

Regular-expression pattern can be compiled using
`re2_regexp(pattern, ...)`. Here are some of the options:

-   `case_sensitive`: Match is case-sensitive
-   `encoding`: UTF8 or Latin1
-   `literal`: Interpret pattern as literal, not regexp
-   `longest_match`: Search for longest match, not first match
-   `posix_syntax`: Restrict regexps to POSIX egrep syntax

`help(re2_regexp)` lists available options.

`re2_get_options(regexp_ptr)` returns a list of options stored in the
compiled regular-expression object.

## Regexp Syntax

re2 supports pearl style regular expressions (with extensions like \\d,
\\w, \\s, …) and provides most of the functionality of PCRE – eschewing
only backreferences and look-around assertions.

See [RE2 Syntax](https://github.com/girishji/re2/wiki/Syntax) for the
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
