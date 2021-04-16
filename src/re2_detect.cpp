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

//' Find the presence of a pattern in string(s)
//'
//' @description
//' Equivalent to grepl(pattern, x). Vectorized over
//'   string. For the equivalent of
//'   grep(pattern, x) use which(re2_detect(x, pattern)).
//'
//' @inheritParams re2_match_cpp
//'
//' @return A logical vector. TRUE if match is found, FALSE if not.
//'
//' @usage
//'   re2_detect(string, pattern)
//'   re2_detect(string, re2_regexp(pattern, ...)))
//'
//' @examples
//' 
//' ## Character vector input
//' s <- c("barbazbla", "foobar", "this is a test")
//' pat <- "(foo)|(bar)baz"
//' re2_detect(s, pat)
//' 
//' ## Use precompiled regexp
//' re <- re2_regexp("(foo)|(bAR)baz", case_sensitive=FALSE)
//' re2_detect(s, re)
//' stopifnot(re2_detect(s, re) == c(TRUE, TRUE, FALSE))
//'      
//' @seealso
//'   \code{\link{re2_regexp}} for options to regular expression,
//'   \link{re2_syntax} for RE2 syntax, and 
//'   \code{\link{re2_match_cpp}} which this function wraps.
//'
// [[Rcpp::export]]
SEXP re2_detect(StringVector string, SEXP pattern) {
  re2::RE2Proxy re2proxy(pattern);
  return re2_match_inner(string, re2proxy.get(), true, false,
			 0, SIZE_MAX, 0, RE2::UNANCHORED);
}
