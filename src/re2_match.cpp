// Copyright (c) 2021 Girish Palya
// License: https://github.com/girishji/re2/blob/main/LICENSE.md

#include <Rcpp.h>
#include <re2/re2.h>
#include "re2_re2proxy.h"

using namespace Rcpp;

SEXP re2_match_inner(StringVector text, const RE2& re2regexp,
                     bool logical, bool verbose,
		     size_t startpos,
		     size_t endpos,
		     int nsubmatch,
		     RE2::Anchor anchor);

//' Extract matched groups from a string
//'
//' @description
//' Vectorized over string. Match against a string using a regular
//'    expression and extract matched substrings.
//'
//' Matching regexp "(foo)|(bar)baz" on "barbazbla" will return
//'   submatches '.0' = "barbaz", '.1' = NA, and '.2' = "bar". '.0' is
//'   the entire matching text. '.1' is the first group,
//'   and so on. Groups can also be named.
//'
//' @inheritParams re2_match_cpp
//'
//' @return A character matrix mapping group names to matching
//'   substrings. Column '.0' is the entire matching text.
//'
//' @usage
//'   re2_match(string, pattern)
//'   re2_match(string, re2_regexp(pattern, ...))
//'
//' @examples
//' 
//' ## Substring extraction
//' s <- c("barbazbla", "foobar", "this is a test")
//' pat <- "(foo)|(?P<TestGroup>bar)baz"
//' re2_match(s, pat)
//' #
//' res <- re2_match(s, pat)
//' stopifnot(is.matrix(res))
//' stopifnot(colnames(res) == c(".0", ".1", "TestGroup"))
//' stopifnot(nrow(res) == 3, ncol(res) == 3)
//' stopifnot(res[2, 1:2] == c("foo", "foo"))
//' 
//' ## Compile regexp
//' re <- re2_regexp("(foo)|(BaR)baz", case_sensitive=FALSE)
//' re2_match(s, re)
//' 
//' @seealso
//'   \code{\link{re2_regexp}} for options to regular expression,
//'   \link{re2_syntax} for RE2 syntax, and 
//'   \code{\link{re2_match_cpp}} which this function wraps.
//'
// [[Rcpp::export]]
SEXP re2_match(StringVector string, SEXP pattern) {
  re2::RE2Proxy re2proxy(pattern);
  return re2_match_inner(string, re2proxy.get(), false, false,
			 0, SIZE_MAX,
			 re2proxy.get().NumberOfCapturingGroups() + 1,
			 RE2::UNANCHORED);

}
