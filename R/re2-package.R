#' re2: R interface to the Google's RE2 (C++) regular-expression library
#' 
#' @description
#'
#' Regular expression matching can be done in two ways: using
#'     recursive backtracking or using finite automata-based
#'     techniques.
#'
#' Perl, PCRE, Python, Ruby, Java, and many other languages rely on
#'     regular expression implementations based on recursive
#'     backtracking that can be excruciatingly slow. The time
#'     complexity can be exponential in some matching situations. In
#'     contrast, regular expression matching using finite
#'     automata-based techniques (like re2) can be simple and fast,
#'     guaranteeing linear time execution (in the length of the input
#'     string) and a fixed stack footprint (memory usage of execution
#'     engine can be configured).
#'
#' re2 supports pearl style regular expressions (with extensions like
#'     \\d, \\w, \\s, ...) and provides most of the functionality of
#'     PCRE -- eschewing only backreferences and look-around
#'     assertions. re2 is designed with safety in mind, and
#'     implemented with an explicit goal of being able to handle
#'     regular expressions from untrusted users without risk. Google
#'     has open sourced RE2 C++ project since 2010. See links to Russ
#'     Cox's articles in references section.
#'
#' @section Primary re2 functions:
#'
#' re2 supports four types of operations: matching, substring
#'     extraction, replacement, and extraction with replacement.
#' 
#' \bold{Matching} is provided by \code{\link{re2_match}} with logical option
#' set to TRUE. Matching regexp "(foo)|(bar)baz" on "barbazbla" will
#' return TRUE, for instance. Matching on "barbabla" will result in
#' FALSE.
#'
#' \bold{Substring extraction} is also provided by \code{\link{re2_match}}.
#' Matching regexp "(foo)|(bar)baz" on "barbazbla" will return
#' submatches '.0' = "barbaz", '.1' = NA, and '.2' = "bar". '.0' is
#' the entire matching text. '.1' is the first group, and so
#' on. Groups can also be named.
#'
#' \bold{Replacement} functionality is provided by \code{\link{re2_replace}}
#' and \code{\link{re2_global_replace}}. Replacing first occurrence of
#' pattern "b+" using replacement string "d" on text "yabba dabba doo"
#' will result in "yada dabba doo". Replacing globally will result in
#' "yada dada doo".
#' 
#' \bold{Extraction with replacement} is provided by
#' \code{\link{re2_extract}}. It is like replacement except that
#' non-matching text is ignored. Extracting "(.*)@([^.]*)" from text
#' "bunny@wunnies.pl", using replacement string "\\2!\\1" will result
#' in "wunnies!bunny". "\\1" and "\\2" are names of capturing
#' subgroups.
#'
#' In all the above functions regexp patterns can be precompiled and
#' reused for efficiency. See \code{\link{re2_re2}}. In addition, both
#' input parameter and result are vectorized.
#'
#' Primary re2 functions are:
#' \itemize{
#' \item \code{\link{re2_match}}
#' \item \code{\link{re2_replace}}
#' \item \code{\link{re2_global_replace}}
#' \item \code{\link{re2_extract}}
#' \item \code{\link{re2_re2}}
#' }
#' 
#' @section Secondary functions (not needed for most applications):
#' 
#' There are also a few utility functions:
#' \code{\link{re2_get_options}},
#' \code{\link{re2_capturing_group_names}},
#' \code{\link{re2_named_capturing_groups}},
#' \code{\link{re2_number_of_capturing_groups}},
#' \code{\link{re2_quote_meta}},
#' \code{\link{re2_max_submatch}},
#' \code{\link{re2_possible_match_range}},
#' \code{\link{re2_check_rewrite_string}}.
#'
#' @docType package
#' 
#' @author
#' Girish Palya <girishji@gmail.com>
#' 
#' @references
#' \itemize{
#' \item Regular Expression Matching Can Be Simple And Fast \url{https://swtch.com/~rsc/regexp/regexp1.html}
#' \item Regular Expression Matching: the Virtual Machine Approach \url{https://swtch.com/~rsc/regexp/regexp2.html}
#' \item Regular Expression Matching in the Wild \url{https://swtch.com/~rsc/regexp/regexp3.html}
#' \item RE2 Syntax \url{https://github.com/google/re2/wiki/Syntax}
#' \item RE2 C++ source \url{https://github.com/google/re2}
#' \item R source of RE2 \url{https://github.com/girishji/re2}
#' }
#' 
#' @name re2
#' 
#' @examples
#' ### Match
#' pat <- "(foo)|(?P<TestGroup>bar)baz"
#' re2_match("barbazbla", pat, l=TRUE)
#' 
#' # With precompiled regexp
#' re <- re2_re2("(foo)|(?P<TestGroup>bar)baz")
#' re2_match("barbazbla", re, l=TRUE)
#' 
#' # Vectorized input
#' s <- c("barbazbla", "foobar", "this is a test")
#' pat <- "(foo)|(?P<TestGroup>bar)baz"
#' re2_match(s, pat, l=TRUE)
#' 
#' ### Substring extraction
#' s <- c("barbazbla", "foobar", "this is a test")
#' pat <- "(foo)|(?P<TestGroup>bar)baz"
#' re2_match(s, pat)
#' 
#' ### Replace
#' re2_replace("yabba dabba doo", "b+", "d")
#' 
#' # Global replace
#' re2_global_replace("yabba dabba doo", "b+", "d")
#' 
#' ### Extract with replacement
#' re2_extract("bunny@wunnies.pl", "(.*)@([^.]*)", "\\2!\\1")
#'  
#' @keywords internal
"_PACKAGE"
#> [1] "_PACKAGE"
