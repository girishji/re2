#' re2: R interface to the Google's RE2 (C++) regular-expression library
#' 
#' @description
#'
#' Regular expression matching can be done in two ways: using
#'     recursive backtracking or using finite automata-based
#'     techniques.
#'
#' Perl, PCRE, Python, Ruby, Java, and many other languages rely on
#'     recursive backtracking for their regular expression implementations.
#'     The problem with this approach is that performance can degrade very 
#'     quickly. Time complexity can be exponential.
#'     In contrast, re2 uses finite automata-based techniques for regular 
#'     expression matching,
#'     guaranteeing linear time execution and a fixed stack footprint. See 
#'     links to Russ Cox's articles in references section.
#'
#' re2 supports pearl style regular expressions (with extensions like
#'     \\d, \\w, \\s, ...) and provides most of the functionality of
#'     PCRE -- eschewing only backreferences and look-around
#'     assertions. 
#'
#' @section Primary re2 functions:
#'
#' re2 supports three types of operations on a character vector: matching 
#'     (substring extraction), detection, and replacement.
#' 
#' Matching and substring extraction is provided by \code{\link{re2_match}} and 
#' \code{\link{re2_match_all}}.
#' Matching regexp "(foo)|(bar)baz" on "barbazbla" will return
#' submatches '.0' = "barbaz", '.1' = NA, and '.2' = "bar". '.0' is
#' the entire matching text. '.1' is the first group, and so
#' on. Groups can also be named.
#'
#' \code{\link{re2_detect}} finds the presence of a pattern in a string, like 
#' \code{grepl}.
#'
#' \code{\link{re2_replace}} and \code{\link{re2_replace_all}} will replace 
#' matched substring with replacement. Replacing first occurrence of
#' pattern "b+" using replacement string "d" on text "yabba dabba doo"
#' will result in "yada dabba doo". Replacing globally will result in
#' "yada dada doo". \code{\link{re2_extract_replace}} functions like 
#' \code{re2_replace} except that
#' non-matching text is ignored (not returned).
#'
#' In all the above functions regexp patterns can be precompiled and
#' reused. This greatly improves performance when the same regular expression 
#' is used repeatedly. See \code{\link{re2_regexp}}.
#'
#' List of re2 functions :
#' \itemize{
#' \item \code{\link{re2_match}}
#' \item \code{\link{re2_match_all}}
#' \item \code{\link{re2_split}}
#' \item \code{\link{re2_detect}}
#' \item \code{\link{re2_which}}
#' \item \code{\link{re2_subset}}
#' \item \code{\link{re2_locate}}
#' \item \code{\link{re2_locate_all}}
#' \item \code{\link{re2_count}}
#' \item \code{\link{re2_replace}}
#' \item \code{\link{re2_replace_all}}
#' \item \code{\link{re2_extract_replace}}
#' \item \code{\link{re2_regexp}}
#' \item \code{\link{re2_get_options}}
#' }
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
#' @keywords internal
"_PACKAGE"
#> [1] "_PACKAGE"
